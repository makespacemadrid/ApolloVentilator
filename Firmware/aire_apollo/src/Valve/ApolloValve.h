#ifndef APOLLO_VALVE_H
#define APOLLO_VALVE_H

#include<Arduino.h>
#include "../../include/defaults.h"
#include "trace.h"
class ApolloValve
{
    public:
        ApolloValve();
        ~ApolloValve();
        virtual void    open(double percent = 100, bool wait = false) {;}
        virtual void    close(bool wait = false)                      {open(0,wait);}
        virtual bool    begin()     {TRACE("METODO VIRTUAL SIN IMPLEMENTAR!") ; return false;}
        virtual bool    test()      {TRACE("METODO VIRTUAL SIN IMPLEMENTAR!") ; return false;}
        virtual double  status()    {TRACE("METODO VIRTUAL SIN IMPLEMENTAR!") ; return 0;}
        virtual double  target()    {return status();}
        virtual void    update()         {;}
        virtual void    highFreqUpdate() {;}
    protected:
};

#endif
