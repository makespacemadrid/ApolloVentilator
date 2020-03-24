#ifndef APOLLO_VALVE_H
#define APOLLO_VALVE_H

class ApolloValve
{
    public:
        ApolloValve();
        ~ApolloValve(); 
        void open();
        void close(); 
        virtual int readState();   
        virtual void setOpeningDegree(int value);   
    private:
        
};

#endif