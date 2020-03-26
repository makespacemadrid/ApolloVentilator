#include "ApolloHal.h"
#include <Arduino.h>

ApolloHal::ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *flowSensor, ApolloValve *enrtyEV, ApolloValve *exitEV)
{
    this->preSensor = preSensor;
    this->flowSensor = flowSensor;
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
    bool status = true;

    status &= preSensor->begin();
    status &= flowSensor->begin();
    status &= entryEV->begin();
    status &= exitEV->begin();

    return status;
}

ApolloHal::~ApolloHal()
{
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    digitalWrite(10, LOW);
}

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
void ApolloHal::valveOpen()
{
    Serial.println("Open valve");
    digitalWrite(9, HIGH);
    delayMicroseconds(500);
    for (int i = 0; i < 100; i++)
    {
        digitalWrite(8, HIGH);
        delayMicroseconds(500);
        digitalWrite(8, LOW);
        delayMicroseconds(500);
    }
}
void ApolloHal::valveClose()
{
    Serial.println("Close valve");
    digitalWrite(9, LOW);
    delayMicroseconds(500);
    for (int i = 0; i < 100; i++)
    {
        digitalWrite(8, HIGH);
        delayMicroseconds(500);
        digitalWrite(8, LOW);
        delayMicroseconds(500);
    }
}
