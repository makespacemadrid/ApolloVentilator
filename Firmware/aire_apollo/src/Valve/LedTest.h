#ifndef LedTest_H
#define  LedTest_H

#include <Arduino.h>
#include "../ApolloValve.h"

class LedTest : public ApolloValve
{

public:

  LedTest(uint8_t pin_);
  bool    begin();
  void    open(double percent = 100);
  void    close();
  double  status() {return this->percent;}
  void    update(); //Required to move de steps


protected:

  double percent;
  uint8_t pin;

  uint8_t stepEnd;
  uint8_t pinFcIni = 0;
  uint8_t pinFcEnd = 0;

  uint8_t stepDestination = 0;
  uint8_t stepNow = 0;

  bool lastDir;
  bool lastStep;
  unsigned long lastTime;

};

#endif