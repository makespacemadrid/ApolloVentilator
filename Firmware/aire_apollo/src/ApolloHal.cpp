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
    pinMode(0, INPUT_PULLUP);
    pinMode(1, INPUT_PULLUP);

    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);

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
    //Serial.println("Open Ins valve");
    digitalWrite(7, HIGH);
    digitalWrite(6, LOW);
    // delayMicroseconds(500);
    // for (int i = 0; i < 100; i++)
    // {
    //     digitalWrite(8, HIGH);
    //     delayMicroseconds(500);
    //     digitalWrite(8, LOW);
    //     delayMicroseconds(500);
    // }
}
void ApolloHal::valveInsClose()
{
    //Serial.println("Close Ins valve");
    digitalWrite(7, LOW);
    digitalWrite(6, HIGH);
    // delayMicroseconds(500);
    // for (int i = 0; i < 100; i++)
    // {
    //     digitalWrite(8, HIGH);
    //     delayMicroseconds(500);
    //     digitalWrite(8, LOW);
    //     delayMicroseconds(500);
    // }
}
void ApolloHal::valveExsOpen()
{
    //Serial.println("Open Exs valve");
    digitalWrite(4, HIGH);
    digitalWrite(3, LOW);
}
void ApolloHal::valveExsClose()
{
    //Serial.println("Close Exs valve");
    digitalWrite(4, LOW);
    digitalWrite(3, HIGH);
}
