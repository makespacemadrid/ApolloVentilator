#include "ApolloHal.h"
#include <Arduino.h>

ApolloHal::ApolloHal()
{}

ApolloHal::~ApolloHal()
{}

void ApolloHal::setFlow(float flow, float pressure)
{

}

// Get metric from pressure sensor in mBar
float ApolloHal::getMetricPressureEntry()
{
    // preSensor MUST return value in mBar
    float val = preSensor.readPressure();
    return val;    
}
