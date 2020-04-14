#ifndef SOFT_SFM3000_FLOW_SENSOR_H
#define SOFT_SFM3000_FLOW_SENSOR_H

#include "Sfm3000FlowSensor.h"



class SoftSfm3000FlowSensor : public Sfm3000FlowSensor
{
    public:      
        bool begin();
      
    protected:      
        uint8_t     sendCommand(uint16_t command);            
        int         readBytes();               
};


#endif
