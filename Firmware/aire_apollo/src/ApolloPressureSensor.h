#ifndef APOLLO_PRESSURE_SENSOR_H
#define APOLLO_PRESSURE_SENSOR_H


// This class MUST be derived
class ApolloPressureSensor
{
    public:
        ApolloPressureSensor() {};
        virtual ~ApolloPressureSensor() {};
        virtual int begin();                // to be able to report error if the sensor is not detected
        virtual float readPressure();
    private:
        
};


#endif