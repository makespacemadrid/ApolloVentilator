#ifndef APOLLO_PRESSURE_SENSOR_H
#define APOLLO_PRESSURE_SENSOR_H

#include "Arduino.h"
#include "../../../include/defaults.h"
#include "trace.h"

// This class MUST be derived
//MUST implement readPascal() for your sensor!!!
class ApolloPressureSensor
{
    public:
        ApolloPressureSensor() {};
        virtual ~ApolloPressureSensor() {};
        virtual bool begin() {Serial.println("METODO VIRTUAL SIN IMPLEMENTAR!") ;return true;}              // to be able to report error if the sensor is not detected
        virtual float readPascal() = 0; //Implementar en la herencia!!!!!!
        virtual float readMilibar();
        virtual float readMMHg();
        virtual float readCMH2O();
        virtual void  update() {;}
        virtual void  highFreqUpdate() {;}

    protected:

};


#endif
