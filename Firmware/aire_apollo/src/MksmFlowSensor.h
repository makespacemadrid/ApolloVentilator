#ifndef MKSM_FLOW_SENSOR_H
#define MKSM_FLOW_SENSOR_H

#include "ApolloFlowSensor.h"

class MksmFlowSensor : public ApolloFlowSensor
{
    public:
        MksmFlowSensor(uint16_t pulses_per_liter = 100,uint8_t sampling_ms = 100);
        ~MksmFlowSensor();
        bool begin();
        float getFlow();
        float getInstantFlow();
        void resetFlow();
        void update();
        void pulse();

    protected:
        uint16_t      _pulsesPerLiter;
        uint8_t       _samplingMS;
        uint16_t      _pulsesSinceLastSample;
        uint16_t      _instantFlow;
        unsigned long _pulseCounter;
        unsigned long _lastSampleTime;
};


#endif
