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

#define ENTRY_EV_PIN    10   //ElectroValvula - Entrada
#define EXIT_EV_PIN     9   //ElectroValvula - Salida

#define ENTRY_FLOW_PIN  4    //Sensor de Flujo - Entrada
#define EXIT_FLOW_PIN   5    //Sendor de Flujo - Salida

#define LOG_INTERVAL    10    //milliseconds

//#define PRESSURE_SENSOR_PIN      ??
#define BME280_ADDR                0x76

#define SEALEVELPRESSURE_HPA (1013.25)

#define INSPIRATION_TIME 1000
#define ESPIRATION_TIME 4000
#define INSPIRATION_THRESHOLD 10  //Descenso en la presion que marca el inicio de la respiracion

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
int volc = weight * 6; // weight * 6u8 (mL) Volumen tidal
int bpm = 10; // breaths per minute
int voli = volc * bpm;
//int timep; // good to have, pause
unsigned long lastInspirationStart = 0; // (s)
unsigned long lastEspirationStart = 0; // (s)
uint16_t inspirationTimeout = 0;
uint16_t inspTime = 0;
uint16_t espTime = 0;

//OJO a los contadores que se desbordan en algun momento!!!!
// Gestion rtc? / deteccion de desbordamiento?

//uint8_t power;
char          logBuffer[50];
unsigned long lastLogTime=0;

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

 // TODO Check real sensor output values, some parts of datasheets says it is linear
 // while a table of SLM/Volt shows the oposite
 float v=analogRead(ENTRY_FLOW_PIN)*0.0049F;

 return v;

}

// Get metric from exit flow mass sensor
float getMetricVolumeExit(){
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

 // TODO Check real sensor output values, some parts of datasheets says it is linear
 // while a table of SLM/Volt shows the oposite
 float v=analogRead(EXIT_FLOW_PIN)*0.0049F;

 return v;

}

// Get metric from pressure sensor in mBar
float getMetricPressureEntry()
{
    float val = bme.readPressure();
    return val / 100.0F; // hpa
}

// END sensors and actuator


int getMetricPpeak(){return 22;}
int getMetricPplat(){return 22;}

int calculateResistance (int ppeak, int pplat) {
	return ppeak - pplat;
}

int getMetricPeep(){return 22;}



void checkLeak(float volEntry, float volExit){}
float getMetricVolMax(){return 22;}
float getMetricPresMax(){return 22;}

int calculateCompliance (int pplat, int peep) {
	return pplat - peep;
}


void logData()
{
    unsigned long now = millis();
    if( (now - lastLogTime) >= LOG_INTERVAL)
    {
        //TODO use real EV states based on pin status
        //sprintf(logBuffer, "%d,%d,%f,%f,%f", 1, 1, getMetricPressureEntry(), getMetricVolumeEntry(), getMetricVolumeExit() );
        //Serial.println(logBuffer);
        String result = String(getMetricPressureEntry()) + "," + String(getMetricVolumeEntry()) + "," + String(getMetricVolumeExit());
        Serial.println(result);
        lastLogTime = now;
    }
}

void setBPM(uint8_t CiclesPerMinute)
{
  inspTime            = 60000.0/float(bpm)*0.25;
  espTime             = 60000.0/float(bpm)*0.60;
  inspirationTimeout  = 60000.0/float(bpm)*0.15;
  Serial.println("BPM set: iTime:"+String(inspTime)+", eTime:"+String(espTime)+"iTimeout:"+String(inspirationTimeout));
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
    setBPM(8);
    Serial.println();
}


enum respiratorStatus
{
  RESPIRATOR_PAUSED,
  WAIT_FOR_INSPIRATION,
  INSPIRATION_CICLE,
  ESPIRATION_CICLE,
};

respiratorStatus status = WAIT_FOR_INSPIRATION;

void beginInspiration()
{
  openEntryEV();
  closeExitEV();
  lastInspirationStart = millis();
  status = INSPIRATION_CICLE;
}

void beginEspiration()
{
  closeEntryEV();
  openExitEV(); // hack para pruebas!!!
  lastEspirationStart = millis();
  status = ESPIRATION_CICLE;
}

bool checkForPatientInspiration()
{
  float pressureReference = 950;
  if ( (pressureReference - getMetricPressureEntry()) > INSPIRATION_THRESHOLD )
    return true;
  else
    return false;
}

void loop() {
// Control del ciclo de respiracion
  if(status == RESPIRATOR_PAUSED)
  {
    Serial.println("PAUSED...");
  }
  else if(status == WAIT_FOR_INSPIRATION)
  {
    if(checkForPatientInspiration())
    {
      Serial.println("Insp DETECTED!");
      beginInspiration();
    }
    else if(millis() - (lastInspirationStart + inspTime + espTime) >= inspirationTimeout)
    {
      Serial.println("FORCING Insp");
      beginInspiration();
    }
  }
  else if(status == INSPIRATION_CICLE)
  {
    if(millis() - lastInspirationStart >= inspTime)
    {
      Serial.println("BeginEspiration");
      beginEspiration();
    }
  }
  else if(status == ESPIRATION_CICLE)
  {
    if(millis() - lastEspirationStart >= espTime)
    {
      status = WAIT_FOR_INSPIRATION;
      Serial.println("Cicle Done! :), Wait for inspiration");
    }
  }

//Comprobacion de alarmas

	ppeak = getMetricPpeak();
	pplat = getMetricPplat();

  calculateResistance(ppeak, pplat);

  if (ppeak > 40)
  {
    Serial.println("PRESSURE ALERT");
	}

  int peep = getMetricPeep();
  float volExit = getMetricVolumeExit();
	checkLeak(volc, volExit);
	calculateCompliance(pplat, peep);


// envio de datos
  logData();
}
