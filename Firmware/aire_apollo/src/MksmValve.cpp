#include "MksmValve.h"
#include <Arduino.h>
#include "trace.h"


/**
 * @brief Construct a new Apollo Valve:: Apollo Valve object
 * 
 * @param pin pin conectes to the valve
 * @param hz 
 * @param invertedLogic 
 */
MksmValve::MksmValve(uint8_t pin, uint16_t hz, bool invertedLogic) : ApolloValve{_pin, hz, invertedLogic} 
{
    pinMode(_pin, OUTPUT);
}


MksmValve::~MksmValve()
{}


/**
 * @brief open the valve 
 * 
 * @param percent Not used in this valve
 */
void MksmValve::open(uint8_t percent)
{
    _openPercent = 100;    
   digitalWrite(_pin,_trueState);
}

void MksmValve::close()
{
    _openPercent = 0;
    digitalWrite(_pin,!_trueState);
}


