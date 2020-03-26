#ifndef APOLLO_PRESSURE_SENSOR_H
#define APOLLO_PRESSURE_SENSOR_H


// This class MUST be derived
class ApolloPressureSensor
{
    public:
        ApolloPressureSensor() {};
        virtual ~ApolloPressureSensor() {};
        virtual bool begin();                // to be able to report error if the sensor is not detected
        virtual float read();
        virtual float readHpa();        
        virtual float readMMHg();
    protected:
        
};


#endif