#include "ApolloHal.h"
#include <Arduino.h>
#include <pidautotuner.h>


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
  _hwStatus = hardwareUNKNOWN;
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

void ApolloHal::sendData()
{
  unsigned long start = micros();
  sendMedicalData();
  sendValveStatus();
  uint16_t elapsed = micros() - start;
  _avgTelemetryUpdateMicros = ((_avgTelemetryUpdateMicros*5) + elapsed ) / 6.0;
  _telemetryLoops++;
}

void ApolloHal::sendMedicalData()
{
  StaticJsonDocument<MAX_JSON_SIZE> jsonOutput;
  jsonOutput[STR_JSON_TYPE] = STR_MEDICAL_DATA;

  if(_hasPressureSensor)
  {
    jsonOutput[STR_PRESSURE] = getPressure();
    jsonOutput[STR_TARGET_PRESSURE] = _pressureTarget;
  }

  if(_hasFlowSensors)
  {
    float  in  = getInputFlow();
    float  out = getOutputFlow();
    float  vol = in + out;

    jsonOutput[STR_INPUT_FLOW]  = in;
    jsonOutput[STR_OUTPUT_FLOW] = out;
    jsonOutput[STR_INPUT_FLOW_INSTANT]  = getInputInstantFlow();
    jsonOutput[STR_OUTPUT_FLOW_INSTANT] = getOutputFlow();
    jsonOutput[STR_VOLUME]     = vol;
  }

  serializeJson(jsonOutput, Serial);
  Serial.println();
}

void ApolloHal::sendValveStatus()
{
  StaticJsonDocument<MAX_JSON_SIZE> jsonOutput;
  jsonOutput[STR_JSON_TYPE] = STR_HARDWARE_INFO;

  jsonOutput[STR_INPUT_STATUS] = getInputValveStatus();
  jsonOutput[STR_INPUT_TARGET] = getInputValveTarget();

  jsonOutput[STR_OUTPUT_STATUS] = getOutputValveStatus();
  jsonOutput[STR_OUTPUT_TARGET] = getOutputValveTarget();

  serializeJson(jsonOutput, Serial);
  Serial.println();
}

void ApolloHal::sendVentilatorStatus(String errorMSG)
{
  StaticJsonDocument<MAX_JSON_SIZE> jsonOutput;
  jsonOutput[STR_JSON_TYPE] = STR_VENT_STATUS;


  enum hardwareStatus
  {
    hardwareUNKNOWN,
    hardwareOK,
    hardwareUNCAL,
    hardwareERROR,
    hardwareTESTING,
    hardwareCALIBRATION
  };

  enum ventilatorStatus
  {
    ventilatorSTOPPED,
    ventilatorRUNNING,
    ventilatorPAUSED,
    ventilatorERROR
  };

  String hwS    = "NULL";
  String ventS  = STR_ERROR; // TODO!

  switch(_hwStatus)
  {
      case hardwareUNKNOWN      : hwS = STR_UNKNOWN;    break;
      case hardwareOK           : hwS = STR_OK;         break;
      case hardwareUNCAL        : hwS = STR_UNCAL;      break;
      case hardwareERROR        : hwS = STR_ERROR;      break;
      case hardwareTESTING      : hwS = STR_TESTING;    break;
      case hardwareCALIBRATION  : hwS = STR_CALIBRATION;break;
  }

  jsonOutput[STR_HARDWARE_STATUS] = hwS;
  jsonOutput[STR_VENT_STATUS]     = ventS;
  jsonOutput[STR_ERROR_MSG]       = errorMSG;

  serializeJson(jsonOutput, Serial);
  Serial.println();
}

void ApolloHal::sendPIDConfig()
{
  StaticJsonDocument<MAX_JSON_SIZE> jsonOutput;
  jsonOutput[STR_JSON_TYPE] = STR_HARDWARE_INFO;

  jsonOutput[STR_PRESSURE_PID_KP] = _constantPressurePIDKp;
  jsonOutput[STR_PRESSURE_PID_KI] = _constantPressurePIDKi;
  jsonOutput[STR_PRESSURE_PID_KD] = _constantPressurePIDKd;
  serializeJson(jsonOutput, Serial);
  Serial.println();
}

void ApolloHal::sendMetrics()
{
  StaticJsonDocument<MAX_JSON_SIZE> jsonOutput;
  jsonOutput[STR_JSON_TYPE] = STR_HARDWARE_INFO;

  jsonOutput[STR_AVG_SENSOR_LOOP_MICROS] = _avgSensorLoopMicros;
  jsonOutput[STR_AVG_HF_LOOP_MICROS]     = _avgHighFreqUpdateMicros;
  jsonOutput[STR_AVG_TELEMETRY_MICROS]   = _avgTelemetryUpdateMicros;

  jsonOutput[STR_SENSOR_LOOPS]    = _sensorLoops;
  jsonOutput[STR_HF_LOOPS]        = _hfLoops;
  jsonOutput[STR_TELEMETRY_LOOPS] = _telemetryLoops;

  _sensorLoops = 0;
  _hfLoops     = 0;
  _telemetryLoops = 0;

  serializeJson(jsonOutput, Serial);
  Serial.println();
  _lastMetricsUpdate = millis();
}

