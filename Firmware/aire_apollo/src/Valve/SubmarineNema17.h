#ifndef SubmarineNema17_H
#define  SubmarineNema17_H

#include <Arduino.h>
#include "../ApolloValve.h"
// this pin should connect to Ground when want to stop the motor
#define STOPPER_PIN 4

// Motor steps per revolution. Most steppers are 200 steps or 1.8 degrees/step
#define MOTOR_STEPS 200
#define RPM 120
// Microstepping mode. If you hardwired it to save pins, set to the same value here.
#define MICROSTEPS 16
#define DIR 22
#define STEP 23

//#include "A4988.h"
#include "BasicStepperDriver.h" // generic
// BasicStepperDriver stepper(DIR, STEP);

class SubmarineNema17 : public ApolloValve
{

public:

  SubmarineNema17(uint8_t pinEna_,uint8_t pinDir_,uint8_t pinPul_,uint8_t pinFcIni_ = 0,uint8_t pinFcEnd_ = 0);
  bool    begin();
  void    open(uint8_t percent = 100);
  void    close();
  double  status() {return _percent > 0;}
  void    update(); //Required to move de steps
  BasicStepperDriver stepper;


protected:

  uint8_t _percent;
  uint8_t pinEna;
  uint8_t pinDir;
  uint8_t pinPul;
  uint8_t stepEnd;
  uint8_t pinFcIni = 0;
  uint8_t pinFcEnd = 0;

  uint8_t stepDestination = 0;
  uint8_t stepNow = 0;

  bool lastDir;
  int lastStep;
  unsigned long lastTime;

};

#endif