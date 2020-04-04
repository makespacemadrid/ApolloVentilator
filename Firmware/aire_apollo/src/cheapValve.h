#ifndef MKSM_VALVE_H
#define MKSM_VALVE_H

#include<Arduino.h>
#include "ApolloValve.h"

class cheapValve : public ApolloValve
{
public:
  cheapValve(uint8_t pin, uint16_t openDelay = 15 , uint16_t closeDelay = 10 , bool invertedLogic = false);

  bool begin();
  void open(uint8_t percent = 100);
  void close();

  uint8_t status()      {return _openPercent;}
  void update();


protected:

  void magnetize()    {digitalWrite(_pin,_trueState);}
  void demagnetize()  {digitalWrite(_pin,!_trueState);}
  bool isMagnetized() {return(digitalRead(_pin)==_trueState);}


  const uint8_t   _pin;
  const bool      _trueState;
  uint8_t         _openPercent;
  const uint16_t  _openDelayMS;
  const uint16_t  _closeDelayMS;
  uint16_t        _cycleTimeMS;
  uint16_t        _magnetizedTimeMS;
  uint16_t        _deMagnetizedTimeMS;
  uint16_t        _waitTimeMS;
};

#endif
