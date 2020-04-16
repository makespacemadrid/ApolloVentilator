#include "Sfm3000FlowSensor.h"
#include <Arduino.h>
#include <Wire.h>

Sfm3000FlowSensor::Sfm3000FlowSensor(uint8_t sampling_ms, uint8_t addr) : _addr(addr), _samplingMS(sampling_ms)
  {
    _lastSampleTime = 0;
  }

  Sfm3000FlowSensor::~Sfm3000FlowSensor()
  {}

  bool Sfm3000FlowSensor::begin()
  {

    Wire.begin();
    delay(100);      // give the sensor time to start after power up
    softReset();
    readScale();  // Read scale facor and read offset from sensor
    readOffset();
    startFlowMeasurement();
    readBytes();    // first read is always invalid
    delay(1);

    return true;
  }


  /**
   * @brief Update count of ml insufflated to the patient in this period
   *
   */

  void Sfm3000FlowSensor::update()
  {

    unsigned long now = millis();

    if( now - _lastSampleTime > _samplingMS)
    {
      _volSinceReset += (now-_lastSampleTime)*(_lastInstantFlow/60.0/1000.0);
      _lastSampleTime = now;
    }
  }

  /**
   * @brief Return instal flow  in SLM
   *
   * @return float Return instal flow  in SLM or -1 if error
   */
  float Sfm3000FlowSensor::getInstantFlow()
  {

    if (readBytes() == -1)
      return -1;    // ERROR

    uint16_t newValue = (_data[0]<<8) | _data[1];

    _lastInstantFlow= ((float)newValue - _flowOffset) / _scaleFactor;

    return _lastInstantFlow;

  }

  /**
   * @brief Return amount of air in ml given to the patient in this period
   *
   * @return float
   */
  float Sfm3000FlowSensor::getFlow()
  {
    return _volSinceReset;
  }

  /**
   * @brief Reset volume count
   *   Needed to know how many ml inspired
   *
   */
  void Sfm3000FlowSensor::resetFlow()
  {
    _volSinceReset = 0;
    _lastSampleTime = millis();
  }


/**
 * @brief Read the flow offset needed to calculate l/m
 *
 * @return uint16_t the read offset
 */
  uint16_t  Sfm3000FlowSensor::readOffset()
  {
      sendCommand(SFM3000_READ_OFFSET);
      readBytes();
      _flowOffset = (_data[0]<<8) | _data[1];

      return _flowOffset;
  }

/**
 * @brief get the scale factor fro the sensor
 *
 * @return int
 */
  uint16_t Sfm3000FlowSensor::readScale()
  {
      sendCommand(SFM3000_READ_SCALE);
      readBytes();
      _scaleFactor = (_data[0]<<8) | _data[1];

     return _scaleFactor;
  }

/**
 * @brief Soft reset the sensor
 *
 */
void Sfm3000FlowSensor::softReset()
{
  sendCommand(SFM3000_RESET);
  delay(100);
}

/**
 * @brief Start continuous measurement until another command is sent
 *
 */
void Sfm3000FlowSensor::startFlowMeasurement()
{
  sendCommand(SFM3000_START_FLOW);
  delay(100);
}


/**
   * @brief Write 2 byte command to the sensor
   *
   * @param command 2 byte command
   * @return uint8_t 0 if success
   */
  uint8_t Sfm3000FlowSensor::sendCommand(uint16_t command)
  {
    uint8_t status = 0;

    Wire.beginTransmission(_addr);
    Wire.write(command >> 8);
    Wire.write(command  & 0xFF);
    status = Wire.endTransmission();


    return status;
  }

  /**
   * @brief Read sizeof(_data) bytes from sensor and store values in _data
   *
   * @return uint8_t number of bytes readed or -1 is error
   */
  int Sfm3000FlowSensor::readBytes()
  {
    int bytesReaded = 0;

    bytesReaded = Wire.requestFrom(_addr, sizeof(_data));

    if(bytesReaded != sizeof(_data))
      return -1;

    for(int i = 0; i<bytesReaded; i++)
      _data[i] = Wire.read();

    if (Wire.endTransmission() != 0)
      return -1;

    uint8_t crc = 0;
    crc = CRC(_data[0], crc);
    crc = CRC(_data[1], crc);

    if (crc != _data[2])
      return -1;             // TODO find the best value to incate error, flow may be negative

    return bytesReaded;
  }

/**
 * @brief CRC code from
 *  https://github.com/MyElectrons/sfm3300-arduino/blob/master/examples/SFM3300_Flow_Volume_selfT/SFM3300_Flow_Volume_selfT.ino
 *
 * @param x
 * @param crc
 * @return uint8_t
 */
uint8_t Sfm3000FlowSensor::CRC(uint8_t x, uint8_t crc) {
  crc ^= x;
  for (uint8_t bit = 8; bit > 0; --bit) {
    if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
    else crc = (crc << 1);
  }
  return crc;
}
