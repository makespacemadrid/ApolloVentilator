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
    ApolloPressureSensor *preSensor_;
    ApolloFlowSensor *entryFlowSensor_;
    ApolloFlowSensor *exitFlowSensor_;
    ApolloValve *entryEV_;
    ApolloValve *exitEV_;

    ApolloAlarms *alarms_;

    void openEntryEV();
    void openExitEV();

    //Define the aggressive and conservative Tuning Parameters
    double c_consKp = 1, c_consKi = 0.0, c_consKd = 0.1;

    //Cache pressure to prevent overload values of sensors
    double currentPressureIns_;
    double currentPressureExs_;
    double currentFlowIns_;

    double statusPressureIns_;
    double statusPressureExs_;
    double statusFlowIns_;

    //Targets of PID
    double pressureInsTarget_;
    double pressureExsTarget_;
    double flowInsTarget_;

    bool enableFlowIns_ = false;
    bool enablePressureIns_ = false;
    bool enablePressureExs_ = false;

    PID pidPressureIns_ = PID(&this->currentPressureIns_, &this->statusPressureIns_, &this->pressureInsTarget_, c_consKp, c_consKi, c_consKd, DIRECT);
    PID pidPressureExs_ = PID(&this->currentPressureExs_, &this->statusPressureExs_, &this->pressureExsTarget_, c_consKp, c_consKi, c_consKd, DIRECT);
    PID pidFlowIns_ = PID(&this->currentFlowIns_, &this->statusFlowIns_, &this->flowInsTarget_, c_consKp, c_consKi, c_consKd, DIRECT);

    void initializePidPressureIns();
    void initializePidPressureExs();
    void initializePidFlowIns();

    void pidPressureInsCompute();
    void pidFlowInsCompute();
    void pidPressureExsCompute();

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

    ApolloPressureSensor *pressuresSensor() { return preSensor_; }
    ApolloFlowSensor *intakeFlowSensor() { return entryFlowSensor_; }
    ApolloFlowSensor *exitFlowSensor() { return exitFlowSensor_; }
    ApolloValve *intakeValve() { return entryEV_; }
    ApolloValve *exitValve() { return exitEV_; }

    //Test only
    void valveInsOpen(double pressureTarget);
    void valveInsClose();
    void valveExsOpen(double pressureTarget);
    void valveExsClose();

    double getPresureIns(bool cache = false);
    double getPresureExs(bool cache = false);
    bool getValveExsState() { return this->exitEV_->status(); };
    bool getValveInsState() { return this->entryEV_->status(); };

    void setPressureInsTarget(double pressure);
    void setPressureExsTarget(double pressure);
    void setFlowInsTarget(double flow);

    void prepare();
    void update();
};

#endif
