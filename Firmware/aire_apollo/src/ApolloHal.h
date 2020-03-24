#ifndef APOLLO_HALL_H
#define APOLLO_HALL_H

#include "ApolloFlowSensor.h"
#include "ApolloPressureSensor.h"
#include "ApolloValve.h"

class ApolloHal
{
    private:        
        ApolloPressureSensor    preSensor;
        ApolloFlowSensor        flowSensor;
        ApolloValve             entryEV;
        ApolloValve             exitEV;

        void openEntryEV();
        void openExitEV();
        

    public:
        ApolloHal();
        ~ApolloHal();

        void setFlow(float flow, float pressure);
        float getMetricPressureEntry();
    
};


#endif