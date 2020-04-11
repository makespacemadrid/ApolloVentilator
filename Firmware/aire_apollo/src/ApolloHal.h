#ifndef APOLLO_HALL_H
#define APOLLO_HALL_H

#include "../include/defaults.h"
#include "Sensor/FlowSensor/ApolloFlowSensor.h"
#include "Sensor/Pressure/ApolloPressureSensor.h"
#include "Valve/ApolloValve.h"
#include "ApolloAlarms.h"
//#include <AutoPID.h>
#include <PID_v1.h>
class ApolloHal
{

public:
    ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *entryFlowSensor, ApolloFlowSensor *exitFlowSensor, ApolloValve *entryEV, ApolloValve *exitEV, ApolloAlarms *alarms);
    ~ApolloHal();

    bool  begin();
    void  setFlow(float flow, float pressure);


    void valveInsOpen(uint8_t percent = 100);
    void valveInsClose();
    void valveExsOpen(uint8_t percent = 100);
    void valveExsClose();

    void setPressureInsTarget(double pressure);
    void setPressureExsTarget(double pressure);
    void setFlowInsTarget    (double flow);


    double getPresureIns(bool cache = false);
    double getPresureExs(bool cache = false);
    double getEntryValveStatus()  { return this->entryEV_->status(); };
    double getExitValveStatus()   { return this->exitEV_->status(); };

    double getEntryFlow()         { return this->entryFlowSensor_->getFlow();}
    double getEntryInstantFlow()  { return this->entryFlowSensor_->getInstantFlow();}
    void   resetEntryFlow()       { return this->entryFlowSensor_->resetFlow();}
    double getExitFlow()          { return this->exitFlowSensor_->getFlow();}
    double getExitInstantFlow()   { return this->exitFlowSensor_->getInstantFlow();}
    void   resetExitFlow()        { return this->exitFlowSensor_->resetFlow();}


    void updateSensors();
    void pidCompute();
    void ISR1ms(); //

  #ifdef INTFLOWSENSOR // Gestion de los sensores de flujo por interrupcion
    void flowIn()
    {
      entryFlowSensor_->pulse();
    }

    void flowOut()
    {
      exitFlowSensor_->pulse();
    }
  #endif


  private:
      ApolloPressureSensor  *entryPressureSensor_;
      ApolloFlowSensor      *entryFlowSensor_;
      ApolloFlowSensor      *exitFlowSensor_;
      ApolloValve           *entryEV_;
      ApolloValve           *exitEV_;

      ApolloAlarms *alarms_;

      void openEntryEV();
      void openExitEV();

      //Define the aggressive and conservative Tuning Parameters
      double c_consKp = 5, c_consKi = 3, c_consKd = 1;

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

      bool enableFlowIns_     = false;
      bool enablePressureIns_ = false;
      bool enablePressureExs_ = false;

      PID pidPressureIns_;// = PID(&this->currentPressureIns_, &this->statusPressureIns_, &this->pressureInsTarget_, c_consKp, c_consKi, c_consKd, DIRECT);
      PID pidPressureExs_;// = PID(&this->currentPressureExs_, &this->statusPressureExs_, &this->pressureExsTarget_, c_consKp, c_consKi, c_consKd, REVERSE);
      PID pidFlowIns_;    // = PID(&this->currentFlowIns_    , &this->statusFlowIns_    , &this->flowInsTarget_    , c_consKp, c_consKi, c_consKd, DIRECT);

      void initializePidPressureIns();
      void initializePidPressureExs();
      void initializePidFlowIns();

      void pidPressureInsCompute();
      void pidFlowInsCompute();
      void pidPressureExsCompute();

};

#endif
