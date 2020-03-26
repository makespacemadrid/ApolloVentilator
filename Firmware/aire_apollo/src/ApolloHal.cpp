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

/**
 * @brief Initialize and check all sensor
 * 
 * @return true if all sensor are present a initilized
 * @return false if something goes wrong
 */
bool ApolloHal::begin()
{
    bool status=true;

    status &= preSensor->begin();
    status &= entryFlowSensor->begin();
    status &= exitFlowSensor->begin();
    status &= entryEV->begin();
    status &= exitEV->begin();

    return status;
}

ApolloHal::~ApolloHal()
{}

void ApolloHal::setFlow(float flow, float pressure)
{

}

// Get metric from pressure sensor in mBar
float ApolloHal::getMetricPressureEntry()
{
    // preSensor MUST return value in mBar
    float val = preSensor->read();
    return val;    
}

// Get metric from entry flow sensor
float ApolloHal::getMetricVolumeEntry()
{    
    float val = entryFlowSensor->read();
    return val;    
}

// Get metric from pressure sensor in mBar
float ApolloHal::getMetricVolumeExit()
{
    // preSensor MUST return value in mBar
    float val = exitFlowSensor->read();
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