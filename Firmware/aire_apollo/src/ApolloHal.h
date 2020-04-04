#ifndef APOLLO_HALL_H
#define APOLLO_HALL_H

#include "ApolloFlowSensor.h"
#include "ApolloPressureSensor.h"
#include "ApolloValve.h"
#include "ApolloAlarms.h"
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

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

    double _targetPressure, _currentPressure, _inputValvePercent;
    //Define the aggressive and conservative Tuning Parameters
    double _aggKp = 15, _aggKi = 0.0, _aggKd = 1;
    double _consKp = 1, _consKi = 0.0, _consKd = 0.1;

    //Cache pressure to prevent overload values of sensors
    double currentPressureIns;
    double currentPressureExp;
    double currentFlowIns;

    double statusPressureIns;
    double statusPressureExp;
    double statusFlowIns;

    //Targets of PID
    double pressureInsTarget;
    double pressureExpTarget;
    double flowInsTarget;

    bool enableFlowIns = false;
    bool enablePressureIns = false;
    bool enablePressureExp = false;

    PID pidPressureIns = PID(&currentPressureExp, &statusPressureIns, &pressureExpTarget, _consKp, _consKi, _consKd, DIRECT);
    PID pidFlowIns = PID(&currentPressureIns, &statusPressureExp, &pressureInsTarget, _consKp, _consKi, _consKd, DIRECT);
    PID pidPressureExp = PID(&currentFlowIns, &statusFlowIns, &flowInsTarget, _consKp, _consKi, _consKd, DIRECT);

    void initializePidPressureIns();
    void initializePidPressureExp();
    void initializePidFlowIns();

    void pidPressureInsCompute();
    void pidFlowInsCompute();
    void pidPressureExpCompute();

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
    void valveInsOpen(float pressureTarget);
    void valveInsClose();
    void valveExsOpen(float pressureTarget);
    void valveExsClose();

    int getPresureIns(bool cache = false);
    int getPresureExp(bool cache = false);
    bool getValveExsState() { return this->_exitEV->status(); };
    bool getValveInsState() { return this->_entryEV->status(); };

    void setPressureInsTarget(int pressure);
    void setPressureExpTarget(int pressure);
    void setFlowInsTarget(float flow);

    void prepare();
    void update();
};

#endif
