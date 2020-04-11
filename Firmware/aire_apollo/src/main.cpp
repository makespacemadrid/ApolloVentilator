/***************************************************************************
Apollo firmware



*Compatible Valves:
-cheap nonPWM Valves (not recommended)
-MUR servoValve (Maker Grade)
-PWM capable Valves (Depending on the valve could be Medical grade)
-Nema based pinch valve     (Maker && Medical grade)
.Nema based scuba regulator (Maker && Medical grade)

*Compatible Flow sensors:
-Interrupt based Flow sensor (use interrupt enabled pin and uncoment INTFLOWSENSOR)

*Compatible Pressure Sensors:
-BME280 (single one not recommended,no differential pressure!)
-DUAL_BME280 (Maker Grade)
-HoneywellTruStability (Medical Grade)



 ***************************************************************************/

 #define DEBUG         //Activar mensajes debug - Algo pasa con el TRACE()
// #define INTFLOWSENSOR //Activar solo para usar los sensores de flujo por interrupcion.(NO NECESARIO PARA EL RESTO DE SENSORES DE FLUJO)
// #define LOCALCONTROLS // Display y encoders presentes.


#include "../include/defaults.h"
#include "trace.h"

//Arduino libraries
#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <FlexiTimer2.h>

//Apollo clases
#include "ApolloHal.h"
#include "Comunications.h"
#include "MechVentilation.h"
#include "ApolloConfiguration.h"
//Interfaces

//Sensors
#include "Valve/cheapValve.h"
#include "Valve/pwmValve.h"
#include "Valve/servoValve.h"
#include "Valve/StepperNema.h"
#include "Valve/LedTest.h"
#include "Sensor/FlowSensor/MksmFlowSensor.h"
#include "Sensor/Pressure/mksBME280.h"
#include "Sensor/Pressure/AnalogLinearPressure.h"
#include "Sensor/Pressure/DummyPressure.h"
#include "LocalControl/LocalEncoder.h"
#include "LocalControl/LocalDisplay.h"


int rpm     = DEFAULT_RPM;
int vTidal  = DEFAULT_MIN_VOLUMEN_TIDAL;
int porcentajeInspiratorio = DEFAULT_POR_INSPIRATORIO;


uint16_t  logTimeCounter = 0;
bool      sendLog        = false;

ApolloHal           *hal;
MechVentilation     *ventilation;
ApolloConfiguration *configuration = new ApolloConfiguration();
Comunications       *com           = new Comunications(configuration);
ApolloAlarms        *alarms        = new ApolloAlarms(com, PIN_BUZZER, true);


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

uint16_t c1 = 0;
uint16_t c2 = 0;

void ISR1ms() //Esta funcion se ejecuta cada 1ms para gestionar sensores/actuadores!
{             // OJO!!! no bloquear ni hacer nada muy costoso en tiempo!!!!!!
  c1++;
  hal->ISR1ms();
  if(++logTimeCounter >= LOG_INTERVAL) {sendLog = true;logTimeCounter = 0;}
}

void ISRHighFreq()//interrupcion cada 100 microsegundos
{ //Aqui si que no se puede hacer casi nada o el programa petara por todas partes!
  //En principio usar solo para la gestion de los steppers
  hal->ISR1ms();
  c2++;
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
{//UNTESTED!
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
  ApolloFlowSensor     *fInSensor   = new MksmFlowSensor();
  ApolloFlowSensor     *fOutSensor  = new MksmFlowSensor();
  ApolloPressureSensor *pSensor     = new DummyPressure();

//  ApolloValve* inValve  = new servoValve(ENTRY_EV_PIN,3,100);
//  ApolloValve* outValve = new servoValve(EXIT_EV_PIN,3,100);

//El penultimo valor es cuantos pasos hay desde el final de carrera hasta apretar del todo el boton.
//El ultimo valor es cuantos pasos hay desde el final de carrera hasta que empiezas a apretar el boton.
  StepperNema *inStepper  = new StepperNema(STEPER1_ENABLE,STEPER1_DIR,STEPER1_STEP,STEPER1_ENDSTOP,NO_PIN,12000,8500,5400);
  StepperNema *outStepper = new StepperNema(STEPER2_ENABLE,STEPER2_DIR,STEPER2_STEP,NO_PIN,NO_PIN,2900,0);
  inStepper->setMinEndStopPressedState(HIGH);
  outStepper->setMinEndStopPressedState(LOW);

  ApolloValve *inValve  = inStepper;
  ApolloValve *outValve = outStepper;


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
  FlexiTimer2::set(10, 1.0/10000,ISR1ms); // Interrupcion de 1ms para el manejo de sensores/actuadores.
//  FlexiTimer2::set(1 , 1.0/10000,ISRHighFreq); // Interrupcion de 1ms para el manejo de sensores/actuadores.
  FlexiTimer2::start();

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
//delay(10);
//Serial.println("c1:"+String(c1)+"c2 "+String(c2));
}
