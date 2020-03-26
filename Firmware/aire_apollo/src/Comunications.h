#ifndef COMUNICATIONS_H
#define COMUNICATIONS_H
#include <Arduino.h>
#include "../include/defaults.h"

class Comunications
{
public:
    Comunications();
    void alert(String msg);
    void data(String msg[]);

protected:
    void send(String msg);
    byte nmsg;
    String msgs[];

private:
    unsigned long lastLogTime = 0;
    unsigned long logInterval = LOG_INTERVAL;
};

#endif