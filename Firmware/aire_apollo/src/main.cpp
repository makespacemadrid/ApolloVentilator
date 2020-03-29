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

#define DEBUG         //Activar mensajes debug
#define INTFLOWSENSOR //Activar para usar los sensores de flujo por interrupcion.
#define LOCALCONTROLS // Display y encoders presentes.

#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <MsTimer2.h>

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

int rpm = DEFAULT_RPM;
int vTidal = DEFAULT_MIN_VOLUMEN_TIDAL;
int porcentajeInspiratorio = DEFAULT_POR_INSPIRATORIO;

ApolloHal *hal;
ApolloConfiguration *configuration = new ApolloConfiguration();
Comunications com = Comunications(configuration);
MechVentilation *ventilation;

#ifdef LOCALCONTROLS
ApolloEncoder encoderRPM(PIN_ENC_RPM_DT, PIN_ENC_RPM_CLK, PIN_ENC_RPM_SW);
ApolloEncoder encoderTidal(PIN_ENC_TIDAL_DT, PIN_ENC_TIDAL_CLK, PIN_ENC_TIDAL_SW);
ApolloEncoder encoderPorcInspira(PIN_ENC_PCTINS_DT, PIN_ENC_PCTINS_CLK, PIN_ENC_PCTINS_SW);
Display display = Display();
#endif

int calculateResistance(int ppeak, int pplat)
{
  return ppeak - pplat;
}

void checkLeak(float volEntry, float volExit) {}

int calculateCompliance(int pplat, int peep)
{
  return pplat - peep;
}

void ISR1ms() //Esta funcion se ejecuta cada 1ms para gestionar sensores/actuadores!
{             // OJO!!! no bloquear ni hacer nada muy costoso en tiempo!!!!!!
  hal->ISR1ms();
}

#ifdef INTFLOWSENSOR // Gestion de los sensores de flujo por interrupcion
void flowIn()
{
  hal->intakeFlowSensor()->pulse();
}

void flowOut()
{
  hal->exitFlowSensor()->pulse();
}
#endif

/// Porgram Begin

void logData()
{
  String pressure(hal->pressuresSensor()->readCMH2O());
  String intakeFlow(hal->intakeFlowSensor()->getFlow());
  String exitFlow(hal->exitFlowSensor()->getFlow());
  String intakeInstantFlow(hal->intakeFlowSensor()->getInstantFlow());
  String exitInstantFlow(hal->exitFlowSensor()->getInstantFlow());

  String data[] = {pressure, intakeFlow, exitFlow, intakeInstantFlow, exitInstantFlow};
  com.data(data, 5);
}

void setup()
{
  Serial.begin(115200);
  TRACE("INIT SETUP");

  TRACE("BEGIN CONFIG!");

  //ApolloConfiguration *configuration = new ApolloConfiguration();
  while (!configuration->begin())
  {
    com.debug("setup", "Esperando configuración");
  }
  com.debug("setup", "Configuración recibida");
  TRACE("CONFIG END");

  // Create hal layer with
  ApolloFlowSensor *fInSensor = new MksmFlowSensor();
  ApolloFlowSensor *fOutSensor = new MksmFlowSensor();
  ApolloPressureSensor *pSensor = new mksBME280(BME280_ADDR);
  ApolloValve *inValve = new MksmValve(ENTRY_EV_PIN);
  ApolloValve *outValve = new MksmValve(EXIT_EV_PIN);

  hal = new ApolloHal(pSensor, fInSensor, fOutSensor, inValve, outValve);

  TRACE("BEGIN HAL!");

  while (!hal->begin())
  {
    TRACE("HAL ERR!"); // No arrancamos si falla algun componente o podemos arrancar con algunos en fallo?
    delay(1000);
    //alarma!!
  }

  TRACE("HAL READY!");

  ventilation = new MechVentilation(hal, configuration);

#ifdef LOCALCONTROLS
  display.init();
  display.clear();
  display.writeLine(0, "RPM: " + String(configuration->getRpm()));
  display.writeLine(1, "Vol Tidal: " + String(configuration->getMlTidalVolumen()));
  display.writeLine(2, "Press PEEP: " + String(configuration->getPressionPeep()));
  display.writeLine(3, "% Insp: " + String(configuration->getPorcentajeInspiratorio()));
#endif

  //ISRs
  MsTimer2::set(1, ISR1ms); // Interrupcion de 1ms para el manejo de sensores/actuadores.
  MsTimer2::start();

#ifdef INTFLOWSENSOR
  attachInterrupt(digitalPinToInterrupt(ENTRY_FLOW_PIN), flowIn, RISING);
  attachInterrupt(digitalPinToInterrupt(EXIT_FLOW_PIN), flowOut, RISING);
#endif
  TRACE("SETUP COMPLETED!");
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

  if (millis() % LOG_INTERVAL == 0)
    logData();
  // gestion del ventilador
  ventilation->update();

#ifdef LOCALCONTROLS
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
#endif
  //com.serialRead();
}
