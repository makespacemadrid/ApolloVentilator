#ifndef SFM3000_FLOW_SENSOR_H
#define SFM3000_FLOW_SENSOR_H

#include "ApolloFlowSensor.h"

#define SFM3000_START_FLOW      0x1000      // Start continuous measurement
#define SFM3000_START_TEMP      0X1001      // Start temp measurement
#define SFM3000_READ_SCALE      0x30DE      // Read flow scale factor
#define SFM3000_READ_OFFSET     0x30DF      // Read flow offset
#define SFM3000_PART_NO_1       0x31E3      // Firs part of type/article number
#define SFM3000_PART_NO_2       0x31E4      // Second part of type/article number
#define SFM3000_SERIAL_1        0x31AE      // First command to read serial
#define SFM3000_SERIAL_2        0x31AF      // Second command to read serial
#define SFM3000_RESET           0x2000      // Soft reset

#define POLYNOMIAL 0x31     //P(x)=x^8+x^5+x^4+1 = 100110001

class Sfm3000FlowSensor : public ApolloFlowSensor
{
    public:
        Sfm3000FlowSensor(uint8_t sampling_ms = 10, uint8_t addr = 0x40);
        ~Sfm3000FlowSensor();
        virtual bool begin();
        float getFlow();
        float getInstantFlow();
        void resetFlow();
        virtual void update();

    protected:
        uint8_t       _addr;
        uint8_t       _data[3];
        uint16_t      _scaleFactor;
        uint16_t      _flowOffset;

        uint8_t       _samplingMS;
        float         _volSinceReset;               // This value represent number of ml provided to the patient since last cycle reset
        unsigned long _lastSampleTime;
        float         _lastInstantFlow;

        uint8_t     sendCommand(uint16_t command);
        bool        softReset();
        virtual int readBytes();
        uint16_t    readOffset();
        uint16_t    readScale();
        bool        startFlowMeasurement();
        uint8_t     calcCRC(uint8_t x, uint8_t crc);

};


#endif
