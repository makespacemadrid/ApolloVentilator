#ifndef APOLLO_FLOW_SENSOR_H
#define APOLLO_FLOW_SENSOR_H

#include "Arduino.h"
#include "../../../include/defaults.h"
#include "trace.h"

class ApolloFlowSensor
{
    public:
        ApolloFlowSensor();
        ~ApolloFlowSensor();
        virtual bool begin()            {TRACE("METODO VIRTUAL SIN IMPLEMENTAR!") ;return false;}
        virtual float getFlow()         {TRACE("METODO VIRTUAL SIN IMPLEMENTAR!") ;return 0;} //implementar en la herencia!
        virtual float getInstantFlow()  {TRACE("METODO VIRTUAL SIN IMPLEMENTAR!") ;return 0;}
        virtual void resetFlow()        {;}
        virtual void update()           {;}
        virtual void highFreqUpdate()   {;}
        virtual void pulse()            {;}
    protected:


};


#endif
