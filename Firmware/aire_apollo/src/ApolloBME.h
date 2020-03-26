#ifndef APOLLO_BME_H
#define APOLLO_BME_H

#include "ApolloPressureSensor.h"
#include <Adafruit_BME280.h>

/**
 * @brief BME280 I2C sensor
 * 
 */
class ApolloBME : public ApolloPressureSensor
{
    public:
        ApolloBME(uint8_t addr = 0x77);
        ~ApolloBME();

        float read();
        bool begin();  
       
          
    private:
        uint8_t     addr;
        Adafruit_BME280 *bme = NULL; // I2C        
};


#endif