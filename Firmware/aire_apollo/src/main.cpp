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
  String data[] = {String(hal->getMetricPressureEntry()), String(hal->getMetricVolumeEntry()), String(hal->getMetricVolumeExit())};
  com.data(data);
}

void setBPM(uint8_t CiclesPerMinute)
{
  inspTime = 60000.0 / float(bpm) * 0.25;
  espTime = 60000.0 / float(bpm) * 0.60;
  inspirationTimeout = 60000.0 / float(bpm) * 0.15;
  TRACE("BPM set: iTime:" + String(inspTime) + ", eTime:" + String(espTime) + "iTimeout:" + String(inspirationTimeout));
}

void setup()
{
  Serial.begin(115200);
  while (!Serial); // time to get serial running

  // Create hal layer with 
  hal = new ApolloHal(new ApolloBME(), new ApolloFlowSensor(), new ApolloFlowSensor(), new MksmValve(ENTRY_EV_PIN), new MksmValve(EXIT_EV_PIN));

  if(!hal->begin())
  {
    TRACE("ERROR intializing sensors!!");
    while(true);
  }
 
  delayTime = 0;
  setBPM(8);
  Serial.println();
}

void beginInspiration()
{
  hal->beginInspiration();
  lastInspirationStart = millis();
  status = INSPIRATION_CICLE;
}

void beginEspiration()
{
  hal->beginEspiration(); // hack para pruebas!!!
  lastEspirationStart = millis();
  status = ESPIRATION_CICLE;
}

bool checkForPatientInspiration()
{
  float pressureReference = 950;
  if ((pressureReference - hal->getMetricPressureEntry()) > INSPIRATION_THRESHOLD)
    return true;
  else
    return false;
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
    com.alert("PRESSURE ALERT");
  }

  int peep = getMetricPeep();

  // ¿se debe meter la detección de perdidas en el hal?
  float volExit = hal->getMetricVolumeExit();
  checkLeak(volc, volExit);
  
  calculateCompliance(pplat, peep);

  // envio de datos
  logData();
}
