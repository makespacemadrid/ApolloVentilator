
#include "servoValve.h"

servoValve::servoValve(uint8_t pin,uint8_t minpos,uint8_t maxpos) :
_pin(pin),_minPos(minpos),_maxPos(maxpos)
{

}


bool servoValve::begin()
{
    _servo.attach(_pin);
    return true;
}

void servoValve::open(uint8_t percent)
{
  if(_percent > 100) percent = 100;
  _percent = percent;
  int val = map(percent, 0, 100, _minPos, _maxPos);     // scale it to use it with the servo (value between 0 and 180)
  _servo.write(val);
}

void servoValve::close()
{
  open(0);
}
