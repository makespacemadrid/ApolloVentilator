#ifndef HSC_SPI_PRESS_SENSOR_H
#define HSC_SPI_PRESS_SENSOR_H

#include "ApolloPressureSensor.h"
#include <Arduino.h>
#include "HoneywellTruStabilitySPI.h"


enum sensorStatus {
    HscValidData,       ///< Data readed by sensor and ready for master to read
    HscCommandMode,     ///< This should never happend
    HscStaleData,       ///< Old data already readed by master
    HscDiagCond         ///< Diagnostic condition. Something went wrong
                        ///     Sensor EEPROM has changed
                        ///     Loss of sense element connection
                        ///     Short circuit of sense element 
};

/**
 * @brief HoneywellTruStabilitySPI  SPI pressure sensor
 * 
 */
class HscSpiPresSensor : public ApolloPressureSensor
{
    public:
        HscSpiPresSensor(uint8_t pin = 0);
        ~HscSpiPresSensor();

        float read();
        bool begin();  
       
          
    private:
        uint8_t     _pin;
        TruStabilityPressureSensor *_sensor;
        const float _min_pressure = -6894.76;     ///< Value in Pa
        const float _max_pressure = 6894.76;    ///< Value in Pa
};


#endif