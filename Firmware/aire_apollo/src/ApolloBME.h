#ifndef APOLLO_BME_H
#define APOLLO_BME_H

#include "ApolloFlowSensor.h"
#include <Adafruit_BME280.h>

class ApolloBME : public ApolloFlowSensor
{
    public:
        ApolloBME();
        ~ApolloBME();

        float readPressure();
        bool begin(uint8_t addr);    
    private:
        Adafruit_BME280 bme; // I2C
        
};


#endif