#include "ApolloHal.h"
#include <Arduino.h>

ApolloHal::ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *entryFlowSensor, ApolloFlowSensor *exitFlowSensor, ApolloValve *enrtyEV, ApolloValve *exitEV)
{
    this->preSensor = preSensor;
    this->entryFlowSensor = entryFlowSensor;
    this->exitFlowSensor = exitFlowSensor;
    this->entryEV = entryEV;
    this->exitEV = exitEV;
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

    status &= preSensor->begin();
    status &= entryFlowSensor->begin();
    status &= exitFlowSensor->begin();
    status &= entryEV->begin();
    status &= exitEV->begin();

    return status;
}

void ApolloHal::ISR1ms()
{
  entryFlowSensor->update();
  exitFlowSensor->update();
  entryEV->update();
  exitEV->update();
  preSensor->update();
}

void ApolloHal::setFlow(float flow, float pressure)
{
}

// Get metric from pressure sensor in mBar
float ApolloHal::getMetricPressureEntry()
{
    // preSensor MUST return value in mBar
    float val = preSensor->readMilibar();
    return val;
}
int ApolloHal::getPresureIns()
{
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
  entryEV->open();
}
void ApolloHal::valveInsClose()
{
  entryEV->close();
}
void ApolloHal::valveExsOpen()
{
  exitEV->open();
}
void ApolloHal::valveExsClose()
{
  exitEV->close();
}

// Get metric from entry flow sensor
float ApolloHal::getMetricVolumeEntry()
{
    float val = entryFlowSensor->getInstantFlow();
    return val;
}

// Get metric from pressure sensor in mBar
float ApolloHal::getMetricVolumeExit()
{
    // preSensor MUST return value in mBar
    float val = exitFlowSensor->getInstantFlow();
    return val;
}


void ApolloHal::beginInspiration()
{
    entryEV->open();
    exitEV->close();
}

void ApolloHal::beginEspiration()
{
    entryEV->close();
    exitEV->open();
}
