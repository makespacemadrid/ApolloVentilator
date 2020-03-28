#include "ApolloPressureSensor.h"
#include <Arduino.h>


/**
 * @brief read pressure from sensor
 *
 * @return float the pressure measured in hPa
 */
float ApolloPressureSensor::readMilibar()
{
   return readPascal() / 100.0F;
}

/**
 * @brief read pressure from sensor
 *
 * @return float the pressure measured in mmHg
 */

float ApolloPressureSensor::readMMHg()
{
  return readPascal() * 0.00750062F; //mmhg
}

float ApolloPressureSensor::readCMH2O()
{
  return readPascal() * 0.0101972F; //mmhg
}
