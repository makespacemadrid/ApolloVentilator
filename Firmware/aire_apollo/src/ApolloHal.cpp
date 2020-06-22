#include "ApolloHal.h"
#include <Arduino.h>
#include <pidautotuner.h>
#include "globalFunctions.h"


ApolloHal::ApolloHal(ApolloStorage* storage)
{
  _storage = storage;
  _inputPressureSensor  = NULL;
  _inputFlowSensor      = NULL;
  _outputFlowSensor     = NULL;
  _inputValve           = NULL;
  _outputValve          = NULL;

  _lastSensorsUpdate         =0;
  _hwStatus                  = hardwareUNKNOWN;
  _mode                      = idleMode;
  _lastInspiratoryValveStatus = 0;
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


/**
 * @brief Initialize and check all sensor
 *
 * @return true if all sensor are present a initilized
 * @return false if something goes wrong
 */
hardwareStatus ApolloHal::begin()
{

  if(!_inputValve || !_outputValve)
  {
    _hwStatus = hardwareERROR;
    _lastError = "ERROR! NO VALVES!";
    return _hwStatus;
  }

  if (!_outputValve->begin())
  {
    _hwStatus = hardwareERROR;
    _lastError = "ERROR! VALVE-OUT";
    return _hwStatus;
  }

  _outputValve->open(100,true);

  if (!this->_inputValve->begin())
  {
    _hwStatus = hardwareERROR;
    _lastError = "ERROR! VALVE-IN";
    return _hwStatus;
  }

  _inputValve->close(true);


  if(_inputPressureSensor)
  {
    debug("wait for pressure release!");
    delay(5000);
    if (!_inputPressureSensor->begin())
    {
      _hwStatus = hardwareERROR;
      _lastError = "ERROR! PRESSSURE SENSOR!";
      return _hwStatus;
    }
    _hasPressureSensor = true;
  }
  else {_hasPressureSensor = false;}

  if(_inputFlowSensor && _outputFlowSensor)
  {
    if (!this->_inputFlowSensor->begin())
    {
      _hwStatus = hardwareERROR;
      _lastError = "ERROR! FLOW-IN!";
      return _hwStatus;
    }

    if (!this->_outputFlowSensor->begin())
    {
      _hwStatus = hardwareERROR;
      _lastError = "ERROR! FLOW-OUT!";
      return _hwStatus;
    }
    _hasFlowSensors = true;
  }
  else {_hasFlowSensors = false;}


  loadCalibration();
  initPIDs();
  _lastError = "BEGIN OK!";
  return _hwStatus;
}

bool ApolloHal::test()
{
  _hwStatus = hardwareTESTING;
//  sendVentilatorStatus();
  debug("TESTING HARDWARE");
  if(!_inputValve || !_outputValve)
  {
    _hwStatus = hardwareERROR;
    _lastError = "ERROR! NO VALVES!";
    debug("ERROR! NO VALVES!");
    return false;
  }

  _inputValve->close(true);

  if (!_outputValve->test())
  {
    _hwStatus = hardwareERROR;
    _lastError = "TEST ERROR! VALVE-OUT";
    debug("TEST ERROR! VALVE-OUT");
    _outputValve->begin();
    return false;
  }

  _outputValve->open(100,true);

  if (!this->_inputValve->test())
  {
    _hwStatus = hardwareERROR;
    _lastError = "TEST ERROR! VALVE-IN";
    debug("TEST ERROR! VALVE-IN");
    return false;
  }
  _inputValve->close(true);
  _lastError = "TEST OK!";
  if(_storage->getCalibration().calibrated)
    _hwStatus = hardwareOK;
  else
    _hwStatus = hardwareUNCAL;
  return true;
}

bool ApolloHal::calibrate()
{
  if(_hasPressureSensor && calibratePressure())
  {
    loadCalibration();
    return true;
  }
  else
    return false;
}

void ApolloHal::autotunePressurePID(float target)
{
/*
  debug("Autotune START!");
  _pressureTarget  = target;
  _overPressurePIDTarget = DEFAULT_CMH20_MAX;
  _pressureMode = none;
  PIDAutotuner tuner = PIDAutotuner();
  tuner.setTargetInputValue(_pressureTarget);
  tuner.setLoopInterval(SENSORS_INTERVAL);
  tuner.setOutputRange(0,100);
  tuner.setZNMode(PIDAutotuner::ZNModeBasicPID);
  tuner.startTuningLoop();

  while (!tuner.isFinished()) {

      // This loop must run at the same speed as the PID control loop being tuned
      unsigned long startTime = millis();

      // Get input value here (temperature, encoder position, velocity, etc)
      double input =_inputPressureSensor->readCMH2O();

      // Call tunePID() with the input value
      double output = tuner.tunePID(input);
      _constantPressurePIDKp = tuner.getKp();
      _constantPressurePIDKi = tuner.getKi();
      _constantPressurePIDKd = tuner.getKd();
      sendPIDConfig();
      // Set the output - tunePid() will return values within the range configured
      // by setOutputRange(). Don't change the value or the tuning results will be
      // incorrect.
      //double result = _lastInputValveStatus + output;
      //double inputValvePercent  = constrain(result , 0, 100);
      if(output != _lastInputValveTarget)
        _inputValve->open(output);
      // This loop must run at the same speed as the PID control loop being tuned
      while (millis() - startTime < SENSORS_INTERVAL)
      {
        update();
      }
  }
  _constantPressurePIDKp = tuner.getKp();
  _constantPressurePIDKi = 0;
  _constantPressurePIDKd = 0;
  _constantPressurePID.SetTunings(_constantPressurePIDKp , _constantPressurePIDKi , _constantPressurePIDKd);
  debug("AutoTune result: Kp:"+String(tuner.getKp())+" Ki:"+String(tuner.getKi())+" Kd:"+String(tuner.getKd()));
//  sendPIDConfig();
  ///////////// FIN AUTOCALIBRADOR
*/
}


bool ApolloHal::calibratePressure()
{
return false;
  if(!releasePressure(true,6000))
  {
    _hwStatus = hardwareUNCAL;
    _lastError = "[CAL]Too much pressure on start!";
    return false;
  }

  ApolloCalibration cal = _storage->getCalibration();
  _hwStatus = hardwareCALIBRATION;

  const float totalVolumeLow     = CALIBRATION_TUBES_VOLUME_LOW  + CALIBRATION_BALLON_VOLUME_LOW;
  const float totalVolumeHigh    = CALIBRATION_TUBES_VOLUME_HIGH + CALIBRATION_BALLON_VOLUME_HIGH;
  const float lowPressureTarget  = CALIBRATION_LOW_PRESSURE;
  const float highPressureTarget = CALIBRATION_HIGH_PRESSURE;
  const float calibrationVolume  = totalVolumeHigh-totalVolumeLow;
  _lastInspiratoryValveStatus    = 30;


  for(uint8_t c = 0 ; c < CALIBRATION_TABLE_RESOLUTION ; c ++)
  {//una iteracion para cada entrada de la tabla de calibracion
    const float    targetFlow     = map(c,0,CALIBRATION_TABLE_RESOLUTION,CALIBRATION_TABLE_MIN_VOLUME,CALIBRATION_TABLE_MAX_VOLUME);
    const uint16_t targetRiseTime = calibrationVolume/targetFlow*60.0*1000.0;
    debug("CAL: Ciclo:" + String(c)+ " Meter " + String(calibrationVolume) + "L en " + String(targetRiseTime) + "msec  - " +String(targetFlow)+"L/m" );

    //Primero encontramos el porcentaje de valvula adecuado para conseguir el tiempo.
    uint8_t errorCount = 0;
    _inspiratoryRisePID.setTarget(targetFlow);
    _inspiratoryRisePID.setOutput(_lastInspiratoryValveStatus);
    while(errorCount<20)
    {
      //cebamos el circuito a la presion inicial
      expiratoryRelease(lowPressureTarget,true);
      _mode = idleMode;
      unsigned long start = millis();
      _inputValve->open(_lastInspiratoryValveStatus);
      while(_lastPressure < CALIBRATION_HIGH_PRESSURE && getMilisecondsFrom(start) < 5000)
      {
        update();
      }
      uint16_t elapsed = getMilisecondsFrom(start);
      expiratoryRelease(lowPressureTarget,true);

      float    flow    = calibrationVolume*60.0/(elapsed/1000.0);
      float    error   = 1-(targetFlow/flow); error = abs(error);
      debug("Valvula: "+String(_lastInspiratoryValveStatus)+"  Tiempo: "+String(elapsed) + " Flujo: " + String(flow)+ " error:"+String(error));
      if(error < 0.025)
      {
        cal.flowCalibrationTable[c] = _lastInspiratoryValveStatus;
        cal.maxSupplyFlow           = flow;
        _storage->setCalibration(cal);
        debug("OK! Pos:"+String(c)+" valvePercent:"+String(_lastInspiratoryValveStatus)+" flow:"+String(flow));
        break;
      }
      else
      {
        _inspiratoryRisePID.setInput(flow);
        _inspiratoryRisePID.update();
        _lastInspiratoryValveStatus = _inspiratoryRisePID.getOutput();
        errorCount++;
      }
    }
    //Ahora hallamos el tiempo de frenada de la valvula para ese flujo
    errorCount=0;
    while(errorCount<20)
    {
      errorCount++;
    }

  }

  cal.calibrated = true;
  _storage->setCalibration(cal);
  return true;
/*
  _hwStatus = hardwareCALIBRATION;
  _inspiratoryRisePIDTarget = 1000;
  if(_lastInspiratoryValveStatus == 0) _lastInspiratoryValveStatus = 20;

  double calTarget = 30;
  double calPEEP   = 5;

  sendVentilatorStatus();
  _inputValve->close(true);
  update();
  _outputValve->open(true);
  update();
  debug("Wait for pressure release...");
  delay(3000);
  update();

  _lastPressure = _inputPressureSensor->readCMH2O();
  if(_lastPressure > 5)
  {
    _hwStatus = hardwareUNCAL;
//    sendVentilatorStatus("CAL ERROR! too much pressure on start!");
    debug("Calibration error, too much pressure on start!");
    return false;
  }


  //autotunePressurePID(calTarget);

  _inputValve->close(true);
  update();
  _outputValve->open(true);
  update();
  debug("Wait for pressure release...");
  delay(5000);

  _lastPressure = _inputPressureSensor->readCMH2O();
  _lastPressureRisePressure = _lastPressure;
  _lastPressureBrakeTime = 250;
  _pressureBraking = false;
  _pressureRising  = false;

  _lastPressureRiseSample = millis();

  for(int i = 0; i < 15 ; i++)
  {
    debug("Cycle:"+String(i));
    _pressureTarget = calTarget;
    _constantPressurePIDTarget = _pressureTarget;
    _overPressurePIDTarget = calTarget+10;
    _pressureMode = rampUpPressure;
    _pressureTargetArchived = false;
    _pressureBraking = false;
    _pressureRising  = false;
    update();
    _outputValve->close(true);
    update();
    _lastPressure = _inputPressureSensor->readCMH2O();
    _lastPressureRisePressure = _lastPressure;
    _lastInspiratoryRiseStart = millis();
    _lastPressureRiseSample   = _lastInspiratoryRiseStart;
    unsigned long deadline = _lastInspiratoryRiseStart+5000;
    _inputValve->open(_lastInspiratoryValveStatus);
    while(millis() < deadline)
    {
        update();
    }
    _pressureMode = none;
    if(!_pressureTargetArchived)
      _lastInspiratoryRiseTimeMS = 3000;
    _inspiratoryRisePIDInput = _lastInspiratoryRiseTimeMS;
    _inspiratoryRisePID.Compute();
    _lastInspiratoryValveStatus += _inspiratoryRisePIDOutput;
    _lastInspiratoryValveStatus = constrain(_lastInspiratoryValveStatus,0,100);

    _inputValve->close(true);
    _outputValve->open(100);
    _pressureMode = constantPressure;
    _pressureTarget = calPEEP;
    _constantPressurePIDTarget = _pressureTarget;
    _overPressurePIDTarget = calPEEP+5;
    deadline = millis()+3000;
    while(millis() < deadline)
    {
        update();
    }

  }
  _hwStatus = hardwareOK;
//  sendVentilatorStatus();
  return true;
*/
}

bool ApolloHal::loadCalibration()
{
  _constantPressurePID.setConstants(_storage->getCalibration().constantPressurePID);
  _constantFlowPID.setConstants(_storage->getCalibration().constantFlowPID);
  _overPressurePID.setConstants(_storage->getCalibration().overPressurePID);
  _inspiratoryRisePID.setConstants(_storage->getCalibration().inspiratoryRisePID);
  _expiratoryPID.setConstants(_storage->getCalibration().expiratoryPID);

  if(_storage->getCalibration().calibrated)
  {
    _hwStatus = hardwareOK;
    return true;
  }
  else
  {
    _hwStatus = hardwareUNCAL;
    return false;
  }
}


////////
bool  ApolloHal::inspiratoryRisePressure(float pressure, uint16_t targetTime)
{
  _mode = inspiratoryRisePressureMode;
  return true;
}

bool  ApolloHal::inspiratoryRiseFlow(float flow, uint16_t targetTime, float maxPressure)
{
  _mode = inspiratoryRiseFlowMode;
  return true;
}

bool  ApolloHal::expiratoryRelease(float peep,bool wait,uint16_t timeout)
{
  _mode = expiratoryReleaseMode;
  _inputValve->close();
  _outputValve->open();
  if(!wait)return true;


  return true;
}

bool  ApolloHal::releasePressure(bool wait,uint16_t timeout)
{
  _mode = pressureReleaseMode;
  _inputValve->close();
  _outputValve->open();
  if(!wait) return true;
  unsigned long start = millis();
  while( wait && (_mode != idleMode) && (getMilisecondsFrom(start) < timeout) )
      update();
  return _mode != pressureReleaseMode;
}



////////Gestion de los modos de operacion:
void ApolloHal::handleIdleMode()
{

}
void ApolloHal::handleInpiratoryRiseSpontaneousMode()
{

}

void ApolloHal::handleInspiratoryRisePressureMode()
{

}

void ApolloHal::handleInspiratoryRiseFlowMode()
{

}

void ApolloHal::handleExpiratoryReleaseMode()
{

}

void ApolloHal::handleExpiratoryPEEPMode()
{

}

void ApolloHal::handlePressureReleaseMode()
{
  if(_lastPressure < 0.5)
    _mode = idleMode;
}
////////////
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
  _hfLoops++;
  _avgHighFreqUpdateMicros = ( (_avgHighFreqUpdateMicros*5) + getMicrosecondsFrom(startTime)) /6;
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

  _sensorLoops++;
  _avgSensorLoopMicros = ( (_avgSensorLoopMicros*5) + getMicrosecondsFrom(startTime)) /6;
}

