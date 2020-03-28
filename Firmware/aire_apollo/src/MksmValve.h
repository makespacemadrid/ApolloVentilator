#ifndef MKSM_VALVE_H
#define MKSM_VALVE_H

#include<Arduino.h>
#include "ApolloValve.h"

class MksmValve : public ApolloValve
{
public:
  MksmValve(uint8_t pin, uint16_t openDelay = 30 , uint16_t closeDelay = 10 , bool invertedLogic = false);

  bool begin();
  void open(uint8_t percent = 100);
  void close();

  uint8_t openPercent() {return _openPercent;}
  bool    isOpen()      {return _openPercent > 0;}
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
};

#endif
