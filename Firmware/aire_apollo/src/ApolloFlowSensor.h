#ifndef APOLLO_FLOW_SENSOR_H
#define APOLLO_FLOW_SENSOR_H

class ApolloFlowSensor
{
    public:
        ApolloFlowSensor();
        ~ApolloFlowSensor();
        virtual bool begin();
        virtual float read();
    private:
        
};


#endif