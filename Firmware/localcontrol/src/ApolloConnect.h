#ifndef _APOLLO_CONNECT_H
#define _APOLLO_CONNECT_H

#include "../include/defaults.h"
#include "Arduino.h"

class ApolloConnect
{
public:
    ApolloConnect(int bauds=115200){};
    void send();
    double _mode = 0;
    double _weight = 80;
    double _height = 170;
    double _sexo = 0;
    double _rpm = 15;
    double _mlTidalVolume = 400;
    double _porcentajeInspiratorio = 33;
    double _porcentajeFiO2 = 90;
    double _pressionPeep = 14.0;
    double _pressionPico = 45.0;
    double _pressionMeseta = 32.0;
    double _pressionMax = 50.0;
    double _presionTriggerInspiration = 3.0;

private:
    //HardwareSerial _port = Serial;


    bool defaultConfig();
};

#endif //_APOLLO_CONNECT_H