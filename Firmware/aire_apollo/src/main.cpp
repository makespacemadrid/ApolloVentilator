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
#include "ApolloConfiguration.h"

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

//OJO a los contadores que se desbordan en algun momento!!!!
// Gestion rtc? / deteccion de desbordamiento?

//uint8_t power;
char logBuffer[50];

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

MechVentilation *ventilation;
ApolloConfiguration *configuration;
Comunications com = Comunications(configuration);

ApolloEncoder encoderRPM(12, 13, 0);
ApolloEncoder encoderTidal(10, 11, 0);
ApolloEncoder encoderPorcInspira(8, 9, 0);

Display display = Display();

int porcentajeInspiratorio = DEFAULT_POR_INSPIRATORIO;
int rpm = DEFAULT_RPM;
int vTidal = 0;

void setup()
{

  // Display de inicio
  display.init();
  display.writeLine(2, "     Apollo AIRE");
  delay(2000);
  Serial.begin(115200);
  // while (!Serial); // time to get serial running

  ApolloConfiguration *configuration = new ApolloConfiguration();
  while (!configuration->begin())
  {
    com.debug("setup", "Esperando configuración");
  }
  com.debug("setup", "Configuración recibida");

  // Create hal layer with
  MksmFlowSensor *fInSensor = new MksmFlowSensor();
  ApolloFlowSensor *fOutSensor = new MksmFlowSensor();
  ApolloPressureSensor *pSensor = new mksBME280(BME280_ADDR);
  ApolloValve *inValve = new MksmValve(ENTRY_EV_PIN);
  ApolloValve *outValve = new MksmValve(EXIT_EV_PIN);
  hal = new ApolloHal(pSensor, fInSensor, fOutSensor, inValve, outValve);

  while (!hal->begin())
  {
    TRACE("ERROR intializing HAL!!");
  }

  ventilation = new MechVentilation(hal, configuration);

  display.clear();
  display.writeLine(0, "RPM: " + String(configuration->getRpm()));
  display.writeLine(1, "Vol Tidal: " + String(configuration->getMlTidalVolumen()));
  display.writeLine(2, "Press PEEP: " + String(configuration->getPressionPeep()));
}

void logData()
{
  String data[] = {String(hal->getMetricPressureEntry()), String(hal->getMetricVolumeEntry()), String(hal->getMetricVolumeExit())};
  com.data(data, 3);
}

void loop()
{

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
  if (millis() % 100 == 0)
    logData();
  ventilation->update();

  if (encoderRPM.updateValue(&rpm))
  {
    configuration->setRpm(rpm);
    display.writeLine(0, "RPM: " + String(configuration->getRpm()));
  }

  if (encoderTidal.updateValue(&vTidal, 10))
  {
    configuration->setTidalVolume(vTidal);
    display.writeLine(1, "Vol Tidal: " + String(configuration->getMlTidalVolumen()));
  }

  if (encoderPorcInspira.updateValue(&porcentajeInspiratorio, 1))
  {
    configuration->setPorcentajeInspiratorio(porcentajeInspiratorio);
    display.writeLine(3, "% Insp: " + String(configuration->getPorcentajeInspiratorio()));
  }
  com.serialRead();
}
