#ifndef APOLLO_VALVE_H
#define APOLLO_VALVE_H
#include<Arduino.h>

class ApolloValve
{
    public:
        ApolloValve();
        ApolloValve(uint8_t pin, uint16_t hz = 10, bool invertedLogic = false);
        ~ApolloValve();
        virtual bool    begin();
        virtual void    open(uint8_t percent = 100) {;}
        virtual void    close()                     {;}
        virtual uint8_t status()                    {return 0;}
        virtual void    update()                    {;}
    protected:
};

#endif
