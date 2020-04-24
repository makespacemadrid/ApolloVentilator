#ifndef SERVOVALVE_H
#define SERVOVALVE_H

#include "ApolloValve.h"
#include <Servo.h>



class servoValve : public ApolloValve
{
public:
  servoValve(uint8_t pin,uint8_t minpos = 0,uint8_t maxpos = 180);
  virtual bool    begin();
  virtual void    open(double percent = 100,bool wait = false);
  virtual void    close(bool wait = false);
  virtual double  status();
  virtual double  target() {return _percent;}
protected:
  Servo   _servo;
  uint8_t _pin;
  double  _percent;
  uint8_t _minPos;
  uint8_t _maxPos;
  double  _lastPos;
  unsigned long _lastOrderTimestamp;
  unsigned long _estimatedFinishTime;
  const uint16_t _OpenTimeMS;
  bool           _blockInterrupt;
};


#endif
