#ifndef ANALOG_LINEAR_PRESSURE_SENSOR_H
#define ANALOG_LINEAR_PRESSURE_SENSOR_H

#include "Sensor/ApolloPressureSensor.h"
#include <Arduino.h>

class AnalogLinearPressure : public ApolloPressureSensor
{
    public:
        AnalogLinearPressure(uint8_t pin_, int min_, int max_);
        bool    begin();
        float   readPascal();
        void    update(){};
        float   readBar();

    protected:
        uint8_t     pin;
        //Min presure read
        int         min;
        //Max presure read
        int         max;
};


#endif
