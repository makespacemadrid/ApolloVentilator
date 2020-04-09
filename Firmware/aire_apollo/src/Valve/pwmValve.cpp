#include "pwmValve.h"

pwmValve::pwmValve(uint8_t pin) : _pin(pin)
{

}

bool    pwmValve::begin()
{
  pinMode(_pin,OUTPUT);
  digitalWrite(_pin,0);
  return true;
}

void    pwmValve::open(double percent)
{
  if(percent > 100) percent = 100;
  if(percent < 0)   percent = 0;
    _percent = percent;
    uint8_t output = 255 * (percent/100.0);
    analogWrite(_pin,output);
}

void    pwmValve::close()
{
  digitalWrite(_pin,0);
}
