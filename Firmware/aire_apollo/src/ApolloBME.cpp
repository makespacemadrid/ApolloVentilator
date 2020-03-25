
#include <ApolloBME.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

/**
 * @brief Construct a new Apollo B M E:: Apollo B M E object
 * 
 */
ApolloBME::ApolloBME(uint8_t addr)
{
    this->addr=addr;
    bme=new Adafruit_BME280();
}

ApolloBME::~ApolloBME()
{
    if(bme)
        delete bme;
}


/**
 * @brief Check if the sensor is detected and configure sampling
 * 
 * @param addr sensor I2C address
 * @return true if the sensor is detected
 * @return false if the sensor is not detected
 */
bool ApolloBME::begin()
{
    
     if (!bme->begin(addr)) {
        return false;
    }

    // set max sampling for pressure sensor
    bme->setSampling(Adafruit_BME280::MODE_NORMAL,
                   Adafruit_BME280::SAMPLING_X1,
                   Adafruit_BME280::SAMPLING_X16,
                   Adafruit_BME280::SAMPLING_X1,
                   Adafruit_BME280::FILTER_OFF,
                   Adafruit_BME280::STANDBY_MS_0_5);
    return true;
}



/**
 * @brief read pressure from sensor
 * 
 * @return float the pressure measured in mBars
 */
float ApolloBME::read()
{
     float val = bme->readPressure();
    return val / 100.0F; 
}

