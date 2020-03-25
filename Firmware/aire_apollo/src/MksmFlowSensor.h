#ifndef MKSM_FLOW_SENSOR_H
#define MKSM_FLOW_SENSOR_H

#include "ApolloFlowSensor.h"

class MksmFlowSensor : public ApolloFlowSensor
{
    public:
        MksmFlowSensor();
        ~MksmFlowSensor();
        virtual bool begin();
        virtual float readFlow();
    private:
        
};


#endif