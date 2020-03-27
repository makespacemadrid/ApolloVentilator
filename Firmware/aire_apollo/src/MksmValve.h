#ifndef MKSM_VALVE_H
#define MKSM_VALVE_H

#include<Arduino.h>
#include "ApolloValve.h"

class MksmValve : public ApolloValve
{
    public:
        MksmValve();
        MksmValve(uint8_t pin, uint16_t hz = 10, bool invertedLogic = false);
        ~MksmValve(); 
        virtual void open(uint8_t percent = 100);
        virtual void close(); 
         
};

#endif