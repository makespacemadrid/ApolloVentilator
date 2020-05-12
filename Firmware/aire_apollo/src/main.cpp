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

// #define DEBUG         //Activar mensajes debug - Algo pasa con el Serial.println()
// #define INTFLOWSENSOR //Activar solo para usar los sensores de flujo por interrupcion.(NO NECESARIO PARA EL RESTO DE SENSORES DE FLUJO)



#include "../include/defaults.h"
#include "trace.h"
#include "strings.h"

//Arduino libraries
#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
//include <FlexiTimer2.h>

//Apollo clases
#include "ApolloHal.h"
#include "Comunications.h"
#include "MechVentilation.h"
#include "ApolloConfiguration.h"
//Interfaces

//Sensors
#include "Valve/cheapValve.h"
#include "Valve/pwmValve.h"
#include "Valve/StepperNema.h"
#include "Valve/servoValve.h"
#include "Sensor/FlowSensor/MksmFlowSensor.h"
#include "Sensor/FlowSensor/Sfm3000FlowSensor.h"
#include "Sensor/Pressure/mksBME280.h"
#include "Sensor/Pressure/mksBME280diff.h"
#include "Sensor/Pressure/AnalogLinearPressure.h"
#include "Sensor/Pressure/DummyPressure.h"




int rpm     = DEFAULT_RPM;
int vTidal  = DEFAULT_MIN_VOLUMEN_TIDAL;
int porcentajeInspiratorio = DEFAULT_POR_INSPIRATORIO;


ApolloHal hal;
//MechVentilation     *ventilation;
//ApolloConfiguration *configuration = new ApolloConfiguration();
//Comunications       *com           = new Comunications(configuration);
//ApolloAlarms        *alarms        = new ApolloAlarms(com, PIN_BUZZER, true);


unsigned long lastVentilatorUpdate;


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

#ifdef INTFLOWSENSOR // Gestion de los sensores de flujo por interrupcion
  void flowIn()
  {
    hal.flowIn();
  }

  void flowOut()
  {
    hal.flowIn();
  }
#endif

/*
void setRampsPWMFreq()
{//UNTESTED!
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


  int myEraser = 7;      // this is 111 in binary and is used as an eraser
  TCCR2B &= ~myEraser;   // this operation (AND plus NOT),  set the three bits in TCCR2B to 0
  int myPrescaler = 3;   // this could be a number in [1 , 6]. In this case, 3 corresponds in binary to 011.
  TCCR2B |= myPrescaler; //this operation (OR), replaces the last three bits in TCCR2B with our new value 011
}
*/
/// Porgram Begin



//uint8_t test[500];
void setup()
{
  Serial.begin(SERIAL_BAUDS);
  hal.debug("\nINIT");
//  alarms->begin();

/*
  ApolloConfiguration *configuration = new ApolloConfiguration();
  while (!configuration->begin())
  {
    com->debug("setup", "Esperando configuración");
  }
  com->debug("setup", "Configuración recibida");
  Serial.println("CONFIG END");
*/

//Valvulas

//  ApolloValve* inValve  = new servoValve(ENTRY_EV_PIN,0,80);
//  ApolloValve* outValve = new servoValve(EXIT_EV_PIN,2,75);
  StepperNema *inStepper  = new StepperNema(STEPPER1_STEP,STEPPER1_DIR,STEPPER1_ENDSTOP, STEPPER1_ENABLE , LOW ,5400);
  inStepper->setMinEndStopPressedState(HIGH);
  inStepper->enableMinEndstopPullup();
  inStepper->setMaxRPM(12);
  inStepper->setMicrosteps(2);
  inStepper->setOpenPos(700);
  inStepper->setClosedPos(1650);

  StepperNema *outStepper = new StepperNema(STEPPER2_STEP,STEPPER2_DIR,STEPPER2_ENDSTOP, STEPPER2_ENABLE);
  outStepper->setMinEndStopPressedState(HIGH);
  outStepper->enableMinEndstopPullup();
  outStepper->setMaxRPM(300);
  outStepper->setMicrosteps(8);
  outStepper->setOpenPos(100);
  outStepper->setClosedPos(400);

  ApolloValve *inValve  = inStepper;
  ApolloValve *outValve = outStepper;

  hal.addValves(inValve,outValve);

//Sensor Presion
  ApolloPressureSensor *pSensor = new mksBME280();

  hal.addPressureSensor(pSensor);

//Sensores de FLOW
  ApolloFlowSensor     *fInSensor   = new Sfm3000FlowSensor(100,0x40);
  ApolloFlowSensor     *fOutSensor  = new Sfm3000FlowSensor(100,0x40);

  hal.addFlowSensors(fInSensor, fOutSensor);

  hal.debug("BEGIN HAL!");
  while (!hal.begin())
  {
    hal.debug("HAL BEGIN ERR!"); // No arrancamos si falla algun componente o podemos arrancar con algunos en fallo?
    delay(1000);
    //alarma!!
  }

  while (!hal.test())
  {
    hal.debug("HAL TEST ERR!"); // No arrancamos si falla algun componente o podemos arrancar con algunos en fallo?
    delay(1000);
    //alarma!!
  }

  while (!hal.calibrate())
  {
    hal.debug("HAL CALIBRATION ERR!"); // No arrancamos si falla algun componente o podemos arrancar con algunos en fallo?
    delay(1000);
    //alarma!!
  }


  hal.debug("HAL READY!");

//  ventilation = new MechVentilation(hal, configuration, alarms);


  unsigned long now = millis();
  lastVentilatorUpdate = now;


  #ifdef INTFLOWSENSOR
    attachInterrupt(digitalPinToInterrupt(ENTRY_FLOW_PIN), flowIn, RISING);
    attachInterrupt(digitalPinToInterrupt(EXIT_FLOW_PIN), flowOut, RISING);
  #endif

  hal.debug("SETUP COMPLETED!: " +String(now)+" ms");

//TESTING!
//  hal.setConstantPressure(30.0);
}

uint16_t loops;
unsigned long lastLoopTimer = 0;

void loop()
{


  //Serial.println("loop!"+String(tim)); Serial.flush();delay(10);

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


  hal.update();

  unsigned long now = millis();

  if(now >= lastVentilatorUpdate + VENTILATOR_INTERVAL)
  {
    lastVentilatorUpdate = now;
//    ventilation->update();
//    alarms->check();
  }

}
