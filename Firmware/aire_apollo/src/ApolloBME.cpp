
#include <ApolloBME.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

ApolloBME::ApolloBME()
{
    bme=Adafruit_BME280();
}

//
// Check the sensor and configure sampling
//
bool ApolloBME::begin(uint8_t addr)
{
    
     if (!bme.begin(addr)) {
        return false;
    }

    // set max sampling for pressure sensor
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                   Adafruit_BME280::SAMPLING_X1,
                   Adafruit_BME280::SAMPLING_X16,
                   Adafruit_BME280::SAMPLING_X1,
                   Adafruit_BME280::FILTER_OFF,
                   Adafruit_BME280::STANDBY_MS_0_5);
    return true;
}

float ApolloBME::readPressure()
{
     float val = bme.readPressure();
    return val / 100.0F; // hpa
}