#ifndef StepperNema_H
#define  StepperNema_H

#include <Arduino.h>
#include "Valve/ApolloValve.h"
#include "DRV8825.h"

//Valores por defecto, se reconfiguran al instanciar el objeto!
// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM         200
// Acceleration and deceleration values are always in FULL steps / s^2
#define MOTOR_ACCEL 12000
#define MOTOR_DECEL 8000
// Microstepping mode. If you hardwired it to save pins, set to the same value here.
#define MICROSTEPS 16  // Ojo funciona hasta a 16 (TODO:Verificar)

#include "DRV8825.h"
//#include "DRV8825.h"
//#include "BasicStepperDriver.h" // generic


//TODO:
// Comprobar los finales de carrera durante los movimientos (ahora esta desactivado en el update, hay que ponerlo en otro sitio idealmente)
// Pensar en un mecanismo para que el stepper vuelva a hacer home de vez en cuando para asegurar que no se pierden pasos.
//

class StepperNema : public ApolloValve
{

public:

  StepperNema(uint8_t pin_Enable,uint8_t pinDir_,uint8_t pinStep_,uint8_t pinMinEndStop_ = 0,uint8_t pinMaxEndStop_ = 0, int closePos_=0, int startPos_=0,uint16_t stepsPerRevolution_ = 200, uint16_t maxRpm_ = RPM, uint8_t microsteps_ = MICROSTEPS);
  bool    begin();
  void    open(double percent = 100.0,bool wait = false);
  void    close(bool wait = false);
  double  status();
  double  target();
  void    update();
  void    highFreqUpdate(); //Required to move de steps

  bool    calibrate();
  bool    test();

  void    setMinEndStopPressedState(bool state_){minEndStopPressedState = state_;}
  void    setMaxEndStopPressedState(bool state_){maxEndStopPressedState = state_;}
  void    enableMinEndstopPullup()              {minEndstopPinMode = INPUT_PULLUP;}
  void    enableMaxEndstopPullup()              {maxEndstopPinMode = INPUT_PULLUP;}
protected:

  bool      moveAwayMinEndStop();
  bool      moveTowardsMinEndStop();
  bool      home(); //Reemplaza calibrate?

  uint16_t  countStepsToHome();
  bool      isMinEndStopPressed(){return digitalRead(pinMinEndstop) == minEndStopPressedState;}
  bool      isMaxEndStopPressed(){return digitalRead(pinMaxEndstop) == maxEndStopPressedState;}
  bool      isGoingForward()     {return lastDir == true;}



const   uint8_t pinMinEndstop = 0;
const   uint8_t pinMaxEndstop = 0;
        double  percent = 0.0;

        bool    blockUpdate = false;
        bool    lastDir     = 0;
        int32_t lastPos     = 0;
//  uint32_t nextActionTime;

        int32_t  stepDestination = 0;
const   uint32_t openPos  = 0; // Offset steps
const   uint32_t closePos  = 0;
        uint32_t stepsMax  = MOTOR_STEPS*MICROSTEPS;
const   uint16_t maxRPM;
const   uint8_t  microSteps;

  uint8_t  minEndstopPinMode  = INPUT;
  uint8_t  maxEndstopPinMode  = INPUT;
  bool minEndStopPressedState = LOW;
  bool maxEndStopPressedState = LOW;

  DRV8825 stepMotor;
};

#endif
