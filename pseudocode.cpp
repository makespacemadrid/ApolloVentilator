#include <Arduino.h>
# define INSPIRATION_TIME 1000
# define ESPIRATION_TIME 4000

// Measurements
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

// Alerts (pending validation):
// High pressure alert
// Low pressure alert
// Compliance alert
// O2 alert
// CO2 alert
// Leak alert

void openEntryEV(){}
float getMetricVolumeEntry(){}
float getMetricO2Insp(){}
void checkO2Saturation(int current, int limit){}
int getMetricPpeak(){}
int getMetricPplat(){}
int calculateResistance (int ppeak, int pplat) {
	return ppeak - pplat;
}
int getMetricPeep(){}
void closeEntryEV(){}
void openExitEV(){}
float getMetricVolumeExit(){}
void checkLeak(float volEntry, float volExit){}
float getMetricCO2Esp(){}
void checkCO2Saturation(int current, int limit){}
void checkCompliance(int pplat, int peep) {
	int result = getCompliance(pplat, peep);
	if (result >= 12) {
		Serial.println("OK");
	} else if (result <= 15) {
		Serial.println("OK");
	} else {
		Serial.println("COMPLIANCE ALERT");
	}
}
void closeExitEV(){}

float getMetricVolMax(){}
float getMetricPresMax(){}
int calculateCompliance (int pplat, int peep) {
	return pplat - peep;
}

void setup() {
	Serial.begin(115200);
}


void loop() {
	openEntryEV()
	if(millis() >= inspiration + INSPIRATION_TIME){
		inspiration +=INSPIRATION_TIME;
		Serial.println("Insp");
	}	
	getMetricVolumeEntry();
	float currentO2 = getMetricO2Insp();
	checkO2Saturation(currentO2, limitO2);
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
	getMetricVolumeExit();
	checkLeak();
	float currentCO2 = getMetricCO2Esp();
	checkC02Saturacion(currentCO2, limitCO2);
	calculateCompliance(pplat, peep);
	closeExitEV();
} 	