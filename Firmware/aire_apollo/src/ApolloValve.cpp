#include "ApolloValve.h"
#include <Arduino.h>
#include "trace.h"


/**
 * @brief Construct a new Apollo Valve:: Apollo Valve object
 * 
 * @param pin pin conectes to the valve
 * @param hz 
 * @param invertedLogic 
 */
ApolloValve::ApolloValve(uint8_t pin, uint16_t hz, bool invertedLogic) :
  _pin(pin),_trueState(!invertedLogic),_valveCycleMs(1000/hz),
  _openDelayMS(_valveCycleMs/4),_closeDelayMS(_valveCycleMs/4)
  {
    pinMode(_pin, OUTPUT);
  }


ApolloValve::~ApolloValve()
{}

bool ApolloValve::begin()
{
    return true;
}

/**
 * @brief open the valve to the indicated %
 * 
 * @param percent how much to open the valve
 */
void ApolloValve::open(uint8_t percent)
  {
    if(percent > 100) 
        percent = 100;

    if(percent == 0)  
        close();

    _openPercent = percent;    
   
  }

void ApolloValve::close()
{
    _openPercent = 0;    
}


/**
 * @brief 
 * 
 * @return uint8_t open percent
 */
uint8_t ApolloValve::status()
{
    return _openPercent;
}