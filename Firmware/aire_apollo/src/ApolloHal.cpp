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

  _lastSensorsUpdate         =0;
  _lastTelemetryUpdate       =0;
  _lastCommunicationsUpdate  =0;
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
  debug("TESTING HARDWARE");
  if(!_inputValve || !_outputValve)
  {
    debug("ERROR! NO VALVES!");
    return false;
  }

  _inputValve->close(true);

  if (!_outputValve->test())
  {
    debug("TEST ERROR! VALVE-OUT");
    _outputValve->begin();
    return false;
  }

  _outputValve->open(100,true);

  if (!this->_inputValve->test())
  {
    _inputValve->begin();
    debug("TEST ERROR! VALVE-IN");
    return false;
  }
  _inputValve->close(true);
  return true;
}

bool ApolloHal::calibrate()
{

  if(_hasPressureSensor)
    return calibratePressure();
  else
    return true;
}

bool ApolloHal::calibratePressure()
{
  _inputValve->close(true);
  _outputValve->close(true);
  delay(1000);
  _lastPressure = _inputPressureSensor->readCMH2O();
  if(_lastPressure > 5)
  {
    debug("Calibration error, too much pressure on start!");
    return false;
  }

  _lastInspiratoryValveStatus = 15;
  _pressureTarget  = 30;
  _inspiratoryRisePIDTarget = 1500;

  for(int i = 0; i < 10 ; i++)
  {
    update();
    _pressureTargetArchived = false;
    _pressureMode = rampUpPressure;
    _outputValve->close(true);
    update();
    _lastInspiratoryRiseStart = millis();
    unsigned long deadline = _lastInspiratoryRiseStart+5000;
    _inputValve->open(_lastInspiratoryValveStatus);
    while(millis() < deadline)
    {
        update();
    }
    _pressureMode = none;
    if(!_pressureTargetArchived)
      _lastInspiratoryRiseTimeMS = 5000;
    _inspiratoryRisePIDInput = _lastInspiratoryRiseTimeMS;
    _inspiratoryRisePID.Compute();
    _lastInspiratoryValveStatus += _inspiratoryRisePIDOutput;
    _lastInspiratoryValveStatus = constrain(_lastInspiratoryValveStatus,0,100);
    deadline = millis()+5000;

    _inputValve->close();
    _outputValve->open(100);
    while(millis() < deadline)
    {
        update();
    }

  }

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
  _pressureMode = constantPressure;

  _constantPressurePIDTarget  = pressure;
  _overPressurePIDTarget      = pressure+10;
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

void ApolloHal::sensorUpdate()
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

  _lastSensorLoopMicros = micros() - startTime;
}

void ApolloHal::update()
{
  highFrecuencyUpdate();
  unsigned long now = millis();

  if(now >= _lastTelemetryUpdate + TELEMETRY_INTERVAL)
  {
    _lastTelemetryUpdate = now;
    sendData();
  }

  if(now >= _lastSensorsUpdate + SENSORS_INTERVAL)
  {
    _lastSensorsUpdate = now;
    sensorUpdate();
    if(_pressureMode == rampUpPressure)
    {
      if(_lastPressure > _pressureTarget * 0.75)
      {
        _pressureMode = constantPressure;
        _constantPressurePIDTarget = _pressureTarget;
      }
    }

    if(!_pressureTargetArchived && _pressureMode == constantPressure)
    {
        if(_lastPressure >= _pressureTarget*0.98)
        {
          _pressureTargetArchived = true;
          _lastInspiratoryRiseTimeMS = millis() - _lastInspiratoryRiseStart;
          debug("Pressure Achieved! - "+String(_lastInspiratoryRiseTimeMS));
        }
    }

    computePIDs();
  }

  if(now >= _lastCommunicationsUpdate + COMMUNICATIONS_INTERVAL)
  {
    _lastCommunicationsUpdate = now;
//    com->serialRead();
  }
}

void ApolloHal::initPIDs()
{
  _overPressurePIDTarget = DEFAULT_CMH20_MAX;
  _constantPressurePIDKp = 2.5  ,  _constantPressurePIDKi = 1.10 ,_constantPressurePIDKd = 0.10;
  _overPressurePIDKp     = 5.00  , _overPressurePIDKi     = 0.00 ,_overPressurePIDKd     = 0.00;
  _constantFlowPIDKp     = 1.00  , _constantFlowPIDKi     = 0.00 ,_constantFlowPIDKd     = 0.00;
  _inspiratoryRisePIDKp  = 0.01  , _inspiratoryRisePIDKi  = 0.00 ,_inspiratoryRisePIDKd  = 0.00;


  _constantPressurePID.SetTunings (_constantPressurePIDKp , _constantPressurePIDKi , _constantPressurePIDKd);
  _overPressurePID.SetTunings     (_overPressurePIDKp     , _overPressurePIDKi     , _overPressurePIDKd);
  _constantFlowPID.SetTunings     (_constantFlowPIDKp     , _constantFlowPIDKi     , _constantFlowPIDKd);
  _inspiratoryRisePID.SetTunings  (_inspiratoryRisePIDKp  , _inspiratoryRisePIDKi  , _inspiratoryRisePIDKd);

  _constantPressurePID.SetOutputLimits (-25,25);
  _overPressurePID.SetOutputLimits     (-50,50);
  _constantFlowPID.SetOutputLimits     (0,100);
  _inspiratoryRisePID.SetOutputLimits  (-15,15);

  _constantPressurePID.SetSampleTime(50);
  _overPressurePID.SetSampleTime    (50);
  _constantFlowPID.SetSampleTime    (50);
  //_inspiratoryRisePID.SetSampleTime (50);

  _constantPressurePID.SetMode(AUTOMATIC);
  _overPressurePID.SetMode    (AUTOMATIC);
  _constantFlowPID.SetMode    (AUTOMATIC);
  _inspiratoryRisePID.SetMode (AUTOMATIC);
}

void ApolloHal::computePIDs()
{//TODO!
  if(_pressureMode == constantPressure)
  {
      double result;
      _constantPressurePIDInput = _lastPressure;
      _constantPressurePID.Compute();
      result = _lastInputValveStatus + _constantPressurePIDOutput;
      double inputValvePercent  = constrain(result , 0, 100);
      if(inputValvePercent != _lastInputValveTarget)
        _inputValve->open(inputValvePercent);

    //Serial.println("PID-Pressure-Input , target: "+String(_constantPressurePIDTarget)+" input: "+String(_constantPressurePIDInput)+" Output: "+String(_constantPressurePIDOutput));
    //Serial.println("PID-Pressure-Output, target: "+String(_overPressurePIDTarget)+" input: "+String(_overPressurePIDInput)+" Output: "+String(_overPressurePIDOutput));
  }

  if(_lastPressure > _overPressurePIDTarget)
  {
    double result;
    _overPressurePIDInput   = _lastPressure;
    _overPressurePID.Compute();
    result = _lastOutputValveStatus + _overPressurePIDOutput;
    double outputValvePercent  = constrain(result , 0, 100);
    if(outputValvePercent != _lastOutputValveTarget)
      _outputValve->open(outputValvePercent);
  }
}
