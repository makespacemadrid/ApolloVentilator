#ifndef APOLLO_VALVE_H
#define APOLLO_VALVE_H

#include "../../include/defaults.h"
#include<Arduino.h>

class ApolloValve
{
    public:
        ApolloValve();
        ~ApolloValve();
        virtual bool    begin()                         {return true;}
        virtual void    open(double percent = 100)      {;}
        virtual void    close()                         {;}
        virtual void    waitOpen(double percent = 100)  {open(percent);}
        virtual void    waitClose()                     {close();}
        virtual double  status()                        {return 0;}
        virtual double  target()                        {return status();}
        virtual void    update()                        {;}
    protected:
};

#endif
