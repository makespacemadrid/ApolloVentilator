#include "ApolloHal.h"
#include <Arduino.h>
#include "trace.h"
#include <PID_v1.h>
#include <PID_AutoTune_v0.h>

ApolloHal::ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *entryFlowSensor, ApolloFlowSensor *exitFlowSensor, ApolloValve *entryEV, ApolloValve *exitEV, ApolloAlarms *alarms)
{
  _preSensor = preSensor;
  _entryFlowSensor = entryFlowSensor;
  _exitFlowSensor = exitFlowSensor;
  _entryEV = entryEV;
  _exitEV = exitEV;
  alarms = alarms;
  this->initializePidFlowIns();
  this->initializePidPressureIns();
  this->initializePidPressureExp();
}

ApolloHal::~ApolloHal()
{
}

/**
 * @brief Initialize and check all sensor
 *
 * @return true if all sensor are present a initilized
 * @return false if something goes wrong
 */
bool ApolloHal::begin()
{

  bool status = true;

  if (!_entryFlowSensor->begin())
  {
    TRACE("ERROR FLOW-IN!");
    status = false;
  }

  if (!_exitFlowSensor->begin())
  {
    TRACE("ERROR FLOW-OUT");
    status = false;
  }

  if (!_entryEV->begin())
  {
    TRACE("ERROR VALVE-IN");
    status = false;
  }

  if (!_exitEV->begin())
  {
    TRACE("ERROR VALVE-OUT");
    status = false;
  }
  TRACE("VERIFY PRESION!");
  // Close Entry Valve and open exit valve and wait 3 sec to empty the pressure in the system
  // This it's neccesary to reset 0 the presure in the sensor
  _entryEV->close();
  _exitEV->open();
  delay(3000);
  if (!_preSensor->begin())
  {
    TRACE("ERROR PRESION!");
    status = false;
  }

  return status;
}

void ApolloHal::ISR1ms()
{
  _entryFlowSensor->update();
  _exitFlowSensor->update();
  _entryEV->update();
  _exitEV->update();
  _preSensor->update();
}

void ApolloHal::setFlow(float flow, float pressure)
{
}

// Get metric from pressure sensor in mBar
float ApolloHal::getMetricPressureEntry()
{
  // preSensor MUST return value in mBar
  return _preSensor->readMilibar();
  ;
}

/**
 * Inspiration pressure
 * @param bool cache Return value of cache or not; Default false;
 * 
 */
int ApolloHal::getPresureIns(bool cache)
{
  if (cache)
  {
    this->currentPressureIns;
  }
  else
  {
    return this->pressuresSensor()->readCMH2O();
  }
}
/**
 * Expiration pressure
 * @param bool cache Return value of cache or not; Default false;
 * 
 */
int ApolloHal::getPresureExp(bool cache)
{
  if (cache)
  {
    this->currentPressureIns;
  }
  else
  {
    return this->pressuresSensor()->readCMH2O();
  }
}

void ApolloHal::valveInsOpen(float pressureTarget)
{
  this->setPressureInsTarget(pressureTarget);
}
void ApolloHal::valveInsClose()
{
  this->enablePressureIns = false;
  this->_entryEV->close();
}
void ApolloHal::valveExsOpen(float pressureTarget)
{
  this->setPressureExpTarget(pressureTarget);
}
void ApolloHal::valveExsClose()
{
  this->enablePressureExp = false;
  this->_exitEV->close();
}

// Get metric from entry flow sensor
float ApolloHal::getMetricVolumeEntry()
{
  return _entryFlowSensor->getInstantFlow();
}

// Get metric from pressure sensor in mBar
float ApolloHal::getMetricVolumeExit()
{
  // preSensor MUST return value in mBar
  return _exitFlowSensor->getInstantFlow();
  ;
}

void ApolloHal::initializePidPressureIns()
{
  this->pidPressureIns.SetTunings(_consKp, _consKi, _consKd);
  this->pidPressureIns.SetMode(AUTOMATIC);
}
void ApolloHal::initializePidPressureExp()
{
  this->pidPressureExp.SetTunings(_consKp, _consKi, _consKd);
  this->pidPressureExp.SetMode(AUTOMATIC);
}
void ApolloHal::initializePidFlowIns()
{
  this->pidFlowIns.SetTunings(_consKp, _consKi, _consKd);
  this->pidFlowIns.SetMode(AUTOMATIC);
}

