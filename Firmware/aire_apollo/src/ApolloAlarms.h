#ifndef _APOLLO_ALARMS_H
#define _APOLLO_ALARMS_H

#include "../include/defaults.h"
#include "Arduino.h"
#include "Comunications.h"

enum StateAlarm
{
    Silent = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
};

class ApolloAlarms
{
public:
    ApolloAlarms(Comunications *com, uint8_t pin_buzzer, bool invert = false);
    bool begin();

    //Tipos de alarmas
    void info(int errn, String msg = "");
    void warning(int errn, String msg = "");
    void error(int errn, String msg = "");
    void critical(int errn, String msg = "");

    void alarmReset();
    void check();

private:
    int history[];
    Comunications *com;
    void beep(unsigned long time, int times = 1, unsigned long timeWait = 0);
    StateAlarm state = Silent;
    uint8_t pin_buzzer;
    bool invert; //Buzzer de las narices al revés.

    unsigned long tini = 0;
    unsigned long time = 0;
    unsigned long timeWait = 0;
    int loop = 0;
    int loopFin = 0;
    bool action = true;
    bool cicle = false;
    void sound();
};

#endif
