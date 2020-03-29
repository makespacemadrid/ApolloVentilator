#include "MksmFlowSensor.h"
#include <Arduino.h>

MksmFlowSensor::MksmFlowSensor(uint16_t pulses_per_liter,uint8_t sampling_ms) :
  _pulsesPerLiter(pulses_per_liter), _samplingMS(sampling_ms)
  {
    _lastSampleTime = 0;
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
    unsigned long now = millis();
    if( now - _lastSampleTime > _samplingMS)
    {
      _instantFlow = (float(_pulsesSinceLastSample) / float(_pulsesPerLiter) ) * (60.0*(1000.0/float(_samplingMS))) ;
      _pulsesSinceLastSample = 0;
      _lastSampleTime = now;
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
