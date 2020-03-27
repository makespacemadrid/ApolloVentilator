#ifndef APOLLO_PRES_I2C_H
#define APOLLO_PRES_I2C_H

#include "ApolloPressureSensor.h"
#include <Wire.h>

/**
 * @brief Generic I2C pressure sensor
 * 
 */
class ApolloPresI2C : public ApolloPressureSensor
{
    public:
        ApolloPresI2C(uint8_t addr = 0x28);
        ~ApolloPresI2C();

        float read();
        bool begin();  
       
          
    private:
        uint8_t     addr;
        
};


#endif