void ApolloHal::updateControl()
{
  switch(_mode)
  {
      case idleMode                       : handleIdleMode()                      ;break;
      case inpiratoryRiseSpontaneousMode  : handleInpiratoryRiseSpontaneousMode() ;break;
      case inspiratoryRisePressureMode    : handleInspiratoryRisePressureMode()   ;break;
      case inspiratoryRiseFlowMode        : handleInspiratoryRiseFlowMode()       ;break;
      case expiratoryReleaseMode          : handleExpiratoryReleaseMode()         ;break;
      case expiratoryPEEPMode             : handleExpiratoryPEEPMode()            ;break;
      case pressureReleaseMode            : handlePressureReleaseMode()           ;break;
  }
  checkOverPressure();
/*
if(_pressureMode == rampUpPressure || _pressureRising)
{
//      debug("ramp,pt:"+String(_pressureTarget) +"lp: "+String(_lastPressureRisePressure)+" p:"+String(_lastPressure));
  unsigned long now = millis();
  // m = y2-y1 / x2-x1
  float pdiff = _lastPressure - _lastPressureRisePressure;
  float tdiff = now - _lastPressureRiseSample;
  float slope = pdiff / tdiff;
  _lastPressureRisePressure = _lastPressure;
  _lastPressureRiseSample = now;

  if(!_pressureRising)
  {
      if(slope > 0.05) _pressureRising = true;
  }
  else
  {
    if(!_pressureBraking)
    {
      // ca = co / m
      float timeToTarget = float(_pressureTarget - _lastPressure) / slope;
//          debug("ramp, pdiff:" +String(pdiff)+ " tdiff:"+ String(tdiff) + " slope:"+String(slope)+" timeto:"+String(timeToTarget));
      if(timeToTarget < _lastPressureBrakeTime)
      {
//            debug("BRAKING!");
        now = millis();
        _lastPressureBrakeStart = now;
        _pressureBraking = true;
        _inputValve->close();
      }
    }
    else if(_pressureBraking && slope < 0.025)
    {
      _lastPressureBrakeTime = millis() - _lastPressureBrakeStart;
      debug("Brake time:"+String(_lastPressureBrakeTime));
      _pressureMode = none;
      _pressureBraking = false;
      _pressureRising  = false;
    }
  }

  if(!_pressureTargetArchived && _lastPressure >= _pressureTarget*0.95)
  {
        _pressureTargetArchived = true;
        _lastInspiratoryRiseTimeMS = millis() - _lastInspiratoryRiseStart;
        debug("Pressure Achieved! - "+String(_lastInspiratoryRiseTimeMS));
  }

//      if(_lastPressure > _pressureTarget * 0.85)
//      {
//        _pressureMode = constantPressure;
//        _constantPressurePIDTarget = _pressureTarget;
//        _constantPressurePIDOutput = _lastInputValveStatus;
//      }

}

*/
}


