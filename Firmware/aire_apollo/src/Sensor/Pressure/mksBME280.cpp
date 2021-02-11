
#include "Sensor/Pressure/mksBME280.h"

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
/**
 * @brief Construct a new Apollo B M E:: Apollo B M E object
 *
 * @param addr sensor I2C address
 */

mksBME280::mksBME280(uint8_t addr, uint8_t i2cBus) : _addr(addr), _i2cBus(i2cBus)
{
     //    _bme = new Adafruit_BME280();
}

mksBME280::~mksBME280()
{
     //    if(_bme)
     //        delete _bme;
}

void mksBME280::i2cBusSelect(uint8_t busNumber) {
  
  if (busNumber > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << busNumber);
  Wire.endTransmission();  
}


/**
 * @brief Check if the sensor is detected and configure sampling
 *
 * @return true if the sensor is detected
 * @return false if the sensor is not detected
 */
bool mksBME280::begin()
{
     Serial.println("Debug: i2cBus "+String(this->_i2cBus));

     i2cBusSelect(this->_i2cBus);

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
     i2cBusSelect(this->_i2cBus);

     return _bme.readPressure() - this->zero;
}
