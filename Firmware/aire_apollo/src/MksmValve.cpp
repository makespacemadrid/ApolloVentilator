#include "MksmValve.h"
#include <Arduino.h>
#include "trace.h"

MksmValve::MksmValve(uint8_t pin, uint16_t openDelay, uint16_t closeDelay, bool invertedLogic) :
_pin(pin),_trueState(!invertedLogic),
_openDelayMS(openDelay),_closeDelayMS(closeDelay) {;}

void MksmValve::setup()
{
  pinMode(_pin, OUTPUT);
  close();
}

void MksmValve::open(uint8_t percent)
{
  if(percent > 100) percent = 100;
  if(_openPercent == 0)//Si estabamos cerrados primero cargamos la bobina de la valvula
  {
      magnetize();
      delay(_openDelayMS);
  }
  if(percent == 0)  close();
  _openPercent = percent;
  _deMagnetizedTimeMS  =  (100-percent);
  _magnetizedTimeMS = _deMagnetizedTimeMS * (_openDelayMS/float(_closeDelayMS));
  if(_openPercent == 100) _magnetizedTimeMS = 100;
  _cycleTimeMS = _magnetizedTimeMS + _deMagnetizedTimeMS;
  TRACE("mTime:" + String(_magnetizedTimeMS) + "dTime:" + String(_deMagnetizedTimeMS) + "cyclems:" + String(_cycleTimeMS) );
}

void MksmValve::close()
{
  _openPercent = 0;
  demagnetize();
  delay(_closeDelayMS);
}

void MksmValve::update()
{
  if(!isOpen()) return;

  uint16_t timeReference = millis()%_cycleTimeMS; //Hallamos en que momento del ciclo de la valvula estamos

  if( isMagnetized() && ( timeReference >= _magnetizedTimeMS ) ) //Si la valvula esta abierta y ya ha pasado el tiempo de estar abierta la cerramos
      demagnetize();
  else if(!isMagnetized() && (timeReference <= _magnetizedTimeMS) ) // Si no está abierta y estamos dentro del tiempo de estar abierta la abrimos
      magnetize();
}
