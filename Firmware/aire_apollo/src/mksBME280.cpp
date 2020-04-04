
#include <mksBME280.h>

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
/**
 * @brief Construct a new Apollo B M E:: Apollo B M E object
 *
 */
mksBME280::mksBME280(uint8_t addr) : _addr(addr)
{
     //    _bme = new Adafruit_BME280();
}

mksBME280::~mksBME280()
{
     //    if(_bme)
     //        delete _bme;
}

/**
 * @brief Check if the sensor is detected and configure sampling
 *
 * @param addr sensor I2C address
 * @return true if the sensor is detected
 * @return false if the sensor is not detected
 */
bool mksBME280::begin()
{

     if (!_bme.begin(_addr))
     {
          return false;
     }

     // set max sampling for pressure sensor
     _bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                      Adafruit_BME280::SAMPLING_X1,
                      Adafruit_BME280::SAMPLING_X16,
                      Adafruit_BME280::SAMPLING_X1,
                      Adafruit_BME280::FILTER_OFF,
                      Adafruit_BME280::STANDBY_MS_0_5);

     //Search Zero bassed in actual pression
     unsigned long init = millis();
     unsigned long now = 0;
     int reads = 0;
     float total = 0;
     while (now < (init + 3000))
     {
          now = millis();
          reads++;
          total += _bme.readPressure();
     }
     this->zero = total / reads;
     return true;
}

/**
 * @brief read pressure from sensor
 *
 * @return float the pressure measured in mBars
 */
float mksBME280::readPascal()
{
     return _bme.readPressure() - this->zero;
}
