/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/
#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "trace.h"
#include "ApolloHal.h"
#include "ApolloBME.h"
#include "MksmValve.h"
#include "Comunications.h"
#include "MechVentilation.h";
#define DEBUG

#define ENTRY_EV_PIN 10 //ElectroValvula - Entrada
#define EXIT_EV_PIN 9   //ElectroValvula - Salida

#define ENTRY_FLOW_PIN 4 //Sensor de Flujo - Entrada
#define EXIT_FLOW_PIN 5  //Sendor de Flujo - Salida

//#define PRESSURE_SENSOR_PIN      ??
#define BME280_ADDR 0x76

#define SEALEVELPRESSURE_HPA (1013.25)

#define INSPIRATION_TIME 1000
#define ESPIRATION_TIME 4000
#define INSPIRATION_THRESHOLD 10 //Descenso en la presion que marca el inicio de la respiracion

Adafruit_BME280 bme; // I2C
ApolloHal *hal;

unsigned long delayTime;

float volmax;
float presmax;
float o2insp;
float limitO2;
float co2esp;
float limitCO2;
float volumeentry;
float volumeexit;
int ppeak = 23;
int pplat = 18;
int peep = 5;

// Parameters
int weight = 70;       // kg
int volc = weight * 6; // weight * 6u8 (mL) Volumen tidal
int bpm = 10;          // breaths per minute
int voli = volc * bpm;
//int timep; // good to have, pause
unsigned long lastInspirationStart = 0; // (s)
unsigned long lastEspirationStart = 0;  // (s)
uint16_t inspirationTimeout = 0;
uint16_t inspTime = 0;
uint16_t espTime = 0;

//OJO a los contadores que se desbordan en algun momento!!!!
// Gestion rtc? / deteccion de desbordamiento?

//uint8_t power;
char logBuffer[50];

enum respiratorStatus
{
  RESPIRATOR_PAUSED,
  WAIT_FOR_INSPIRATION,
  INSPIRATION_CICLE,
  ESPIRATION_CICLE,
};

respiratorStatus status = WAIT_FOR_INSPIRATION;
Comunications com = Comunications();

// BEGIN Sensors and actuators

// Open Entry valve
void openEntryEV()
{
  digitalWrite(ENTRY_EV_PIN, 1);
}

// Close Entry valve
void closeEntryEV()
{
  digitalWrite(ENTRY_EV_PIN, 0);
}

// Get Entry EV state
int getEntryEVState()
{
  return digitalRead(ENTRY_EV_PIN);
}

// Open exit EV
void openExitEV()
{
  digitalWrite(EXIT_EV_PIN, 1);
}

// Close exit EV
void closeExitEV()
{
  digitalWrite(EXIT_EV_PIN, 0);
}

// Get the status of the Exit EV (Open/Close)
// To be change  depending on the EV type Full open, Servo, Solenoid...
int getExitEVState()
{
  return digitalRead(EXIT_EV_PIN);
}

// Get metric from entry flow mass sensor
float getMetricVolumeEntry()
{
  float v;
  //v=analogRead(ENTRY_FLOW_PIN)*0.0049F;

  // Simulate sensor, if valve is open just return 52 l/m
  // to see if it helps displaying graphs

  if (status == INSPIRATION_CICLE)
    v = 52.00F;
  else
    v = 5.00F;

  return v;
}

// Get metric from exit flow mass sensor
float getMetricVolumeExit()
{

  float v = analogRead(EXIT_FLOW_PIN) * 0.0049F;
  return v;
}

// Get metric from pressure sensor in mBar
float getMetricPressureEntry()
{
  float val = bme.readPressure();
  return val / 100.0F; // hpa
}

// END sensors and actuator

int getMetricPpeak() { return 22; }
int getMetricPplat() { return 22; }

int calculateResistance(int ppeak, int pplat)
{
  return ppeak - pplat;
}

int getMetricPeep() { return 22; }

void checkLeak(float volEntry, float volExit) {}
float getMetricVolMax() { return 22; }
float getMetricPresMax() { return 22; }

int calculateCompliance(int pplat, int peep)
{
  return pplat - peep;
}

void logData()
{
  String data[] = {String(getMetricPressureEntry()), String(getMetricVolumeEntry()), String(getMetricVolumeExit())};
  com.data(data);
}

