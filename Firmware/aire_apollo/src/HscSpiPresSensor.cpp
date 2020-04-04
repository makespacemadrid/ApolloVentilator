
#include "HscSpiPresSensor.h"
#include <Arduino.h>
#include<SPI.h>

/**
 * @brief Construct a new HSCMRRD001PDSA5 sensor object
 * 
 */
HscSpiPresSensor::HscSpiPresSensor(uint8_t sensorPin) :  _pin(sensorPin),  _sensor(sensorPin, _min_pressure, _max_pressure)
{    
}    

HscSpiPresSensor::~HscSpiPresSensor()
{
}


/**
 * @brief Check if the sensor is detected 
 * 
 * 
 * @return true if the sensor is detected
 * @return false if the sensor is not detected
 */
bool HscSpiPresSensor::begin()
{
    
    _sensor.begin();
    
    uint8_t status = _sensor.readSensor();

    
    if(status!=HscValidData)
        return false;

    return true;
}



/**
 * @brief read pressure from sensor
 * 
 * @return float the pressure measured in Pa
 */
float HscSpiPresSensor::readPascal()
{
    float val = 0;

    uint8_t status =_sensor.readSensor();

    if( status == HscValidData  || status == HscStaleData) 
        val = _sensor.pressure();
    else
        val = _min_pressure*10.00F;  ///< Return an out of range value to indicate and error condition
     
    return val; 
}

