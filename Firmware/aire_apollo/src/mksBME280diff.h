#ifndef APOLLO_BME_DIFF_H
#define APOLLO_BME_DIFF_H

#include "ApolloPressureSensor.h"
#include <Adafruit_BME280.h>

/**
 * @brief BME280 differential I2C sensor
 *
 */
class mksBME280diff : public ApolloPressureSensor
{
public:
    mksBME280diff(uint8_t addr = 0x76,uint8_t addr_offset = 0x77);
    ~mksBME280diff();

    bool begin();
    float readPascal();

private:
    uint8_t _addr;
    uint8_t _addrOffset;    
    Adafruit_BME280 _bme; // I2C
    Adafruit_BME280 _bmeOffset; // Ambient sensor
    float zero;
};

#endif
