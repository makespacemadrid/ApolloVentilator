
#include "mksBME280diff.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
/**
 * @brief Construct a new Apollo BME diffrential object
 *
 */
mksBME280diff::mksBME280diff(uint8_t addr_,uint8_t addrOffset_)
{
     this->addr = addr_;
     this->addrOffset = addrOffset_;
}
    

mksBME280diff::~mksBME280diff()
{

}

/**
 * @brief Check if the sensor is detected and configure sampling
 *
 * @param addr sensor I2C address
 * @return true if the sensor is detected
 * @return false if the sensor is not detected
 */
bool mksBME280diff::begin()
{

     if (!this->bme.begin(this->addr))
     {
          return false;
     }

     if (!this->bmeOffset.begin(this->addrOffset))
     {
          return false;
     }


     // set max sampling for pressure sensor
     this->bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                      Adafruit_BME280::SAMPLING_X1,
                      Adafruit_BME280::SAMPLING_X16,
                      Adafruit_BME280::SAMPLING_X1,
                      Adafruit_BME280::FILTER_OFF,
                      Adafruit_BME280::STANDBY_MS_0_5);

     // set max sampling for pressure sensor
     this->bmeOffset.setSampling(Adafruit_BME280::MODE_NORMAL,
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
float mksBME280diff::readPascal()
{
     return this->bme.readPressure() - this->bmeOffset.readPressure();
}
