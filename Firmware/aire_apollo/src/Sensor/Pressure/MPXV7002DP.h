#ifndef MPX7002_H
#define MPX7002_H

#include "ApolloPressureSensor.h"

class MPXV7002DP : public ApolloPressureSensor
{
public:
  MPXV7002DP(int pin,uint16_t initial_offset = 0)
  {
    _pin    = pin;
    _offset = initial_offset;
  }
  virtual bool begin()
  {
    pinMode(_pin,INPUT);
    if(_offset ==0)
    {
      double val = 0;
      for(int i = 0 ; i < 100 ; i++)
      {
        val+=analogRead(_pin);
      }
      val /= 100.0;
      _offset = val - 2048;
    }
    return true;
  }

  virtual float readPascal()
  {
    double val = 0;
    for(int i = 0 ; i < 10 ; i++)
    {
      val+=analogRead(_pin);
    }
    val /= 10;
    Serial.println("v:"+String(val)+" o:"+String(_offset));
    val -=_offset;
    return map(val, 0, 4096, -2000, 2000);
  }

protected:
  int     _pin;
  float _offset;
};

#endif
