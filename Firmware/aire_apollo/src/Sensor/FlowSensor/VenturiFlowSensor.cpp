#include "VenturiFlowSensor.h"
#include <Arduino.h>
#include "Sensor/Pressure/ApolloPressureSensor.h"

VenturiFlowSensor::VenturiFlowSensor(ApolloPressureSensor *sensor, uint8_t sampling_ms = 10) : _sensor(sensor), _samplingMS(sampling_ms)
{

}

VenturiFlowSensor::~VenturiFlowSensor()
{}

bool VenturiFlowSensor::begin()
{
    return _sensor->begin();
}

/**
   * @brief Update count of ml insufflated to the patient in this period
   *
   */

  void VenturiFlowSensor::update()
  {

    unsigned long now = millis();

    if( now - _lastSampleTime > _samplingMS)
    {
//      Serial.println("now: "+String(now)+" _lastSampleTime: "+String(_lastSampleTime)+ " _lastInstantFlow: "+String(_lastInstantFlow));
      _volSinceReset += (now-_lastSampleTime)*(_lastInstantFlow/60000.0);
      _lastSampleTime = now;
//      Serial.println(_volSinceReset);Serial.flush();
    }
  }

  /**
   * @brief Return instal flow  in SLM
   *
   * @return float Return instal flow  in SLM or -1 if error
   */
  float VenturiFlowSensor::getInstantFlow()
  {     

    _lastInstantFlow= _sensor->readPascal();

    return _lastInstantFlow;

  }

  /**
   * @brief Return amount of air in ml given to the patient in this period
   *
   * @return float
   */
  float VenturiFlowSensor::getFlow()
  {
    return _volSinceReset;
  }

  /**
   * @brief Reset volume count
   *   Needed to know how many ml inspired
   *
   */
  void VenturiFlowSensor::resetFlow()
  {
    _volSinceReset = 0;
  }