void ApolloHal::setPressureInsTarget(int pressure)
{
  this->enablePressureIns = true;
  this->pressureInsTarget = pressure;
}
void ApolloHal::setPressureExpTarget(int pressure)
{
  this->enablePressureExp = true;
  this->pressureExpTarget = pressure;
}
void ApolloHal::setFlowInsTarget(float flow)
{
  this->enableFlowIns = true;
  this->flowInsTarget = flow;
}

void ApolloHal::prepare()
{
  this->currentPressureIns = this->getPresureIns(false);
  this->currentPressureExp = this->getPresureExp(false);
  this->statusPressureIns = this->_entryEV->status();
  this->statusPressureExp = this->_exitEV->status();
  this->statusFlowIns = this->_entryEV->status();
}
void ApolloHal::update()
{
  this->pidPressureInsCompute();
  this->pidPressureExpCompute();
  this->pidFlowInsCompute();
}

void ApolloHal::pidPressureInsCompute()
{
  if (!this->enablePressureIns)
  {
    return;
  }

  double gap = abs(this->pressureInsTarget - this->getPresureIns(true)); //distance away from setpoint
  if (gap < 5)
  { //we're close to setpoint, use conservative tuning parameters
    this->pidPressureIns.SetTunings(_consKp, _consKi, _consKd);
  }
  else
  {
    //we're far from setpoint, use aggressive tuning parameters
    this->pidPressureIns.SetTunings(_aggKp, _aggKi, _aggKd);
  }
  this->pidPressureIns.Compute();

  if (this->statusPressureIns > 100)
    this->statusPressureIns = 100;
  if (this->statusPressureIns < 0)
    this->statusPressureIns = 0;
  this->_entryEV->open(this->statusPressureIns);

  //Serial.println("PID: current:" + String(_currentPressure) + " Target:"  + String(_targetPressure) + " Output:" + String(_inputValvePercent) );
}

void ApolloHal::pidPressureExpCompute()
{
  if (!this->enablePressureExp)
  {
    return;
  }

  float _aggKp = 15, _aggKi = 0.0, _aggKd = 1;
  float _consKp = 1, _consKi = 0.0, _consKd = 0.1;

  double gap = abs(this->pressureExpTarget - this->getPresureExp(true)); //distance away from setpoint
  if (gap < 5)
  { //we're close to setpoint, use conservative tuning parameters
    this->pidPressureExp.SetTunings(_consKp, _consKi, _consKd);
  }
  else
  {
    //we're far from setpoint, use aggressive tuning parameters
    this->pidPressureExp.SetTunings(_aggKp, _aggKi, _aggKd);
  }

  this->pidPressureExp.Compute();

  if (this->statusPressureExp > 100)
    this->statusPressureExp = 100;
  if (this->statusPressureExp < 0)
    this->statusPressureExp = 0;
  this->_exitEV->open(this->statusPressureExp);

  //Serial.println("PID: current:" + String(_currentPressure) + " Target:"  + String(_targetPressure) + " Output:" + String(_inputValvePercent) );
}

void ApolloHal::pidFlowInsCompute()
{
  if (!this->enableFlowIns)
  {
    return;
  }

  float _aggKp = 15, _aggKi = 0.0, _aggKd = 1;
  float _consKp = 1, _consKi = 0.0, _consKd = 0.1;

  double gap = abs(this->flowInsTarget - this->getPresureIns(true)); //distance away from setpoint
  if (gap < 5)
  { //we're close to setpoint, use conservative tuning parameters
    this->pidFlowIns.SetTunings(_consKp, _consKi, _consKd);
  }
  else
  {
    //we're far from setpoint, use aggressive tuning parameters
    this->pidFlowIns.SetTunings(_aggKp, _aggKi, _aggKd);
  }

  this->pidFlowIns.Compute();

  if (this->statusFlowIns > 100)
    this->statusFlowIns = 100;
  if (this->statusFlowIns < 0)
    this->statusFlowIns = 0;
  this->_entryEV->open(this->statusFlowIns);

  //Serial.println("PID: current:" + String(_currentPressure) + " Target:"  + String(_targetPressure) + " Output:" + String(_inputValvePercent) );
}