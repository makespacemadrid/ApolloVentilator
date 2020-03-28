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
#include "mksBME280.h"
#include "MksmValve.h"
#include "MksmFlowSensor.h"
#include "Comunications.h"
#include "MechVentilation.h"
#include "ApolloEncoder.h"
#include "Display.h"

#define DEBUG

#define ENTRY_EV_PIN 10 //ElectroValvula - Entrada
#define EXIT_EV_PIN 8   //ElectroValvula - Salida

#define ENTRY_FLOW_PIN 4 //Sensor de Flujo - Entrada
#define EXIT_FLOW_PIN 5  //Sendor de Flujo - Salida

//#define PRESSURE_SENSOR_PIN      ??
#define BME280_ADDR 0x76

#define SEALEVELPRESSURE_HPA (1013.25)

#define INSPIRATION_TIME 1000
#define ESPIRATION_TIME 4000
#define INSPIRATION_THRESHOLD 10 //Descenso en la presion que marca el inicio de la respiracion

ApolloHal *hal;

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
//char logBuffer[50];

Comunications com = Comunications();

/*
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
*/

void logData()
{




  String pressure(hal->getMetricPressureEntry());
  String intakeFlow(hal->getMetricVolumeEntry());
  String exitFlow(hal->getMetricVolumeExit());
  String data[] = {pressure,intakeFlow,exitFlow};
  com.data(data,3);
}


MechVentilation *ventilation;
//ApolloEncoder encoderRPM(12, 13, 0);
//ApolloEncoder encoderTidal(10, 11, 0);
//ApolloEncoder encoderPorcInspira(8, 9, 0);

//Display display = Display();

int porcentajeInspiratorio = DEFAULT_POR_INSPIRATORIO;
int rpm = DEFAULT_RPM;
int vTidal = 0;

void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("INIT");
  //Serial.flush();
  // Display de inicio
//  display.init()
//  display.writeLine(2, "     Apollo AIRE");
//  delay(2000);

  // while (!Serial); // time to get serial running

  // Create hal layer with
  MksmFlowSensor*       fInSensor   = new MksmFlowSensor();
  ApolloFlowSensor*     fOutSensor  = new MksmFlowSensor();
  ApolloPressureSensor* pSensor     = new mksBME280(BME280_ADDR);
  ApolloValve*          inValve     = new MksmValve(ENTRY_EV_PIN);
  ApolloValve*          outValve    = new MksmValve(EXIT_EV_PIN);

  Serial.println("INIT HAL!");
  //Serial.flush();

  hal = new ApolloHal(pSensor,fInSensor,fOutSensor,inValve,outValve);

  Serial.println("begin HAL!");
  Serial.flush();

  while(!hal->begin())
  {
    //TRACE("ERROR intializing HAL!!");
    Serial.println("PROBLEMA HALLLLLL!!!");
    Serial.flush();
  }

  Serial.println("HAL LISTO!");
  Serial.flush();
  ventilation = new MechVentilation(hal, vTidal, rpm, porcentajeInspiratorio);

//  display.clear();
//  display.writeLine(0, "RPM: " + String(ventilation->getRpm()));
//  display.writeLine(1, "Vol Tidal: " + String(ventilation->getTidalVolume()));
//  display.writeLine(2, "Press PEEP: " + String(ventilation->getPressionPeep()));
  Serial.println("Setup completado!");
  Serial.flush();
}


void loop()
{
//  Serial.println("LOOOOP");
// Serial.flush();

  //Comprobacion de alarmas

//  ppeak = getMetricPpeak();
//  pplat = getMetricPplat();

//  calculateResistance(ppeak, pplat);

//  if (ppeak > 40)
//  {
//    com.alert("PRESSURE ALERT");
//  }

//  int peep = getMetricPeep();
// ¿se debe meter la detección de perdidas en el hal?
//  float volExit = hal->getMetricVolumeExit();
//  checkLeak(volc, volExit);
//  calculateCompliance(pplat, peep);



  // envio de datos
  if(millis()%100 ==0)logData();
  ventilation->update();
/*
  if (encoderRPM.updateValue(&rpm))
  {
    ventilation->setRpm(rpm);
//    display.writeLine(0, "RPM: " + String(ventilation->getRpm()));
  }

  if (encoderTidal.updateValue(&vTidal, 10))
  {
    ventilation->setTidalVolume(vTidal);
//    display.writeLine(1, "Vol Tidal: " + String(ventilation->getTidalVolume()));
  }

  if (encoderPorcInspira.updateValue(&porcentajeInspiratorio, 1))
  {
    ventilation->setPorcentajeInspiratorio(porcentajeInspiratorio);
//    display.writeLine(3, "% Insp: " + String(ventilation->getporcentajeInspiratorio()));
  }
*/
}
