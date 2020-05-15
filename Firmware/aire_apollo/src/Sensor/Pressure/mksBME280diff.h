#ifndef APOLLO_BME_DIFF_H
#define APOLLO_BME_DIFF_H

#include "ApolloPressureSensor.h"
#include <Adafruit_BME280.h>

#define TCAADDR 0x70

/**
 * @brief BME280 differential I2C sensor
 *
 */
class mksBME280diff : public ApolloPressureSensor
{
public:
    mksBME280diff(uint8_t addr = 0x76,uint8_t addrOffset_ = 0x77, uint8_t i2cBus = 0);
    ~mksBME280diff();

    bool begin();
    float readPascal();

private:
    void i2cBusSelect(uint8_t busNumber);
    uint8_t addr;
    uint8_t addrOffset;    
    Adafruit_BME280 bme; // I2C
    Adafruit_BME280 bmeOffset; // Ambient sensor
    float zero;
    uint8_t i2cBus;
};

#endif
