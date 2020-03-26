#include "ApolloFlowSensor.h"
#include <Arduino.h>

ApolloFlowSensor::ApolloFlowSensor()
{}

ApolloFlowSensor::~ApolloFlowSensor()
{}

bool ApolloFlowSensor::begin()
{
    return true;
}

float ApolloFlowSensor::read()
{
    return 0.00F;
}