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
    bool  test();
    bool  calibrate();


    bool  setFlowMode     (float flow, float maxPressure);
    bool  setPressureMode (float pressure);


    void openInputValve     (uint8_t percent = 100, bool wait = false);
    void closeInputValve    (bool wait = false);
    void openOutputValve    (uint8_t percent = 100, bool wait = false);
    void closeOutputValve   (bool wait = false);


    double getPressure()          { return _lastPressure;}
    double getInputValveStatus()  { return _lastInputValveStatus;}
    double getOutputValveStatus() { return _lastOutputValveStatus;}
    double getInputValveTarget()  { return _inputValve->target();}
    double getOutputValveTarget() { return _outputValve->target();}

    double getInputFlow()         { return _lastInputFlow;}
    double getInputInstantFlow()  { return _lastInputInstantFlow;}
    void   resetInputFlow()       { _inputFlowSensor->resetFlow();}

    double getOutputFlow()        { return _lastOutputFlow;}
    double getOutputInstantFlow() { return _lastOutputInstantFlow;}
    void   resetOutputFlow()      { _outputFlowSensor->resetFlow();}


    void update();
    void highFrecuencyUpdate();

  #ifdef INTFLOWSENSOR // Gestion de los sensores de flujo que usan interrupciones
    void flowIn()
    {
      _inputFlowSensor->pulse();
    }

    void flowOut()
    {
      _outputFlowSensor->pulse();
    }
  #endif


  private:

    void computePIDs();

    ApolloPressureSensor  *_inputPressureSensor;
    ApolloFlowSensor      *_inputFlowSensor;
    ApolloFlowSensor      *_outputFlowSensor;
    ApolloValve           *_inputValve;
    ApolloValve           *_outputValve;
    ApolloAlarms          *_alarms;

//
    float  _lastInputValveStatus;
    float  _lastOutputValveStatus;
    float  _lastPressure;
    float  _lastInputFlow;
    float  _lastOutputFlow;
    float  _lastInputInstantFlow;
    float  _lastOutputInstantFlow;



//CONTROL PID PRESSURE
    PID     _inputPressurePID;
    double  _inputPressurePIDInput;
    double  _inputPressurePIDTarget;
    double  _inputPressurePIDOutput;
    double  _inputPressurePIDKp;
    double  _inputPressurePIDKd;
    double  _inputPressurePIDKi;

    PID     _outputPressurePID;
    double  _outputPressurePIDInput;
    double  _outputPressurePIDTarget;
    double  _outputPressurePIDOutput;
    double  _outputPressurePIDKp;
    double  _outputPressurePIDKd;
    double  _outputPressurePIDKi;

//CONTROL PID FLOW
    PID    _flowPID;
    double _flowPIDInput;
    double _flowPIDTarget;
    double _flowPIDOutput;
    double _flowPIDKp;
    double _flowPIDKd;
    double _flowPIDKi;

//
    uint16_t _sensorLoopMS;
    uint16_t _highFreqUpdateMillis;
};

#endif
