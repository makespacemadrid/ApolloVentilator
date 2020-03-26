#ifndef APOLLO_VALVE_H
#define APOLLO_VALVE_H
#include<Arduino.h>

class ApolloValve
{    
    public:
        ApolloValve();
        ApolloValve(uint8_t pin, uint16_t hz = 10, bool invertedLogic = false);
        ~ApolloValve(); 
        virtual bool begin();
        virtual void open(uint8_t percent = 100);
        virtual void close(); 
        virtual uint8_t status();          
    
    protected:        
       
        const uint8_t   _pin;
        const bool      _trueState;
        const uint16_t  _valveCycleMs;       
        uint8_t         _openPercent;
        uint16_t        _openDelayMS;
        uint16_t        _closeDelayMS;
        uint16_t        _openTimeMS;       
};

#endif