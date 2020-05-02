#include "ApolloHal.h"
#include <Arduino.h>
//#define DEBUG
#include "trace.h"

ApolloHal::ApolloHal() :
    _constantPressurePID(&_constantPressurePIDInput , &_constantPressurePIDOutput , &_constantPressurePIDTarget , _constantPressurePIDKp , _constantPressurePIDKi , _constantPressurePIDKd , DIRECT),
    _constantFlowPID    (&_constantFlowPIDInput     , &_constantFlowPIDOutput     , &_constantFlowPIDTarget     , _constantFlowPIDKp     , _constantFlowPIDKi     , _constantFlowPIDKd     , DIRECT),
    _overPressurePID    (&_overPressurePIDInput     , &_overPressurePIDOutput     , &_overPressurePIDTarget     , _overPressurePIDKp     , _overPressurePIDKi     , _overPressurePIDKd     , REVERSE),
    _inspiratoryRisePID (&_inspiratoryRisePIDInput  , &_inspiratoryRisePIDOutput  , &_inspiratoryRisePIDTarget  , _inspiratoryRisePIDKp  , _inspiratoryRisePIDKi  , _inspiratoryRisePIDKd  , REVERSE)

{
  _inputPressureSensor  = NULL;
  _inputFlowSensor      = NULL;
  _outputFlowSensor     = NULL;
  _inputValve           = NULL;
  _outputValve          = NULL;
}

ApolloHal::~ApolloHal()
{
}

void ApolloHal::addValves(ApolloValve* inputValve,ApolloValve* outputValve)
{
  _inputValve  = inputValve;
  _outputValve = outputValve;
}

void ApolloHal::addPressureSensor(ApolloPressureSensor* pressureSensor)
{
  _inputPressureSensor = pressureSensor;
}

void ApolloHal::addFlowSensors(ApolloFlowSensor* inputFlow, ApolloFlowSensor* outputFlow)
{
  _inputFlowSensor  = inputFlow;
  _outputFlowSensor = outputFlow;
}

void ApolloHal::debug(String debugmsg)
{
  Serial.print("DEBUG: ");
  Serial.println(debugmsg);
  Serial.flush();
}

void ApolloHal::sendData()
{
  String pressure(getPressure());
  String intakeInstantFlow(getInputInstantFlow());
  String exitInstantFlow  (getOutputInstantFlow());

  float  in  = getInputFlow();
  float  out = getOutputFlow();
  float  vol = in + out;

  String intakeFlow(in);
  String exitFlow  (out);
  String volume    (vol);

  String intakeValveStatus(getInputValveStatus());
  String ExitValveStatus  (getOutputValveStatus());
  String intakeValveTarget(getInputValveTarget());
  String ExitValveTarget  (getOutputValveTarget());

//  String Status(ventilation->getStatus());
  String Status(-1);
  String data[] = {pressure, intakeInstantFlow, exitInstantFlow, intakeFlow, exitFlow,volume ,intakeValveStatus, ExitValveStatus,intakeValveTarget,ExitValveTarget,Status};
  Serial.print("DATA:");
  Serial.print(data[0]);
  for(int i = 1 ; i < 11 ; i++)
  {
    Serial.print(","+String(data[i]));
  }
  Serial.println();
}

/**
 * @brief Initialize and check all sensor
 *
 * @return true if all sensor are present a initilized
 * @return false if something goes wrong
 */
bool ApolloHal::begin()
{

  if(!_inputValve || !_outputValve)
  {
    debug("ERROR! NO VALVES!");
    return false;
  }

  if (!_outputValve->begin())
  {
    debug("ERROR! VALVE-OUT");
    return false;
  }

  _outputValve->open(100,true);

  if (!this->_inputValve->begin())
  {
    debug("ERROR! VALVE-IN");
    return false;
  }

  _inputValve->close(true);


  if(_inputPressureSensor)
  {
    delay(5000);
    if (!_inputPressureSensor->begin())
    {
      debug("ERROR PRESSURE SENSOR!");
      return false;
    }
    _hasPressureSensor = true;
  }
  else {_hasPressureSensor = false;}

  if(_inputFlowSensor && _outputFlowSensor)
  {
    if (!this->_inputFlowSensor->begin())
    {
      debug("ERROR FLOW-IN!");
      return false;
    }

    if (!this->_outputFlowSensor->begin())
    {
      debug("ERROR FLOW-OUT");
      return false;
    }
    _hasFlowSensors = true;
  }
  else {_hasFlowSensors = false;}

  initPIDs();
  return true;
}

bool ApolloHal::test()
{
  return true;
}

bool ApolloHal::calibrate()
{
  return true;
}

bool ApolloHal::setConstantFlow(float flow, float maxPressure)
{
  if(!_hasFlowSensors) return false;
  return false;
}

