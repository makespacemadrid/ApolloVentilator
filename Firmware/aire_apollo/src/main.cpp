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


 #define DEBUG
 #define INTFLOWSENSOR  //Activar para usar los sensores de flujo por interrupcion.
 #define LOCALCONTROLS  // Display y encoders presentes.

 #define ENTRY_EV_PIN 10 //ElectroValvula - Entrada
 #define EXIT_EV_PIN 8   //ElectroValvula - Salida

 #define ENTRY_FLOW_PIN 3 //Sensor de Flujo - Entrada
 #define EXIT_FLOW_PIN  4 //Sendor de Flujo - Salida

 //#define PRESSURE_SENSOR_PIN      ??
 #define BME280_ADDR 0x76

 #define INSPIRATION_TIME 1000
 #define ESPIRATION_TIME 4000
 #define INSPIRATION_THRESHOLD 10 //Descenso en la presion que marca el inicio de la respiracion



#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <MsTimer2.h>



#include "trace.h"
#include "ApolloHal.h"
#include "mksBME280.h"
#include "MksmValve.h"
#include "MksmFlowSensor.h"
#include "Comunications.h"
#include "MechVentilation.h"
#include "ApolloEncoder.h"
#include "Display.h"


ApolloHal *hal;

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
int weight = 70;       // kg
int volc = weight * 6; // weight * 6u8 (mL) Volumen tidal
int bpm = 10;          // breaths per minute
int voli = volc * bpm;
//int timep; // good to have, pause
unsigned long lastInspirationStart = 0; // (s)
unsigned long lastEspirationStart = 0;  // (s)
uint16_t inspirationTimeout = 0;
uint16_t inspTime = 0;
uint16_t espTime = 0;

//OJO a los contadores que se desbordan en algun momento!!!!
// Gestion rtc? / deteccion de desbordamiento?


Comunications com = Comunications();

/*

int calculateResistance(int ppeak, int pplat)
{
  return ppeak - pplat;
}

int getMetricPeep() { return 22; }

void checkLeak(float volEntry, float volExit) {}

int calculateCompliance(int pplat, int peep)
{
  return pplat - peep;
}
*/

void logData()
{
  String pressure(hal->pressuresSensor()->readMMHg());
  String intakeFlow(hal->intakeFlowSensor()->getInstantFlow());
  String exitFlow(hal->exitFlowSensor()->getInstantFlow());
  String data[] = {pressure,intakeFlow,exitFlow};
  com.data(data,3);
  delay(1); //Para evitar que sea llamado varias veces en el mismo ms.
}


MechVentilation *ventilation;

#ifdef LOCALCONTROLS
  ApolloEncoder encoderRPM(12, 13, 0);
  ApolloEncoder encoderTidal(10, 11, 0);
  ApolloEncoder encoderPorcInspira(8, 9, 0);
  Display display = Display();
#endif


int porcentajeInspiratorio = DEFAULT_POR_INSPIRATORIO;
int rpm = DEFAULT_RPM;
int vTidal = 0;


void ISR1ms() //Esta funcion se ejecuta cada 1ms para gestionar sensores/actuadores!
{ // OJO!!! no bloquear ni hacer nada muy costoso en tiempo!!!!!!
    hal->ISR1ms();
}

#ifdef INTFLOWSENSOR
  void flowIn()
  {
    hal->intakeFlowSensor()->pulse();
  }

  void flowOut()
  {
    hal->exitFlowSensor()->pulse();
  }
#endif


void setup()
{
  Serial.begin(115200);
  delay(100);
  Serial.println("INIT");

  // Create hal layer with
  MksmFlowSensor*       fInSensor   = new MksmFlowSensor();
  ApolloFlowSensor*     fOutSensor  = new MksmFlowSensor();
  ApolloPressureSensor* pSensor     = new mksBME280(BME280_ADDR);
  ApolloValve*          inValve     = new MksmValve(ENTRY_EV_PIN);
  ApolloValve*          outValve    = new MksmValve(EXIT_EV_PIN);

  Serial.println("INIT HAL!");

  hal = new ApolloHal(pSensor,fInSensor,fOutSensor,inValve,outValve);

  Serial.println("begin HAL!");


  while(!hal->begin())
  {
    //TRACE("ERROR intializing HAL!!");
    Serial.println("PROBLEMA HALLLLLL!!!");
    Serial.flush();
  }

  Serial.println("HAL LISTO!");
  Serial.flush();
  ventilation = new MechVentilation(hal, vTidal, rpm, porcentajeInspiratorio);

  //ISRs
      MsTimer2::set(1, ISR1ms); // Interrupcion de 1ms para el manejo de sensores/actuadores.
      MsTimer2::start();

#ifdef INTFLOWSENSOR
      attachInterrupt(digitalPinToInterrupt(ENTRY_FLOW_PIN), flowIn , RISING);
      attachInterrupt(digitalPinToInterrupt(EXIT_FLOW_PIN) , flowOut, RISING);
#endif

#ifdef LOCALCONTROLS
  display.clear();
  display.writeLine(0, "RPM: " + String(ventilation->getRpm()));
  display.writeLine(1, "Vol Tidal: " + String(ventilation->getTidalVolume()));
  display.writeLine(2, "Press PEEP: " + String(ventilation->getPressionPeep()));
#endif

  Serial.println("Setup completado!");
  Serial.flush();
}


void loop()
{
//Comprobacion de alarmas

//  ppeak = getMetricPpeak();
//  pplat = getMetricPplat();

//  calculateResistance(ppeak, pplat);

//  if (ppeak > 40)
//  {
//    com.alert("PRESSURE ALERT");
//  }

//  int peep = getMetricPeep();
// ¿se debe meter la detección de perdidas en el hal?
//  float volExit = hal->getMetricVolumeExit();
//  checkLeak(volc, volExit);
//  calculateCompliance(pplat, peep);



  // envio de datos
  if(millis()%100 ==0)logData();
  ventilation->update();

#ifdef LOCALCONTROLS
  if (encoderRPM.updateValue(&rpm))
  {
    ventilation->setRpm(rpm);
    display.writeLine(0, "RPM: " + String(ventilation->getRpm()));
  }

  if (encoderTidal.updateValue(&vTidal, 10))
  {
    ventilation->setTidalVolume(vTidal);
    display.writeLine(1, "Vol Tidal: " + String(ventilation->getTidalVolume()));
  }

  if (encoderPorcInspira.updateValue(&porcentajeInspiratorio, 1))
  {
    ventilation->setPorcentajeInspiratorio(porcentajeInspiratorio);
    display.writeLine(3, "% Insp: " + String(ventilation->getporcentajeInspiratorio()));
  }
#endif

}
