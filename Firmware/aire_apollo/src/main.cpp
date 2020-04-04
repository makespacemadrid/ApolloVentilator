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
//#define LOCALCONTROLS // Display y encoders presentes.

#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <MsTimer2.h>

#include "trace.h"
#include "ApolloHal.h"
#include "mksBME280.h"
#include "cheapValve.h"
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
Comunications *com = new Comunications(configuration);
ApolloAlarms *alarms = new ApolloAlarms(com, PIN_BUZZER, true);
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

void setRampsPWMFreq()
{
  /*
  timer 0 (controls pin 13, 4);
  timer 1 (controls pin 12, 11);
  timer 2 (controls pin 10, 9);
  timer 3 (controls pin 5, 3, 2);
  timer 4 (controls pin 8, 7, 6);

    prescaler = 1 ---> PWM frequency is 31000 Hz
    prescaler = 2 ---> PWM frequency is 4000 Hz
    prescaler = 3 ---> PWM frequency is 490 Hz (default value)
    prescaler = 4 ---> PWM frequency is 120 Hz
    prescaler = 5 ---> PWM frequency is 30 Hz
    prescaler = 6 ---> PWM frequency is <20 Hz

  */
  int myEraser = 7;             // this is 111 in binary and is used as an eraser
  TCCR2B &= ~myEraser;   // this operation (AND plus NOT),  set the three bits in TCCR2B to 0
  int myPrescaler = 3;         // this could be a number in [1 , 6]. In this case, 3 corresponds in binary to 011.
  TCCR2B |= myPrescaler;  //this operation (OR), replaces the last three bits in TCCR2B with our new value 011
}

/// Porgram Begin



void logData()
{
  String pressure(hal->pressuresSensor()->readCMH2O());
  String intakeFlow(hal->intakeFlowSensor()->getFlow());
  //String intakeFlow(0);
//  String exitFlow(hal->exitFlowSensor()->getFlow());
  String exitFlow(0);
  String intakeInstantFlow(hal->intakeFlowSensor()->getInstantFlow());
  //String exitInstantFlow(hal->exitFlowSensor()->getInstantFlow());
  //String intakeInstantFlow(0);
  String exitInstantFlow(0);
//  String intakeValve(hal->exitValve()->status());
  String intakeValve(hal->intakeValve()->status());
  String ExitValve(hal->exitValve()->status());

  String data[] = {pressure,intakeInstantFlow,exitInstantFlow,intakeFlow,exitFlow,intakeValve,ExitValve};
  com->data(data, 7);
}

void setup()
{
  Serial.begin(115200);
  TRACE("INIT SETUP");
  alarms->begin();

  TRACE("BEGIN CONFIG!");

  //ApolloConfiguration *configuration = new ApolloConfiguration();
  while (!configuration->begin())
  {
    com->debug("setup", "Esperando configuración");
  }
  com->debug("setup", "Configuración recibida");
  TRACE("CONFIG END");

  // Create hal layer with
  ApolloFlowSensor *fInSensor = new MksmFlowSensor();
  ApolloFlowSensor *fOutSensor = new MksmFlowSensor();
  ApolloPressureSensor *pSensor = new mksBME280(BME280_ADDR);
  ApolloValve *inValve = new cheapValve(ENTRY_EV_PIN);
  ApolloValve *outValve = new cheapValve(EXIT_EV_PIN);

  hal = new ApolloHal(pSensor, fInSensor, fOutSensor, inValve, outValve, alarms);

  TRACE("BEGIN HAL!");

  while (!hal->begin())
  {
    TRACE("HAL ERR!"); // No arrancamos si falla algun componente o podemos arrancar con algunos en fallo?
    delay(1000);
    //alarma!!
  }

  TRACE("HAL READY!");

  ventilation = new MechVentilation(hal, configuration, alarms);

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

  //  if (millis() % LOG_INTERVAL == 0)
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
  if (com.serialRead())
  {
    display.writeLine(0, "RPM: " + String(configuration->getRpm()));
    display.writeLine(1, "Vol Tidal: " + String(configuration->getMlTidalVolumen()));
    display.writeLine(3, "% Insp: " + String(configuration->getPorcentajeInspiratorio()));
  }
#else
  com->serialRead();
#endif
  alarms->check();
}
