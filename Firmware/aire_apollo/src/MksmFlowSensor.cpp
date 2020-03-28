#include "MksmFlowSensor.h"
#include <Arduino.h>

MksmFlowSensor::MksmFlowSensor(uint16_t pulses_per_liter,uint8_t sampling_ms) :
  _pulsesPerLiter(pulses_per_liter), _samplingMS(sampling_ms)
  {

  }

  MksmFlowSensor::~MksmFlowSensor()
  {}

  bool MksmFlowSensor::begin()
  {
    return true;
  }


  void MksmFlowSensor::pulse()
  {
    _pulsesSinceLastSample++;
    _pulseCounter++;
  }

  void MksmFlowSensor::update()
  {
    if( millis() % _samplingMS==0 )
    {
      _instantFlow = (_pulsesSinceLastSample / float(_pulsesPerLiter) ) * (60*(1000/float(_samplingMS))) * 1000.0 ;
      _pulsesSinceLastSample = 0;
    }
  }

  float MksmFlowSensor::getInstantFlow()
  {
    return _instantFlow;
  }

  float MksmFlowSensor::getFlow()
  {
    return _pulseCounter / float(_pulsesPerLiter) * 1000.0;
  }

  void MksmFlowSensor::resetFlow()
  {
    _pulseCounter = 0;
  }
