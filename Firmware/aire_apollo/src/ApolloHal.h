#ifndef APOLLO_HALL_H
#define APOLLO_HALL_H

#include "ApolloFlowSensor.h"
#include "ApolloPressureSensor.h"
#include "ApolloValve.h"
#include "ApolloAlarms.h"
class ApolloHal
{
private:
    ApolloPressureSensor *_preSensor;
    ApolloFlowSensor *_entryFlowSensor;
    ApolloFlowSensor *_exitFlowSensor;
    ApolloValve *_entryEV;
    ApolloValve *_exitEV;

    ApolloAlarms *alarms;

    void openEntryEV();
    void openExitEV();
    bool valveExsState;
    bool valveInsState;

public:
    ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *entryFlowSensor, ApolloFlowSensor *exitFlowSensor, ApolloValve *entryEV, ApolloValve *exitEV, ApolloAlarms *alarms);
    ~ApolloHal();

    bool begin();
    void setFlow(float flow, float pressure);
    float getMetricPressureEntry();
    float getMetricVolumeEntry();
    float getMetricVolumeExit();
    void beginInspiration();
    void beginEspiration();

    void ISR1ms(); //

    ApolloPressureSensor *pressuresSensor() { return _preSensor; }
    ApolloFlowSensor *intakeFlowSensor() { return _entryFlowSensor; }
    ApolloFlowSensor *exitFlowSensor() { return _exitFlowSensor; }
    ApolloValve *intakeValve() { return _entryEV; }
    ApolloValve *exitValve() { return _exitEV; }

    //Test only
    void valveInsOpen();
    void valveInsClose();
    void valveExsOpen();
    void valveExsClose();

    int getPresureIns();
    int getPresureExp();
    bool getValveExsState() { return valveExsState; };
    bool getValveInsState() { return valveInsState; };
};

#endif
