#ifndef APOLLO_BME_H
#define APOLLO_BME_H

#include "ApolloPressureSensor.h"
#include <Adafruit_BME280.h>

#define TCAADDR 0x70

/**
 * @brief BME280 I2C sensor
 *
 */
class mksBME280 : public ApolloPressureSensor
{
public:
    mksBME280(uint8_t addr = 0x76, uint8_t i2cBus = 0x00);
    ~mksBME280();

    bool begin();
    float readPascal();
    

private:
    uint8_t _addr;
    uint8_t _i2cBus;
    Adafruit_BME280 _bme; // I2C
    float zero;

    void i2cBusSelect(uint8_t busNumber);
};

#endif
