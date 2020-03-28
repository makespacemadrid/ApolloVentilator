#ifndef APOLLO_VALVE_H
#define APOLLO_VALVE_H
#include<Arduino.h>

class ApolloValve
{
    public:
        ApolloValve();
        ~ApolloValve();
        virtual bool    begin()                     {  Serial.println("BEGIN VALVE FATHER");Serial.flush();return true;}
        virtual void    open(uint8_t percent = 100) {;}
        virtual void    close()                     {;}
        virtual uint8_t status()                    {return 0;}
        virtual void    update()                    {;}
    protected:
};

#endif
