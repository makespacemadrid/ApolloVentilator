#ifndef PITOT_H
#define PITOT_H

#include "ApolloFlowSensor.h"
#include "../Pressure/ApolloPressureSensor.h"

class PitotFlowSensor : public ApolloFlowSensor
{

public:
  PitotFlowSensor(ApolloPressureSensor* pSensor, float flowArea =65)
  {
    _pSensor = pSensor;
  }

  virtual bool begin()
  {
    return _pSensor->begin();
  }

  virtual float getFlow()
  {
    return _cummulativeFlow;
  }

  virtual float getInstantFlow()
  {
    return _lastFlow;
  }

  virtual void resetFlow()
  {
    _cummulativeFlow = 0;
  }

  virtual void update()
  {
    float pdiff = _pSensor->readPascal();
    float rho = 1.204; // density of air
    float veloc = 0.0;
    if (adc_avg<512){
      veloc = -sqrt((-10000.0*((adc_avg/1023.0)-0.5))/rho);
    } else{
      veloc = sqrt((10000.0*((adc_avg/1023.0)-0.5))/rho);
  }


protected:
  ApolloPressureSensor* _pSensor;
  float _cummulativeFlow;
  float _lastFlow;

};
#endif
