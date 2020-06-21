#ifndef APOLLOCOMMS_H
#define APOLLOCOMMS_H

#include "Arduino.h"
#include "defaults.h"
#include "strings.h"
#include "ApolloStorage.h"
#include <ArduinoJson.h>

class ApolloProtocol
{
public:
  static void sendMedicalData(float pressure, float inputFlow = 0,float outputFlow = 0, float instantInputFlow = 0, float instantOutputFlow = 0)
  {// VTE RPM pPeak pPlat
    StaticJsonDocument<MAX_JSON_SIZE> jsonOutput;
    jsonOutput[STR_JSON_TYPE] = STR_MEDICAL_DATA;
    jsonOutput[STR_PRESSURE] = pressure;
    //jsonOutput[STR_TARGET_PRESSURE] = _hal->getPressureTarget();
    float  in  = inputFlow;
    float  out = outputFlow;
    float  vol = in + out;

    jsonOutput[STR_INPUT_FLOW]  = in;
    jsonOutput[STR_OUTPUT_FLOW] = out;
    jsonOutput[STR_INPUT_FLOW_INSTANT]  = instantInputFlow;
    jsonOutput[STR_OUTPUT_FLOW_INSTANT] = instantOutputFlow;
    jsonOutput[STR_VOLUME]     = vol;
    serializeJson(jsonOutput, Serial);
    Serial.println();
  }

  static void sendHWInfo(float pressure,float targetPressure,float instantInputFlow,float instantOutputFlow,float inputStatus, float inputTarget = 0, float outputStatus = 0, float outputTarget = 0)
  {
    StaticJsonDocument<MAX_JSON_SIZE> jsonOutput;
    jsonOutput[STR_JSON_TYPE] = STR_HARDWARE_INFO;

    jsonOutput[STR_PRESSURE]        = pressure;
    jsonOutput[STR_TARGET_PRESSURE] = targetPressure;

    jsonOutput[STR_INPUT_FLOW_INSTANT]  = instantInputFlow;
    jsonOutput[STR_OUTPUT_FLOW_INSTANT] = instantOutputFlow;

    jsonOutput[STR_INPUT_STATUS] = inputStatus;
    jsonOutput[STR_INPUT_TARGET] = inputTarget;

    jsonOutput[STR_OUTPUT_STATUS] = outputStatus;
    jsonOutput[STR_OUTPUT_TARGET] = outputTarget;

    serializeJson(jsonOutput, Serial);
    Serial.println();
  }

  static void sendHWMetrics(float avgSensor, float avgHF, float avfT, uint16_t sLoops, uint16_t hfLoops, uint16_t tLoops)
  {
    StaticJsonDocument<MAX_JSON_SIZE> jsonOutput;
    jsonOutput[STR_JSON_TYPE] = STR_HARDWARE_INFO;

    jsonOutput[STR_AVG_SENSOR_LOOP_MICROS] = avgSensor;
    jsonOutput[STR_AVG_HF_LOOP_MICROS]     = avgHF;
    jsonOutput[STR_AVG_TELEMETRY_MICROS]   = avfT;

    jsonOutput[STR_SENSOR_LOOPS]    = sLoops;
    jsonOutput[STR_HF_LOOPS]        = hfLoops;
    jsonOutput[STR_TELEMETRY_LOOPS] = tLoops;

//    _hal->resetLoopCounters();
//    _telemetryLoops = 0;

    serializeJson(jsonOutput, Serial);
    Serial.println();
//    _lastMetricsUpdate = millis();
  }

  static void sendPIDConfig(pidConstants constantPressurePID, pidConstants constantFlowPID,pidConstants overPressurePID,pidConstants inspiratoryRisePID, pidConstants expiratoryPID)
  {

    StaticJsonDocument<MAX_JSON_SIZE> jsonOutput;
    jsonOutput[STR_JSON_TYPE] = STR_HARDWARE_INFO;

    jsonOutput[STR_PRESSURE_PID_KP] = constantPressurePID.kp;
    jsonOutput[STR_PRESSURE_PID_KI] = constantPressurePID.ki;
    jsonOutput[STR_PRESSURE_PID_KD] = constantPressurePID.kd;
    serializeJson(jsonOutput, Serial);
    Serial.println();
  }

  static void sendVentilatorStatus(hardwareStatus hw, ventilatorStatus v, String errorMSG = "none")
  {
    StaticJsonDocument<MAX_JSON_SIZE> jsonOutput;
    jsonOutput[STR_JSON_TYPE] = STR_VENT_STATUS;

    String hwS    = "NULL";
    String ventS  = "NULL";

    switch(hw)
    {
        case hardwareUNKNOWN      : hwS = STR_UNKNOWN     ;break;
        case hardwareOK           : hwS = STR_OK          ;break;
        case hardwareUNCAL        : hwS = STR_UNCAL       ;break;
        case hardwareERROR        : hwS = STR_ERROR       ;break;
        case hardwareTESTING      : hwS = STR_TESTING     ;break;
        case hardwareCALIBRATION  : hwS = STR_CALIBRATION ;break;
    }

    switch (v)
    {
      case ventilatorStopped : ventS = STR_STOPPED ;break;
      case ventilatorRunning : ventS = STR_RUNNING ;break;
      case ventilatorPaused  : ventS = STR_PAUSED  ;break;
      case ventilatorError   : ventS = STR_ERROR   ;break;
    }

    jsonOutput[STR_HARDWARE_STATUS] = hwS;
    jsonOutput[STR_VENT_STATUS]     = ventS;
    jsonOutput[STR_ERROR_MSG]       = errorMSG;

    serializeJson(jsonOutput, Serial);
    Serial.println();
  }

  static void sendConfig(ApolloConfiguration conf)
  {//TODO
    //_hal->debug("SEND CONFIG no implementado todavia!!!!");
  }

  static void sendCalibration(ApolloCalibration cal)
  {//TODO
    //_hal->debug("SEND CAL no implementado todavia!!!!");
  }

  static ApolloConfiguration parseConfig(StaticJsonDocument<MAX_JSON_SIZE>* json)
  {
    ApolloConfiguration result;

    return result;
  }

  static ApolloCalibration parseCalibration(StaticJsonDocument<MAX_JSON_SIZE>* json)
  {
    ApolloCalibration result;

    return result;
  }

};

#endif
