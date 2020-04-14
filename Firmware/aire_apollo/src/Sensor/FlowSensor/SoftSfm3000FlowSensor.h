#ifndef SOFT_SFM3000_FLOW_SENSOR_H
#define SOFT_SFM3000_FLOW_SENSOR_H

#include "Sfm3000FlowSensor.h"

#define SDA_PORT PORTC
#define SDA_PIN 2 
#define SCL_PORT PORTC
#define SCL_PIN 3 
#include <SoftWire.h>

#define I2C_7BITADDR 0x40 
#define MEMLOC 0x0A
#define ADDRLEN 1


SoftWire softWire = SoftWire();

class SoftSfm3000FlowSensor : public Sfm3000FlowSensor
{
    public:      
        bool begin();
      
    protected:      
        uint8_t     sendCommand(uint16_t command);            
        int         readBytes();               
};


#endif
