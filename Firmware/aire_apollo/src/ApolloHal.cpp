#include "ApolloHal.h"
#include <Arduino.h>
#define DEBUG
#include "trace.h"

ApolloHal::ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *entryFlowSensor, ApolloFlowSensor *exitFlowSensor, ApolloValve *entryEV, ApolloValve *exitEV, ApolloAlarms *alarms) :
    _inputPressurePID  (&_inputPressurePIDInput  , &_inputPressurePIDOutput , &_inputPressurePIDTarget , _inputPressurePIDKp , _inputPressurePIDKi , _inputPressurePIDKd , DIRECT),
    _outputPressurePID (&_outputPressurePIDOutput, &_outputPressurePIDOutput, &_outputPressurePIDTarget, _outputPressurePIDKp, _outputPressurePIDKi, _outputPressurePIDKd, REVERSE),
    _flowPID           (&_flowPIDInput           , &_flowPIDOutput          , &_flowPIDTarget          , _flowPIDKp          , _flowPIDKi          , _flowPIDKd          , DIRECT)

{
  _inputPressureSensor = preSensor;
  _inputFlowSensor = entryFlowSensor;
  _outputFlowSensor = exitFlowSensor;
  _inputValve = entryEV;
  _outputValve = exitEV;
  _alarms = alarms;
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

  if (!this->_outputValve->begin())
  {
    TRACE("ERROR VALVE-OUT");
    status = false;
    return false;
  }

  _outputValve->open(100,true);

  if (!this->_inputValve->begin())
  {
    TRACE("ERROR VALVE-IN");
    status = false;
    return false;
  }

  _inputValve->close(true);
  delay(1000);

  if (!this->_inputFlowSensor->begin())
  {
    TRACE("ERROR FLOW-IN!");
    status = false;
  }

  if (!this->_outputFlowSensor->begin())
  {
    TRACE("ERROR FLOW-OUT");
    status = false;
  }


  if (!this->_inputPressureSensor->begin())
  {
    TRACE("ERROR PRESION!");
    status = false;
  }

  return status;
}

bool ApolloHal::test()
{
  return false;
}

bool ApolloHal::calibrate()
{
  return false;
}

bool ApolloHal::setFlowMode(float flow, float maxPressure)
{
  return false;
}

bool ApolloHal::setPressureMode(float pressure)
{
  return false;
}



void ApolloHal::openInputValve(uint8_t percent,bool wait)
{
  _inputValve->open(percent, wait);
}

void ApolloHal::closeInputValve(bool wait)
{
  _inputValve->close(wait);
}

void ApolloHal::openOutputValve(uint8_t percent, bool wait)
{
  _outputValve->open(percent, wait);
}

void ApolloHal::closeOutputValve(bool wait)
{
  _outputValve->close(wait);
}

void ApolloHal::highFrecuencyUpdate()
{
  _inputValve->highFreqUpdate();
  _outputValve->highFreqUpdate();
  _inputFlowSensor->highFreqUpdate();
  _outputFlowSensor->highFreqUpdate();
  _inputPressureSensor->highFreqUpdate();

}

void ApolloHal::update()
{
  _inputValve->update();
  _outputValve->update();
  _inputFlowSensor->update();
  _outputFlowSensor->update();
  _inputPressureSensor->update();

  _lastPressure          = _inputPressureSensor->readCMH2O();
  _lastInputValveStatus  = _inputValve->status();
  _lastOutputValveStatus = _outputValve->status();
  _lastInputFlow         = _inputFlowSensor->getFlow();
  _lastOutputFlow        = _outputFlowSensor->getFlow();
  _lastInputInstantFlow  = _inputFlowSensor->getInstantFlow();
  _lastOutputInstantFlow = _outputFlowSensor->getInstantFlow();
  computePIDs();
}

void ApolloHal::computePIDs()
{

}
/*
void ApolloHal::pidPressureInsCompute()
{
  if (!this->enablePressureIns_)
  {
    return;
  }
//  float aggKp   = 2  , aggKi  = 0.00 , aggKd  = 0.5;
    float consKp  = 1.10  ,  consKi = 0.01, consKd = 0.05;

//  double gap = abs(this->pressureInsTarget_ - this->getPresureIns(true)); //distance away from setpoint
//  if (gap < 5)
//  { //we're close to setpoint, use conservative tuning parameters
    this->pidPressureIns_.SetTunings(consKp, consKi, consKd);
//  }
//  else
//  {
    //we're far from setpoint, use aggressive tuning parameters
//    this->pidPressureIns_.SetTunings(aggKp, aggKi, aggKd);
//  }
  this->pidPressureIns_.Compute();
//  pidPressureIns_.run();

  this->statusPressureIns_ = constrain(this->statusPressureIns_,0.0,100.0);
  this->_inputValve->open(this->statusPressureIns_);

//  Serial.println("pidPressureIns: current:" + String(this->currentPressureIns_) + " Target:" + String(this->pressureInsTarget_) + " Output:" + String(this->statusPressureIns_));
}

void ApolloHal::pidPressureExsCompute()
{
  if (!this->enablePressureExs_)
  {
    return;
  }

//  float aggKp = 5    , aggKi = 0.0 , aggKd = 0;
  float consKp = 15, consKi = 0.0, consKd = 0;

//  double gap = abs(this->pressureExsTarget_ - this->getPresureExs(true)); //distance away from setpoint
//  if (gap < 10)
//  { //we're close to setpoint, use conservative tuning parameters
    this->pidPressureExs_.SetTunings(consKp, consKi, consKd);
//  }
//  else
//  {
    //we're far from setpoint, use aggressive tuning parameters
//    this->pidPressureExs_.SetTunings(aggKp, aggKi, aggKd);
//  }

  this->pidPressureExs_.Compute();
//  pidPressureExs_.run();
 // this->pidPressureExs_.run();
  this->statusPressureExs_ =  constrain(this->statusPressureExs_,0.0,100.0);
//  this->_outputValve->open(100-this->statusPressureExs_); //OJO esto tiene que cuadrar con el update sensors!!!
  this->_outputValve->open(statusPressureExs_);

}

void ApolloHal::pidFlowInsCompute()
{
  if (!this->enableFlowIns_)
  {
    return;
  }

//  float aggKp =  5  , aggKi = 0.0, aggKd = 0;
//  float consKp = 1  , consKi = 0.0,consKd = 0;

//  double gap = abs(this->flowInsTarget_ - this->getPresureIns(true)); //distance away from setpoint
//  if (gap < 10)
//  { //we're close to setpoint, use conservative tuning parameters
//    this->pidFlowIns_.SetTunings(consKp, consKi, consKd);
//  }
//  else
//  {
//    //we're far from setpoint, use aggressive tuning parameters
//    this->pidFlowIns_.SetTunings(aggKp, aggKi, aggKd);
//  }

  this->pidFlowIns_.Compute();
  //this->pidFlowIns_.run();
  this->statusFlowIns_ = constrain(this->statusFlowIns_,0.0,100.0);
  this->_inputValve->open(this->statusFlowIns_);
  //Serial.println("pidFlowIns: current:" + String(this->currentFlowIns_) + " Target:" + String(this->flowInsTarget_) + " Output:" + String(this->statusFlowIns_));
}
*/
