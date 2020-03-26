#include "ApolloPressureSensor.h"
#include <Arduino.h>


/**
 * @brief read pressure from sensor
 * 
 * @return float the pressure measured in hPa
 */
float ApolloPressureSensor::readHpa()
{

   return read() / 100.0;
}

/**
 * @brief read pressure from sensor
 * 
 * @return float the pressure measured in mmHg
 */
float ApolloPressureSensor::readMMHg()
{
return read() / 0.00750062; //mmhg
}