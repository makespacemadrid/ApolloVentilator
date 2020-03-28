
#include <ApolloPresI2C.h>
#include <Arduino.h>


/**
 * @brief Construct a new ApolloPresI2C
 * 
 */
ApolloPresI2C::ApolloPresI2C(uint8_t addr)
{
    this->_addr=addr;
}

ApolloPresI2C::~ApolloPresI2C()
{
 
}


/**
 * @brief Check if the sensor is detected and configure sampling
 * 
 * @param addr sensor I2C address
 * @return true if the sensor is detected
 * @return false if the sensor is not detected
 */
bool ApolloPresI2C::begin()
{
    // Read once to check if the sensor is there

    //  Wire.begin(addr);
    //  Wire.requestFrom(addr,2);
    //  Wire.readBytes()
    
    return true;
}



/**
 * @brief read pressure from sensor
 * 
 * @return float the pressure measured in mBars
 */
float ApolloPresI2C::read()
{
     float val = 0;
    return val / 100.0F; 
}