bool ApolloHal::setConstantPressure(float pressure)
{
  if(!_hasPressureSensor) return false;

  _constantPressureEnabled    = true;
  _constantPressurePIDTarget  = pressure;
  _overPressurePIDTarget      = pressure+2.5;
  return true;
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
  unsigned long startTime = micros();

  _inputValve->highFreqUpdate();
  _outputValve->highFreqUpdate();
  if(_hasPressureSensor)
  {
    _inputPressureSensor->highFreqUpdate();
  }
  if(_hasFlowSensors)
  {
    _inputFlowSensor->highFreqUpdate();
    _outputFlowSensor->highFreqUpdate();
  }

  _lastHighFreqUpdateMicros = micros() - startTime;
}

void ApolloHal::update()
{
  unsigned long startTime = micros();
  _inputValve->update();
  _outputValve->update();
  _lastInputValveStatus  = _inputValve->status();
  _lastOutputValveStatus = _outputValve->status();
  _lastInputValveTarget  = _inputValve->target();
  _lastOutputValveTarget = _outputValve->target();

  if(_hasPressureSensor)
  {
    _inputPressureSensor->update();
    _lastPressure = _inputPressureSensor->readCMH2O();

  }

  if(_hasFlowSensors)
  {
    _inputFlowSensor->update();
    _outputFlowSensor->update();
    _lastInputFlow         = _inputFlowSensor->getFlow();
    _lastOutputFlow        = _outputFlowSensor->getFlow();
    _lastInputInstantFlow  = _inputFlowSensor->getInstantFlow();
    _lastOutputInstantFlow = _outputFlowSensor->getInstantFlow();
  }

  computePIDs();
  _lastSensorLoopMicros = micros() - startTime;
}

void ApolloHal::initPIDs()
{
  _constantPressurePIDKp = 0.25 , _constantPressurePIDKi = 0.00 ,_constantPressurePIDKd = 0.00;
  _overPressurePIDKp     = 2.00 , _overPressurePIDKi     = 0.00 ,_overPressurePIDKd     = 0.00;
  _constantFlowPIDKp     = 1.00 , _constantFlowPIDKi     = 0.00 ,_constantFlowPIDKd     = 0.00;
  _inspiratoryRisePIDKp  = 2.00 , _inspiratoryRisePIDKi  = 0.00 ,_inspiratoryRisePIDKd  = 0.00;


  _constantPressurePID.SetTunings (_constantPressurePIDKp , _constantPressurePIDKi , _constantPressurePIDKd);
  _overPressurePID.SetTunings     (_overPressurePIDKp     , _overPressurePIDKi     , _overPressurePIDKd);
  _constantFlowPID.SetTunings     (_constantFlowPIDKp     , _constantFlowPIDKi     , _constantFlowPIDKd);
  _inspiratoryRisePID.SetTunings  (_inspiratoryRisePIDKp  , _inspiratoryRisePIDKi  , _inspiratoryRisePIDKd);

  _constantPressurePID.SetOutputLimits (0,100);
  _overPressurePID.SetOutputLimits     (0,100);
  _constantFlowPID.SetOutputLimits     (0,100);
  _inspiratoryRisePID.SetOutputLimits  (0,100);

  _constantPressurePID.SetSampleTime(20);
  _overPressurePID.SetSampleTime    (20);
  _constantFlowPID.SetSampleTime    (20);
  _inspiratoryRisePID.SetSampleTime (20);

  _constantPressurePID.SetMode(AUTOMATIC);
  _overPressurePID.SetMode    (AUTOMATIC);
  _constantFlowPID.SetMode    (AUTOMATIC);
  _inspiratoryRisePID.SetMode (AUTOMATIC);
}

void ApolloHal::computePIDs()
{//TODO!
  if(_constantPressureEnabled)
  {
      _constantPressurePIDInput = _lastPressure;
      _constantPressurePID.Compute();
      _constantPressurePIDOutput  = constrain(_constantPressurePIDOutput, 0, 100);
      if(_constantPressurePIDOutput != _lastInputValveTarget)
        _inputValve->open(_constantPressurePIDOutput);

      _overPressurePIDInput   = _lastPressure;
      _overPressurePID.Compute();
      _overPressurePIDOutput = constrain(_overPressurePIDOutput, 0, 100);
      if(_overPressurePIDOutput != _lastOutputValveTarget)
        _outputValve->open(0);


    //Serial.println("PID-Pressure-Input , target: "+String(_constantPressurePIDTarget)+" input: "+String(_constantPressurePIDInput)+" Output: "+String(_constantPressurePIDOutput));
    //Serial.println("PID-Pressure-Output, target: "+String(_overPressurePIDTarget)+" input: "+String(_overPressurePIDInput)+" Output: "+String(_overPressurePIDOutput));
  }
}
