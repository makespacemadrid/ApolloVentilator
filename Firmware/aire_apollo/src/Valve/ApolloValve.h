#ifndef APOLLO_VALVE_H
#define APOLLO_VALVE_H

#include "../../include/defaults.h"
#include<Arduino.h>

class ApolloValve
{
    public:
        ApolloValve();
        ~ApolloValve();
        virtual bool    begin()                     {return true;}
        virtual void    open(double percent = 100)  {;}
        virtual void    close()                     {;}
        virtual double  status()                    {return 0;}
        virtual void    update()                    {;}
    protected:
};

#endif
