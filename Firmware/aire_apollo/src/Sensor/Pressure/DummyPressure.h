#ifndef DUMMY_PRESSURE_H
#define DUMMY_PRESSURE_H

#include "ApolloPressureSensor.h"
#include "../../../include/defaults.h"
/**
 * @brief Dummy pressure sensor for test proposes
 */
class DummyPressure : public ApolloPressureSensor
{
public:
    DummyPressure(){};

    bool begin(){return true;};
//    float readPascal(){return 1176.8;}; //12 cmH2o
    float readPascal(){return 2000;}; //12 cmH2o
private:
};

#endif
