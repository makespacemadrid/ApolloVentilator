#ifndef APOLLO_FLOW_SENSOR_H
#define APOLLO_FLOW_SENSOR_H

#include "Arduino.h"

class ApolloFlowSensor
{
    public:
        ApolloFlowSensor();
        ~ApolloFlowSensor();
        virtual bool begin();
        virtual float getFlow()         {return 0;} //implementar en la herencia!
        virtual float getInstantFlow()  {return 0;}
        virtual void resetFlow()        {;}
        virtual void update()           {;}
        virtual void pulse()            {;}
    protected:


};


#endif
