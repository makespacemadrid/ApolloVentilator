#include "ApolloHal.h"
#include <Arduino.h>
#include "trace.h"

ApolloHal::ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *entryFlowSensor, ApolloFlowSensor *exitFlowSensor, ApolloValve *entryEV, ApolloValve *exitEV)
{
  _preSensor = preSensor;
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
  return _exitFlowSensor->getInstantFlow();
  ;
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
