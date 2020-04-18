#ifndef SOFT_SFM3000_FLOW_SENSOR_H
#define SOFT_SFM3000_FLOW_SENSOR_H

#include "Sfm3000FlowSensor.h"
#include <SoftwareWire.h>


class SoftSfm3000FlowSensor : public Sfm3000FlowSensor
{
    public:
        SoftSfm3000FlowSensor(uint8_t sampling_ms = 10, uint8_t addr = 0x40);
        bool begin();

    protected:
        uint8_t     sendCommand(uint16_t command);
        int         readBytes();
};


#endif
