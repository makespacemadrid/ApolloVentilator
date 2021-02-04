#ifndef COMUNICATIONS_H
#define COMUNICATIONS_H
#include <Arduino.h>
#include "../include/defaults.h"
#include "ApolloConfiguration.h"

class Comunications
{
public:
    Comunications(ApolloConfiguration *config);
    void alert(int errn, String msg);
    void data(String msg[], uint8_t size);
    void debug(String module, String msg);
    bool serialRead();

protected:
    void send(String msg);
    byte nmsg;
    String msgs[];

private:
    unsigned long lastLogTime = 0;
    ApolloConfiguration *config;
};

#endif
