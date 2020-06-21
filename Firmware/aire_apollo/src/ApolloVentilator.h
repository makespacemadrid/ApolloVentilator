#ifndef VENTILATOR_H
#define VENTILATOR_H

#include "Arduino.h"
#include "ApolloStorage.h"
#include "ApolloHal.h"
#include <ArduinoJson.h>
#include "ApolloProtocol.h"
#include "globalFunctions.h"




class ApolloVentilator
{
public:
  ApolloVentilator(ApolloHal* hal,ApolloStorage* storage) :
  _hal(hal),_storage(storage)
  {
    _lastTelemetryUpdate       =0;
    _lastCommunicationsUpdate  =0;
  }

  bool begin()
  {
    if(_hal->begin() != hardwareOK || hardwareUNCAL)
    {
      ApolloProtocol::sendVentilatorStatus(_hal->getHWstatus(),_ventilatorStatus,  _hal->getLastErrorString());
      return false;
    }
    else
    {
      ApolloProtocol::sendVentilatorStatus(_hal->getHWstatus(),_ventilatorStatus, _hal->getLastErrorString());
      return true;
    }
  }

  void update()
  {
    _hal->update();
    unsigned long now = millis();

    if(getMilisecondsFrom(_lastVentilationUpdate) >= VENTILATOR_INTERVAL)
    {
      _lastVentilationUpdate = now;
      ventilationUpdate();
      return;
    }

    if(getMilisecondsFrom(_lastTelemetryUpdate) >= TELEMETRY_INTERVAL)
    {
      _lastTelemetryUpdate = now;
      sendTelemetry();
      return;
    }

    if(getMilisecondsFrom(_lastMetricsUpdate) >= 1000)
    {
      _lastMetricsUpdate = now;
      ApolloProtocol::sendHWMetrics(_hal->getAvgSensorLoopMicros(),_hal->getAvgHfLoopMicros(),_avgTelemetryUpdateMicros,_hal->getSensorLoops(),_hal->getHfLoops(),_telemetryLoops);
      _hal->resetLoopCounters();
      _telemetryLoops=0;
      return;
    }

    if(getMilisecondsFrom(_lastCommunicationsUpdate) >= COMMUNICATIONS_INTERVAL)
    {
      _lastCommunicationsUpdate = now;
      readSerial();
      return;
    }
  }




protected:

  ventilationStatus _ventilationStatus;
  ventilatorStatus  _ventilatorStatus;

  ApolloHal*        _hal;
  ApolloStorage*    _storage;

  float     _avgTelemetryUpdateMicros = 0;
  uint32_t  _telemetryLoops           = 0;
  uint32_t  _ventilationLoops         = 0;

  unsigned long _lastTelemetryUpdate      = 0;
  unsigned long _lastCommunicationsUpdate = 0;
  unsigned long _lastMetricsUpdate        = 0;
  unsigned long _lastVentilationUpdate    = 0;


  void ventilationUpdate()
  {

  }





  void sendTelemetry()
  {
    unsigned long start = micros();
    ApolloProtocol::sendMedicalData(_hal->getPressure(),_hal->getInputFlow(),_hal->getOutputFlow(), _hal->getInputInstantFlow(), _hal->getOutputInstantFlow());
    _hal->sendHWInfo();
    uint16_t elapsed = getMilisecondsFrom(start);
    _avgTelemetryUpdateMicros = ((_avgTelemetryUpdateMicros*5) + elapsed ) / 6.0;
    _telemetryLoops++;
  }


  void readSerial()
  {
    if(!Serial.available()) return;
    String payload = Serial.readStringUntil('\n');
    StaticJsonDocument<MAX_JSON_SIZE> jsonInput;
    DeserializationError error = deserializeJson(jsonInput, payload);
    if (error) {
      _hal->debug("deserializeJson() failed: " + String(error.c_str()));
      _hal->debug(payload);
      return;
    }

    String type = jsonInput[STR_JSON_TYPE];
    if(!type)
    {
      _hal->debug("JSON with no type!");
      return;
    }
    if(type == STR_COMMAND)
    {
      String cmd = jsonInput[STR_COMMAND];
      if(cmd == STR_COMMAND_START)
      {
        _hal->debug("START no implementado");
      }
      else if(cmd == STR_COMMAND_STOP)
      {
        _hal->debug("STOP no implementado");
      }
      else if(cmd == STR_COMMAND_PAUSE)
      {
        _hal->debug("PAUSE no implementado");
      }
      else if(cmd == STR_COMMAND_TEST)
      {
        _hal->test();
      }
      else if(cmd == STR_COMMAND_CALIBRATE)
      {
        _hal->calibrate();
      }
      else if(cmd == STR_COMMAND_GET_CONFIG)
      {
        ApolloProtocol::sendConfig(_storage->getConfig());
      }
    }

  }

};


#endif
