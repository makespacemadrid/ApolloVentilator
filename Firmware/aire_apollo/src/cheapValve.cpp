#include "cheapValve.h"
#include <Arduino.h>
#include "trace.h"

cheapValve::cheapValve(uint8_t pin, uint16_t openDelay, uint16_t closeDelay, bool invertedLogic) :
_pin(pin),_trueState(!invertedLogic),
_openDelayMS(openDelay),_closeDelayMS(closeDelay) {;}

bool cheapValve::begin()
{
  pinMode(_pin, OUTPUT);
  return true;
}

void cheapValve::open(double percent)
{
  if(percent > 100) percent = 100;
  if(percent < 0)   percent = 0;

  if(_openPercent == 0)//Si estabamos cerrados primero cargamos la bobina de la valvula
  {
      magnetize();
      delay(_openDelayMS);
  }

  _openPercent = percent;
  float factor = 100.0/float(101-percent);
  _deMagnetizedTimeMS   = _closeDelayMS       * factor;
  _magnetizedTimeMS     = _deMagnetizedTimeMS * (_openDelayMS/float(_closeDelayMS));
  _waitTimeMS           = (_closeDelayMS + _openDelayMS) / (101.0-percent/100.0);// / (factor/4.0);

  if(percent == 100)
  {
    _deMagnetizedTimeMS =   0;
    _magnetizedTimeMS   = 20;
  }
  if(percent == 0)
  {
    _deMagnetizedTimeMS = 20;
    _magnetizedTimeMS   =   0;
  }

  _cycleTimeMS = _magnetizedTimeMS + _deMagnetizedTimeMS + _waitTimeMS;
  //Serial.println("factor: " + String(factor) + " oTime: " + String(_magnetizedTimeMS) + " cTime:" + String(_deMagnetizedTimeMS) + " wTime: " + " cyclems:" + String(_cycleTimeMS) );
}

void cheapValve::close()
{
  _openPercent = 0;
  demagnetize();
  delay(_closeDelayMS);
}

void cheapValve::update()
{
  if(_openPercent == 0) return;

  uint16_t timeReference = millis()%_cycleTimeMS; //Hallamos en que momento del ciclo de la valvula estamos

  if      (isMagnetized() && ( timeReference < _deMagnetizedTimeMS ) ) //Si la valvula esta abierta y ya ha pasado el tiempo de estar abierta la cerramos
          demagnetize();
  else if (!isMagnetized() && (timeReference > _deMagnetizedTimeMS) && (timeReference < (_deMagnetizedTimeMS+_magnetizedTimeMS)) ) // Si no está abierta y estamos dentro del tiempo de estar abierta la abrimos
          magnetize();
}
