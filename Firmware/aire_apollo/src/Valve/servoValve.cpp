
#include "servoValve.h"

#define SERVO_FULL_OPEN_TIME 1200

servoValve::servoValve(uint8_t pin,uint8_t minpos,uint8_t maxpos) :
_pin(pin),_minPos(minpos),_maxPos(maxpos),_OpenTimeMS(SERVO_FULL_OPEN_TIME*(maxpos-minpos)/180.0)
{

}

bool servoValve::begin()
{
    _servo.attach(_pin);
    return true;
    _lastPos = 0;
    _percent = 0;
}


void servoValve::open(double percent)
{
  _blockInterrupt = true;
  if(percent > 100) percent = 100;
  if(percent < 0)   percent = 0;
  unsigned long now = millis();
  _lastPos = status();
  _percent = percent;
  int val = map(percent, 0, 100, _minPos, _maxPos);     // scale it to use it with the servo (value between 0 and 180)
  _servo.write(val);

  _lastOrderTimestamp = now;
  double move = _lastPos - _percent;
  uint16_t moveTime = _OpenTimeMS * abs(move)/100.0;
  _estimatedFinishTime = now + moveTime;
  //Serial.println("MOVE pos: "+String(_lastPos)+" target: "+String(_percent)+"now: "+String(now)+" estimated: "+String(_estimatedFinishTime));Serial.flush();
  _blockInterrupt = false;
}


void servoValve::close()
{
  open(0);
}

void servoValve::waitOpen(double percent)
{
  _blockInterrupt = true;
  if(percent > 100) percent = 100;
  if(percent < 0)   percent = 0;

  _percent    = percent;
  _lastPos    = percent;

  int val = map(percent, 0, 100, _minPos, _maxPos);     // scale it to use it with the servo (value between 0 and 180)
  _servo.write(val);
  delay(_OpenTimeMS);
  _blockInterrupt = false;
}

void servoValve::waitClose()
{
  waitOpen(0);
}

double servoValve::status()
{
  unsigned long now = millis();
  if(now >= _estimatedFinishTime)
      return _percent;
  else
      return map(now,_lastOrderTimestamp,_estimatedFinishTime,_lastPos,_percent);
}

void servoValve::update()
{
    if(_blockInterrupt) return;
}