void ApolloHal::sendConfig()
{//TODO
  debug("SEND CONFIG no implementado todavia!!!!");
}

void ApolloHal::readSerial()
{
  if(!Serial.available()) return;
  String payload = Serial.readStringUntil('\n');
  StaticJsonDocument<MAX_JSON_SIZE> jsonInput;
  DeserializationError error = deserializeJson(jsonInput, payload);
  if (error) {
    debug("deserializeJson() failed: " + String(error.c_str()));
    debug(payload);
    return;
  }

  String type = jsonInput[STR_JSON_TYPE];
  if(!type)
  {
    debug("JSON with no type!");
    return;
  }
  if(type == STR_COMMAND)
  {
    String cmd = jsonInput[STR_COMMAND];
    if(cmd == STR_COMMAND_START)
    {
      debug("START no implementado");
    }
    else if(cmd == STR_COMMAND_STOP)
    {
      debug("STOP no implementado");
    }
    else if(cmd == STR_COMMAND_PAUSE)
    {
      debug("PAUSE no implementado");
    }
    else if(cmd == STR_COMMAND_TEST)
    {
      test();
    }
    else if(cmd == STR_COMMAND_CALIBRATE)
    {
      calibrate();
    }
    else if(cmd == STR_COMMAND_GET_CONFIG)
    {
      sendConfig();
    }
  }

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
    _hwStatus = hardwareERROR;
    sendVentilatorStatus("ERROR! NO VALVES!");
    debug("ERROR! NO VALVES!");
    return false;
  }

  if (!_outputValve->begin())
  {
    _hwStatus = hardwareERROR;
    sendVentilatorStatus("ERROR! VALVE-OUT");
    debug("ERROR! VALVE-OUT");
    return false;
  }

  _outputValve->open(100,true);

  if (!this->_inputValve->begin())
  {
    _hwStatus = hardwareERROR;
    sendVentilatorStatus("ERROR! VALVE-IN");
    debug("ERROR! VALVE-IN");
    return false;
  }

  _inputValve->close(true);


  if(_inputPressureSensor)
  {
    debug("wait for pressure release!");
    delay(5000);
    if (!_inputPressureSensor->begin())
    {
      _hwStatus = hardwareERROR;
      sendVentilatorStatus("ERROR! PRESSSURE SENSOR!");
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
      _hwStatus = hardwareERROR;
      sendVentilatorStatus("ERROR! FLOW-IN!");
      debug("ERROR FLOW-IN!");
      return false;
    }

    if (!this->_outputFlowSensor->begin())
    {
      _hwStatus = hardwareERROR;
      sendVentilatorStatus("ERROR! FLOW-OUT!");
      debug("ERROR FLOW-OUT");
      return false;
    }
    _hasFlowSensors = true;
  }
  else {_hasFlowSensors = false;}

  initPIDs();

  sendPIDConfig();

  if(readCalibrationData())
  {
    _hwStatus = hardwareOK;
    sendVentilatorStatus();
  }
  else
  {
    _hwStatus = hardwareUNCAL;
    sendVentilatorStatus();
  }
  return true;
}

bool ApolloHal::test()
{
  _hwStatus = hardwareTESTING;
  sendVentilatorStatus();
  debug("TESTING HARDWARE");
  if(!_inputValve || !_outputValve)
  {
    _hwStatus = hardwareERROR;
    sendVentilatorStatus("ERROR! NO VALVES!");
    debug("ERROR! NO VALVES!");
    return false;
  }

  _inputValve->close(true);

  if (!_outputValve->test())
  {
    _hwStatus = hardwareERROR;
    sendVentilatorStatus("TEST ERROR! VALVE-OUT");
    debug("TEST ERROR! VALVE-OUT");
    _outputValve->begin();
    return false;
  }

  _outputValve->open(100,true);

  if (!this->_inputValve->test())
  {
    _hwStatus = hardwareERROR;
    sendVentilatorStatus("TEST ERROR! VALVE-IN");
    debug("TEST ERROR! VALVE-IN");
    return false;
  }
  _inputValve->close(true);

  if(readCalibrationData())
  {
    _hwStatus = hardwareOK;
    sendVentilatorStatus();
  }
  else
  {
    _hwStatus = hardwareUNCAL;
    sendVentilatorStatus();
  }
  return true;
}

bool ApolloHal::calibrate()
{
  if(_hasPressureSensor)
    return calibratePressure();
  else
    return true;
}

void ApolloHal::autotunePressurePID(float target)
{
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
  sendPIDConfig();
  ///////////// FIN AUTOCALIBRADOR
}