void ApolloHal::update()
{
  highFrecuencyUpdate();

  if(getMilisecondsFrom(_lastSensorsUpdate) >= SENSORS_INTERVAL)
  {
    _lastSensorsUpdate = millis();
    sensorUpdate();
    updateControl();
  }
}


void ApolloHal::initPIDs()
{
  _overPressurePID.setTarget(DEFAULT_CMH20_MAX);
//

//float minLimit = 0,float maxLimit = 100,bool direct = true,float minOutput = -20,float maxOutput=20
  _constantPressurePID.begin(0,100);
  _constantFlowPID.begin(0,100);
  _overPressurePID.begin(0,100);
  _inspiratoryRisePID.begin(0,100,false,-10,+10);
  _expiratoryPID.begin(0,100);

//  sendPIDConfig();
}

bool ApolloHal::checkOverPressure()
{
  if( (_overPressureTriggered) || (_lastPressure > _overPressurePID.getTarget()) )
  {
    _overPressureTriggered = true;
    _overPressurePID.setInput(_lastPressure);
    _overPressurePID.update();
    if(_outputValve->target() != _overPressurePID.getOutput())
      _outputValve->open(_overPressurePID.getOutput());
    if(_outputValve->target() == 0)
      _overPressureTriggered = false;
  }
  return _overPressureTriggered;
}