void setBPM(uint8_t CiclesPerMinute)
{
  inspTime = 60000.0 / float(bpm) * 0.25;
  espTime = 60000.0 / float(bpm) * 0.60;
  inspirationTimeout = 60000.0 / float(bpm) * 0.15;
  TRACE("BPM set: iTime:" + String(inspTime) + ", eTime:" + String(espTime) + "iTimeout:" + String(inspirationTimeout));
}
MechVentilation *ventilation;

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    ; // time to get serial running

  hal = new ApolloHal(new ApolloBME(), new ApolloFlowSensor(), new MksmValve(ENTRY_EV_PIN), new MksmValve(EXIT_EV_PIN));

  // BME280

  if (!bme.begin(BME280_ADDR))
  {
    TRACE("BME280 sensor not found!!!");
    TRACE("HALT!");
    //while (1);
  }

  // set max sampling for pressure sensor
  bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::SAMPLING_X16,
                  Adafruit_BME280::SAMPLING_X1,
                  Adafruit_BME280::FILTER_OFF,
                  Adafruit_BME280::STANDBY_MS_0_5);
  //

  delayTime = 0;
  setBPM(8);
  Serial.println();
  //float speedIns, speedEsp, tCiclo, tIns, tEsp;
  int porcentajeInspiratorio = DEFAULT_POR_INSPIRATORIO;
  int rpm = DEFAULT_RPM;
  // CÁLCULO: CONSTANTES DE TIEMPO INSPIRACION/ESPIRACION
  // =========================================================================
  //display.writeLine(0, "Tins  | Tesp");
  /**MechVentilation::calcularCicloInspiratorio(&tIns, &tEsp, &tCiclo, porcentajeInspiratorio, rpm);
  //display.writeLine(1, String(tIns) + " s | " + String(tEsp) + " s");
  Serial.println("Tiempo del ciclo (seg):" + String(tCiclo));
  Serial.println("Tiempo inspiratorio (seg):" + String(tIns));
  Serial.println("Tiempo espiratorio (seg):" + String(tEsp));
*/
  int vTidal = MechVentilation::calcularVolumenTidal(170, 1);
  //int ventilationCycle_WaitBeforeInsuflationTime = 800;
  ventilation = new MechVentilation(hal, vTidal, rpm, porcentajeInspiratorio);
  /** ventilation->start();*/
}

void beginInspiration()
{
  openEntryEV();
  closeExitEV();
  lastInspirationStart = millis();
  status = INSPIRATION_CICLE;
}

void beginEspiration()
{
  closeEntryEV();
  openExitEV(); // hack para pruebas!!!
  lastEspirationStart = millis();
  status = ESPIRATION_CICLE;
}

bool checkForPatientInspiration()
{
  float pressureReference = 950;
  if ((pressureReference - getMetricPressureEntry()) > INSPIRATION_THRESHOLD)
    return true;
  else
    return false;
}

// Send alarm to GUI by Serial
void alarm(const char *value)
{
  // Raise alarm localy may be with sound?
  // local sound alarm?
  Serial.println("ALARM: " + String(value));
}

void loop()
{
  // Control del ciclo de respiracion
  if (status == RESPIRATOR_PAUSED)
  {
    TRACE("PAUSED...");
  }
  else if (status == WAIT_FOR_INSPIRATION)
  {
    if (checkForPatientInspiration())
    {
      TRACE("Insp DETECTED!");
      beginInspiration();
    }
    else if (millis() - (lastInspirationStart + inspTime + espTime) >= inspirationTimeout)
    {
      TRACE("FORCING Insp");
      beginInspiration();
    }
  }
  else if (status == INSPIRATION_CICLE)
  {
    if (millis() - lastInspirationStart >= inspTime)
    {
      TRACE("BeginEspiration");
      beginEspiration();
    }
  }
  else if (status == ESPIRATION_CICLE)
  {
    if (millis() - lastEspirationStart >= espTime)
    {
      status = WAIT_FOR_INSPIRATION;
      TRACE("Cicle Done! :), Wait for inspiration");
    }
  }

  //Comprobacion de alarmas

  ppeak = getMetricPpeak();
  pplat = getMetricPplat();

  calculateResistance(ppeak, pplat);

  if (ppeak > 40)
  {
    alarm("PRESSURE ALERT");
  }

  int peep = getMetricPeep();
  float volExit = getMetricVolumeExit();
  checkLeak(volc, volExit);
  calculateCompliance(pplat, peep);

  // envio de datos
  logData();
  ventilation->update();
}
