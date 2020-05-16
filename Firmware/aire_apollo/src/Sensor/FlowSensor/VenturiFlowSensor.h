#ifndef VENTURI_FLOW_SENSOR_H
#define VENTURI_FLOW_SENSOR_H

#include "Arduino.h"
#include "../../../include/defaults.h"
#include "trace.h"
#include "Sensor/FlowSensor/ApolloFlowSensor.h"
#include "Sensor/Pressure/ApolloPressureSensor.h"

class VenturiFlowSensor : public ApolloFlowSensor
{
     public:
        VenturiFlowSensor(ApolloPressureSensor *sensor, uint8_t sampling_ms = 10);
        ~VenturiFlowSensor();
        virtual bool begin();
        float getFlow();
        float getInstantFlow();
        void resetFlow();
        virtual void update();

    protected:
        ApolloPressureSensor*    _sensor;
        
        uint8_t       _samplingMS;
        float         _volSinceReset;               // This value represent number of ml provided to the patient since last cycle reset
        unsigned long _lastSampleTime;
        float         _lastInstantFlow;
        void        softReset();
        virtual int readBytes();        
        void        startFlowMeasurement();        
};


#endif
