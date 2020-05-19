
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
mksBME280diff::mksBME280diff(uint8_t addr_,uint8_t addrOffset_, uint8_t i2cBus_) : addr(addr_), addrOffset(addrOffset_), i2cBus(i2cBus_)
{
}
    

mksBME280diff::~mksBME280diff()
{

}

void mksBME280diff::i2cBusSelect(uint8_t busNumber) {
  
  if (busNumber > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << busNumber);
  Wire.endTransmission();  
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
     Serial.println("Debug: i2cBus "+String(this->i2cBus));

     i2cBusSelect(this->i2cBus);

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
     i2cBusSelect(this->i2cBus);

     float presTubo = this->bme.readPressure();
     float  presAmbiente = this->bmeOffset.readPressure();

//     Serial.println(String(presAmbiente)+"\t"+String(presTubo));
//     Serial.flush();
     return  presTubo - presAmbiente ;
}
