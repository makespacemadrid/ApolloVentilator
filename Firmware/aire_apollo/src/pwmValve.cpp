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

void    pwmValve::open(uint8_t percent)
{
    if(percent > 100) percent = 100;
    _percent = percent;
    uint8_t output = 255 * (percent/100.0);
    analogWrite(_pin,output);
}

void    pwmValve::close()
{
  digitalWrite(_pin,0);
}
