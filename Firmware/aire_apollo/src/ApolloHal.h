#ifndef APOLLO_HALL_H
#define APOLLO_HALL_H

#include "../include/defaults.h"
#include "strings.h"

#include "Sensor/FlowSensor/ApolloFlowSensor.h"
#include "Sensor/Pressure/ApolloPressureSensor.h"
#include "Valve/ApolloValve.h"
#include "ApolloStorage.h"
#include "ApolloPID.h"
#include "ApolloProtocol.h"

enum controlMode
{
  idleMode,
  inpiratoryRiseSpontaneousMode,
  inspiratoryRisePressureMode,
  inspiratoryRiseFlowMode,
  expiratoryReleaseMode,
  expiratoryPEEPMode,
  pressureReleaseMode
};



class ApolloHal
{

public:
//    ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *entryFlowSensor, ApolloFlowSensor *exitFlowSensor, ApolloValve *entryEV, ApolloValve *exitEV, ApolloAlarms *alarms);
    ApolloHal(ApolloStorage* storage);
    ~ApolloHal();

    void addValves        (ApolloValve* inputValve,ApolloValve* outputValve);
    void addPressureSensor(ApolloPressureSensor* pressureSensor);
    void addFlowSensors   (ApolloFlowSensor* inputFlow, ApolloFlowSensor* outputFlow);

    void debug(String debugmsg);

    hardwareStatus  begin();
    bool  test();
    bool  calibrate();
    bool  calibratePressure();
    bool  checkOverPressure();

    bool  inspiratoryRisePressure (float pressure, uint16_t targetTime);
    bool  inspiratoryRiseFlow     (float flow, uint16_t targetTime, float maxPressure);
    bool  expiratoryRelease       (float peep,bool wait = false,uint16_t timeout = 5000);
    bool  releasePressure         (bool wait = false,uint16_t timeout = 10000);

    void openInputValve     (uint8_t percent = 100, bool wait = false)  {_inputValve->open(percent, wait);}
    void closeInputValve    (bool wait = false)                         {_inputValve->close(wait);}
    void openOutputValve    (uint8_t percent = 100, bool wait = false)  {_outputValve->open(percent, wait);}
    void closeOutputValve   (bool wait = false)                         {_outputValve->close(wait);}

    hardwareStatus getHWstatus()  { return _hwStatus;}
    double getPressure()          { return _lastPressure;}
    double getPressureTarget()    { return _pressureTarget;}
    double getInputValveStatus()  { return _lastInputValveStatus;}
    double getOutputValveStatus() { return _lastOutputValveStatus;}
    double getInputValveTarget()  { return _lastInputValveTarget;}
    double getOutputValveTarget() { return _lastOutputValveTarget;}

    double getInputFlow()         { return _lastInputFlow;}
    double getInputInstantFlow()  { return _lastInputInstantFlow;}
    void   resetInputFlow()       { _inputFlowSensor->resetFlow();}

    double getOutputFlow()        { return _lastOutputFlow;}
    double getOutputInstantFlow() { return _lastOutputInstantFlow;}
    void   resetOutputFlow()      { _outputFlowSensor->resetFlow();}

    void sendHWInfo()             {ApolloProtocol::sendHWInfo(_lastPressure,_pressureTarget,_lastInputInstantFlow,_lastOutputInstantFlow,_lastInputValveStatus, _lastInputValveTarget,_lastOutputValveStatus,_lastOutputValveTarget) ;}


    void      resetLoopCounters()     { _sensorLoops = 0,_hfLoops=0;}
    uint16_t  getSensorLoops()        { return _sensorLoops;}
    uint16_t  getHfLoops()            { return _hfLoops;}
    float     getAvgSensorLoopMicros(){ return _avgSensorLoopMicros;}
    float     getAvgHfLoopMicros()    { return _avgHighFreqUpdateMicros;}
    String    getLastErrorString()    { return _lastError;}


    void update();


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

    void initPIDs();
    void computePIDs();
    void autotunePressurePID(float target);
    void highFrecuencyUpdate();
    void sensorUpdate();
    void updateControl();
    bool loadCalibration();

    void handleIdleMode();
    void handleInpiratoryRiseSpontaneousMode();
    void handleInspiratoryRisePressureMode();
    void handleInspiratoryRiseFlowMode();
    void handleExpiratoryReleaseMode();
    void handleExpiratoryPEEPMode();
    void handlePressureReleaseMode();

    ApolloStorage         *_storage;
    ApolloPressureSensor  *_inputPressureSensor;
    ApolloFlowSensor      *_inputFlowSensor;
    ApolloFlowSensor      *_outputFlowSensor;
    ApolloValve           *_inputValve;
    ApolloValve           *_outputValve;
//    ApolloAlarms          *_alarms;

//
    controlMode    _mode;
    hardwareStatus _hwStatus;
    float  _lastInputValveStatus;
    float  _lastOutputValveStatus;
    float  _lastInputValveTarget;
    float  _lastOutputValveTarget;
    float  _lastPressure;
    float  _lastInputFlow;
    float  _lastOutputFlow;
    float  _lastInputInstantFlow;
    float  _lastOutputInstantFlow;
/*
    unsigned long _lastPressureRiseSample;
    float         _lastPressureRisePressure;
    unsigned int  _lastPressureBrakeTime;
    unsigned long _lastPressureBrakeStart;
    bool          _pressureRising;
    bool          _pressureBraking;

    uint16_t      _lastInspiratoryRiseTimeMS;
    unsigned long _lastInspiratoryRiseStart;
    bool          _pressureTargetArchived;
*/

///Parametros del ultimo ciclo(fallback)
    float         _lastInspiratoryValveStatus;
    uint16_t      _lastInspiratoryValveTime;

    bool          _hasPressureSensor;
    bool          _hasFlowSensors;

    float        _pressureTarget;

//CONTROL PID
    pidv1lib _constantPressurePID;
    pidv1lib _constantFlowPID;
    pidv1lib _overPressurePID;
    pidv1lib _inspiratoryRisePID;
    pidv1lib _expiratoryPID;
    bool     _overPressureTriggered;



    float    _avgSensorLoopMicros      = 0;
    float    _avgHighFreqUpdateMicros  = 0;
    uint32_t _sensorLoops             = 0;
    uint32_t _hfLoops                 = 0;
    unsigned long _lastSensorsUpdate  = 0;

    String   _lastError;
};

#endif