bool ApolloHal::calibratePressure()
{
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
    sendVentilatorStatus("CAL ERROR! too much pressure on start!");
    debug("Calibration error, too much pressure on start!");
    return false;
  }


  autotunePressurePID(calTarget);

  _inputValve->close(true);
  update();
  _outputValve->open(true);
  update();
  debug("Wait for pressure release...");
  delay(5000);

  for(int i = 0; i < 10 ; i++)
  {
    _pressureTarget = calTarget;
    _constantPressurePIDTarget = _pressureTarget;
    _overPressurePIDTarget = calTarget+10;
    _pressureTargetArchived = false;
    _pressureMode = rampUpPressure;
    update();
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
    deadline = millis()+5000;
    while(millis() < deadline)
    {
        update();
    }

  }
  _hwStatus = hardwareOK;
  sendVentilatorStatus("CAL ERROR! too much pressure on start!");
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

  _pressureTarget = pressure;
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
  _hfLoops++;
  _avgHighFreqUpdateMicros = ( (_avgHighFreqUpdateMicros*5) + (micros() - startTime)) /6;
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
  _avgSensorLoopMicros = ( (_avgSensorLoopMicros*5) + (micros() - startTime)) /6;
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

  if(now >= _lastMetricsUpdate + 1000)
  {
    sendMetrics();
  }

  if(now >= _lastSensorsUpdate + SENSORS_INTERVAL)
  {
    _lastSensorsUpdate = now;
    sensorUpdate();
    if(_pressureMode == rampUpPressure)
    {
      if(_lastPressure > _pressureTarget * 0.85)
      {
        _pressureMode = constantPressure;
        _constantPressurePIDTarget = _pressureTarget;
        _constantPressurePIDOutput = _lastInputValveStatus;
      }
    }

    if(!_pressureTargetArchived && _pressureMode == constantPressure)
    {
        if(_lastPressure >= _pressureTarget*0.90)
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
    readSerial();
  }
}

void ApolloHal::initPIDs()
{
  _overPressurePIDTarget = DEFAULT_CMH20_MAX;
  _constantPressurePIDKp = 6     , _constantPressurePIDKi = 0.1 ,_constantPressurePIDKd = 0.1;
  _overPressurePIDKp     = 5     , _overPressurePIDKi     = 0.00 ,_overPressurePIDKd     = 0.00;
  _constantFlowPIDKp     = 1.00  , _constantFlowPIDKi     = 0.00 ,_constantFlowPIDKd     = 0.00;
  _inspiratoryRisePIDKp  = 0.01  , _inspiratoryRisePIDKi  = 0.00 ,_inspiratoryRisePIDKd  = 0.00;


  _constantPressurePID.SetTunings (_constantPressurePIDKp , _constantPressurePIDKi , _constantPressurePIDKd);
  _overPressurePID.SetTunings     (_overPressurePIDKp     , _overPressurePIDKi     , _overPressurePIDKd);
  _constantFlowPID.SetTunings     (_constantFlowPIDKp     , _constantFlowPIDKi     , _constantFlowPIDKd);
  _inspiratoryRisePID.SetTunings  (_inspiratoryRisePIDKp  , _inspiratoryRisePIDKi  , _inspiratoryRisePIDKd);

  _constantPressurePID.SetOutputLimits (0,100);
  _overPressurePID.SetOutputLimits     (-15,15);
  _constantFlowPID.SetOutputLimits     (0,100);
  _inspiratoryRisePID.SetOutputLimits  (-15,15);

  //_constantPressurePID.SetSampleTime(50);
  //_overPressurePID.SetSampleTime    (50);
  //_constantFlowPID.SetSampleTime    (50);
  //_inspiratoryRisePID.SetSampleTime (50);

  _constantPressurePID.SetMode(AUTOMATIC);
  _overPressurePID.SetMode    (AUTOMATIC);
  _constantFlowPID.SetMode    (AUTOMATIC);
  _inspiratoryRisePID.SetMode (AUTOMATIC);
  sendPIDConfig();
}

void ApolloHal::computePIDs()
{//TODO!
  if(_pressureMode == constantPressure)
  {
      double result;
      _constantPressurePIDInput = _lastPressure;
      _constantPressurePID.Compute();
      //result = _lastInputValveStatus + _constantPressurePIDOutput;
      //double inputValvePercent  = constrain(result , 0, 100);
      if(_constantPressurePIDOutput != _lastInputValveTarget)
        _inputValve->open(_constantPressurePIDOutput);

    //Serial.println("PID-Pressure-Input , target: "+String(_constantPressurePIDTarget)+" input: "+String(_constantPressurePIDInput)+" Output: "+String(_constantPressurePIDOutput));
    //Serial.println("PID-Pressure-Output, target: "+String(_overPressurePIDTarget)+" input: "+String(_overPressurePIDInput)+" Output: "+String(_overPressurePIDOutput));
  }

  if( (_overPressureTriggered) || (_lastPressure > _overPressurePIDTarget) )
  {
    _overPressureTriggered = true;
    double result;
    _overPressurePIDInput   = _lastPressure;
    _overPressurePID.Compute();
    result = _lastOutputValveStatus + _overPressurePIDOutput;
    double outputValvePercent  = constrain(result , 0, 100);
    if(outputValvePercent != _lastOutputValveTarget)
      _outputValve->open(outputValvePercent);
    if(outputValvePercent == 0)
      _overPressureTriggered = false;
  }
}
