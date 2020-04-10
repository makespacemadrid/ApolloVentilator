/***************************************************************************
Apollo firmware



*Compatible Valves:
-MUR servoValve (Maker Grade)
-PWM capable Valves
-cheap nonPWM Valves (not recommended)

*Compatible Flow sensors:
-Interrupt based Flow sensor (use interrupt enabled pin and uncoment INTFLOWSENSOR)

*Compatible Pressure Sensors:
-BME280 (single one not recommended,no differential pressure!)
-DUAL_BME280 (Maker Grade)
-HoneywellTruStability (Medical Grade)



 ***************************************************************************/

 #define DEBUG         //Activar mensajes debug
 //#define INTFLOWSENSOR //Activar solo para usar los sensores de flujo por interrupcion.(NO NECESARIO PARA EL RESTO DE SENSORES DE FLUJO)
 #define LOCALCONTROLS // Display y encoders presentes.


#include "../include/defaults.h"
#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <MsTimer2.h>

#include "trace.h"
#include "ApolloHal.h"

#include "Sensor/Pressure/mksBME280.h"
#include "Valve/cheapValve.h"
#include "Valve/pwmValve.h"
#include "Valve/servoValve.h"
#include "Sensor/FlowSensor/MksmFlowSensor.h"
#include "Comunications.h"
#include "MechVentilation.h"
#include "LocalControl/LocalEncoder.h"
#include "LocalControl/LocalDisplay.h"
#include "ApolloConfiguration.h"
#include "Valve/StepperNema.h"
#include "Valve/LedTest.h"
#include "Sensor/Pressure/AnalogLinearPressure.h"
#include "Sensor/Pressure/DummyPressure.h"

int rpm = DEFAULT_RPM;
int vTidal = DEFAULT_MIN_VOLUMEN_TIDAL;
int porcentajeInspiratorio = DEFAULT_POR_INSPIRATORIO;



uint16_t  logTimeCounter = 0;
bool      sendLog        = false;



ApolloHal *hal;
ApolloConfiguration *configuration = new ApolloConfiguration();
Comunications *com = new Comunications(configuration);
ApolloAlarms *alarms = new ApolloAlarms(com, PIN_BUZZER, true);
MechVentilation *ventilation;

#ifdef LOCALCONTROLS
  LocalEncoder encoderRPM(PIN_ENC_RPM_DT, PIN_ENC_RPM_CLK, PIN_ENC_RPM_SW);
  LocalEncoder encoderTidal(PIN_ENC_TIDAL_DT, PIN_ENC_TIDAL_CLK, PIN_ENC_TIDAL_SW);
  LocalEncoder encoderPorcInspira(PIN_ENC_PCTINS_DT, PIN_ENC_PCTINS_CLK, PIN_ENC_PCTINS_SW);
  LocalDisplay display = LocalDisplay();
#endif


//implementar en el ventilador ??
/*
int calculateResistance(int ppeak, int pplat)
{
  return ppeak - pplat;
}

void checkLeak(float volEntry, float volExit) {}

int calculateCompliance(int pplat, int peep)
{
  return pplat - peep;
}

*/

void ISR1ms() //Esta funcion se ejecuta cada 1ms para gestionar sensores/actuadores!
{             // OJO!!! no bloquear ni hacer nada muy costoso en tiempo!!!!!!
  hal->ISR1ms();
  if(++logTimeCounter >= LOG_INTERVAL) {sendLog = true;logTimeCounter = 0;}
}

#ifdef INTFLOWSENSOR // Gestion de los sensores de flujo por interrupcion
  void flowIn()
  {
    hal->flowIn();
  }

  void flowOut()
  {
    hal->flowIn();
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
  int myEraser = 7;      // this is 111 in binary and is used as an eraser
  TCCR2B &= ~myEraser;   // this operation (AND plus NOT),  set the three bits in TCCR2B to 0
  int myPrescaler = 3;   // this could be a number in [1 , 6]. In this case, 3 corresponds in binary to 011.
  TCCR2B |= myPrescaler; //this operation (OR), replaces the last three bits in TCCR2B with our new value 011
}

/// Porgram Begin

void logData()
{
  String pressure(hal->getPresureIns(false));
  String intakeFlow(hal->getEntryFlow());
  //String intakeFlow(0);
  //  String exitFlow(hal->exitFlowSensor()->getFlow());
  String exitFlow(0);
  String intakeInstantFlow(hal->getEntryInstantFlow());
  //String exitInstantFlow(hal->exitFlowSensor()->getInstantFlow());
  //String intakeInstantFlow(0);
  String exitInstantFlow(0);
  //  String intakeValve(hal->exitValve()->status());
  String intakeValve(hal->getEntryValveStatus());
  String ExitValve(hal->getExitValveStatus());
  String Status(ventilation->getStatus());

  String data[] = {pressure, intakeInstantFlow, exitInstantFlow, intakeFlow, exitFlow, intakeValve, ExitValve,Status};
  com->data(data, 8);
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
  ApolloFlowSensor *fInSensor   = new MksmFlowSensor();
  ApolloFlowSensor *fOutSensor  = new MksmFlowSensor();
  ApolloPressureSensor *pSensor = new DummyPressure();

//  ApolloValve* inValve  = new servoValve(ENTRY_EV_PIN,3,100);
//  ApolloValve* outValve = new servoValve(EXIT_EV_PIN,3,100);

//El penultimo valor es cuantos pasos hay desde el final de carrera hasta apretar del todo el boton.
//El ultimo valor es cuantos pasos hay desde el final de carrera hasta que empiezas a apretar el boton.
  ApolloValve *inValve  = new StepperNema(STEPER1_ENABLE,STEPER1_DIR,STEPER1_STEP,STEPER1_ENDSTOP,0,2900,0);
  ApolloValve *outValve = new StepperNema(STEPER2_ENABLE,STEPER2_DIR,STEPER2_STEP,STEPER2_ENDSTOP,0,2900,0);


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


  // gestion del ventilador
  ventilation->update();
  if (sendLog) {logData();sendLog = false;}
  alarms->check();


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
  if (com->serialRead())
  {
    display.writeLine(0, "RPM: " + String(configuration->getRpm()));
    display.writeLine(1, "Vol Tidal: " + String(configuration->getMlTidalVolumen()));
    display.writeLine(3, "% Insp: " + String(configuration->getPorcentajeInspiratorio()));
  }
#else
  com->serialRead();
#endif
}
