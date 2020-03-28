#include "ApolloHal.h"
#include <Arduino.h>

ApolloHal::ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *entryFlowSensor, ApolloFlowSensor *exitFlowSensor, ApolloValve *entryEV, ApolloValve *exitEV)
{
    _preSensor    = preSensor;
    _entryFlowSensor = entryFlowSensor;
    _exitFlowSensor = exitFlowSensor;
    _entryEV = entryEV;
    _exitEV = exitEV;
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
    if(!_preSensor->begin())
    {
      Serial.println("ERROR PRESION");
      Serial.flush();
      status = false;
    }

    if(!_entryFlowSensor->begin())
    {
      Serial.println("ERROR FLOW1");
      Serial.flush();
      status = false;
    }

    if(!_exitFlowSensor->begin())
    {
      Serial.println("ERROR FLOW2");
      Serial.flush();
      status = false;
    }

    if(!_entryEV->begin())
    {
      Serial.println("ERROR ev1");
      Serial.flush();
      status = false;
    }

    if(!_exitEV->begin())
    {
      Serial.println("ERROR ev2");
      Serial.flush();
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
    return _preSensor->readMilibar();;
}
int ApolloHal::getPresureIns()
{
  return 1;
    if (digitalRead(0) == HIGH)
    {
        Serial.println("Respira");
        return -10;
    }
    else
    {
        return 1;
    }
}
int ApolloHal::getPresureExp()
{
  return 40;
    if (digitalRead(1) == HIGH)
    {
        Serial.println("Expira demasiado");
        return 13;
    }
    else
    {
        return 40;
    }
}

void ApolloHal::valveInsOpen()
{
  _entryEV->open();
}
void ApolloHal::valveInsClose()
{
  _entryEV->close();
}
void ApolloHal::valveExsOpen()
{
  _exitEV->open();
}
void ApolloHal::valveExsClose()
{
  _exitEV->close();
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
    return _exitFlowSensor->getInstantFlow();;
}


void ApolloHal::beginInspiration()
{
    _entryEV->open();
    _exitEV->close();
}

void ApolloHal::beginEspiration()
{
    _entryEV->close();
    _exitEV->open();
}
