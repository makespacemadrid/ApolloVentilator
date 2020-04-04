#ifndef PWMVALVE_H
#define  PWMVALVE_H

#include<Arduino.h>
#include "ApolloValve.h"

class pwmValve : public ApolloValve
{

public:

  pwmValve(uint8_t pin);
  virtual bool    begin();
  virtual void    open(uint8_t percent = 100);
  virtual void    close();
  virtual uint8_t status() {return _percent > 0;}

protected:

  uint8_t _percent;
  uint8_t _pin;

};








#endif
