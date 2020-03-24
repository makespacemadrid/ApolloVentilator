
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

float ApolloBME::readPressure()
{
     float val = bme.readPressure();
    return val / 100.0F; // hpa
}