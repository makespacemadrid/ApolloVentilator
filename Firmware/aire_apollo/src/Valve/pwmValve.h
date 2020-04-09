#ifndef PWMVALVE_H
#define  PWMVALVE_H

#include<Arduino.h>
#include "ApolloValve.h"

class pwmValve : public ApolloValve
{

public:

  pwmValve(uint8_t pin);
  virtual bool    begin();
  virtual void    open(double percent = 100);
  virtual void    close();
  virtual double status() {return _percent > 0;}

protected:

  double _percent;
  uint8_t _pin;

};


#endif
