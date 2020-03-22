/***************************************************************************
  This is a library for the BME280 humidity, temperature & pressure sensor

  Designed specifically to work with the Adafruit BME280 Breakout
  ----> http://www.adafruit.com/products/2650

  These sensors use I2C or SPI to communicate, 2 or 4 pins are required
  to interface. The device's I2C address is either 0x76 or 0x77.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit andopen-source hardware by purchasing products
  from Adafruit!

  Written by Limor Fried & Kevin Townsend for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  See the LICENSE file for details.
 ***************************************************************************/
#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define ENTRY_EV_PIN    10
#define EXIT_EV_PIN    11


#define BME280_ADDR         0x76

#define SEALEVELPRESSURE_HPA (1013.25)

# define INSPIRATION_TIME 1000
# define ESPIRATION_TIME 4000


Adafruit_BME280 bme; // I2C

unsigned long delayTime;

float volmax;
float presmax;
float o2insp;
float limitO2;
float co2esp;
float limitCO2;
float volumeentry;
float volumeexit;
int ppeak = 23;
int pplat = 18;
int peep = 5;

// Parameters
int weight = 70; // kg
int volc = 300; // weight * 6u8 (mL)
int rpm = 15; // breaths per minute
int voli = volc * rpm;
int timep; // good to have, pause
unsigned long inspiration = 0; // (s)
unsigned long espiration = 0; // (s)


uint8_t power;

// BEGIN Sensors and actuators

// Open Entry valve
void openEntryEV()
{
     digitalWrite(ENTRY_EV_PIN, 1);
}

// Close Entry valve
void closeEntryEV()
{
    digitalWrite(ENTRY_EV_PIN, 0);
}

// Open exit EV
void openExitEV()
{
    digitalWrite(EXIT_EV_PIN, 1);
}

// Close exit EV
void closeExitEV()
{
    digitalWrite(EXIT_EV_PIN, 0);
}


// Get metric from entry flow mass sensor
float getMetricVolumeEntry(){
/*
    L/m V
     0  1.00
    25  2.99
    50  3.82
    75  4.30
    100 4.58
    150 4.86
    200 5.00
*/
 
 // TODO Check real sensor output values, som parts of datasheets says it is linear
 // while a table of SLM/Volt shown the oposite
 float v=analogRead(ENTRY_EV_PIN)*0.0049F;

 return v;

}

// Get metric from exit flow mass sensor
float getMetricVolumeExit(){}

// Get metric from pressure sensor
float getMetricPressureEntry()
{    
    // Using bme sensor until we have the correct one
    return float(bme.readPressure()) / 100.0F;
}

// END sensors and actuator


int getMetricPpeak(){}
int getMetricPplat(){}

int calculateResistance (int ppeak, int pplat) {
	return ppeak - pplat;
}

int getMetricPeep(){}



void checkLeak(float volEntry, float volExit){}
float getMetricVolMax(){}
float getMetricPresMax(){}

int calculateCompliance (int pplat, int peep) {
	return pplat - peep;
}





void setup() {
    Serial.begin(115200);
    while(!Serial);    // time to get serial running
    

    pinMode(ENTRY_EV_PIN, OUTPUT);
    pinMode(EXIT_EV_PIN, OUTPUT);

    // BME280
    
    if (!bme.begin(BME280_ADDR)) {
        Serial.println(F("BME280 sensor not found!!!"));
        Serial.println("HALT!");
        while (1);
    }

    // set max sampling for pressure sensor
    bme.setSampling(Adafruit_BME280::MODE_NORMAL,
                   Adafruit_BME280::SAMPLING_X1,
                   Adafruit_BME280::SAMPLING_X16,
                   Adafruit_BME280::SAMPLING_X1,
                   Adafruit_BME280::FILTER_OFF,
                   Adafruit_BME280::STANDBY_MS_0_5);
    //

    delayTime = 0;
    Serial.println();
}


void loop() {
    
    openEntryEV();
	
    if(millis() >= inspiration + INSPIRATION_TIME){
		inspiration +=INSPIRATION_TIME;
		Serial.println("Insp");
	}	

	ppeak = getMetricPpeak();
	pplat = getMetricPplat();
	
    calculateResistance(ppeak, pplat);
	
    if (ppeak > 40){
		Serial.println("PRESSURE ALERT");
	}
	
    int peep = getMetricPeep();
	
    closeEntryEV();
	
    openExitEV();
	
    if(millis() >= inspiration + ESPIRATION_TIME){
		inspiration +=ESPIRATION_TIME;
		Serial.println("Esp");
	}
	
    float volExit = getMetricVolumeExit();
	checkLeak(volc, volExit);
	calculateCompliance(pplat, peep);
	
    closeExitEV();
    
}
