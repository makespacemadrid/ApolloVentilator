#ifndef APOLLO_HALL_H
#define APOLLO_HALL_H

#include "ApolloFlowSensor.h"
#include "ApolloPressureSensor.h"
#include "ApolloValve.h"

class ApolloHal
{
private:
    ApolloPressureSensor *preSensor;
    ApolloFlowSensor *flowSensor;
    ApolloValve *entryEV;
    ApolloValve *exitEV;

    void openEntryEV();
    void openExitEV();

public:
    ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *flowSensor, ApolloValve *enrtyEV, ApolloValve *exitEV);
    ~ApolloHal();

    bool begin();
    void setFlow(float flow, float pressure);
    float getMetricPressureEntry();

    //Test only
    void valveInsOpen();
    void valveInsClose();
    void valveExsOpen();
    void valveExsClose();

    int getPresureIns();
};

#endif