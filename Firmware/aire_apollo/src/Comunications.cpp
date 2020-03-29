#include "Comunications.h"

Comunications::Comunications()
{
}

void Comunications::send(String msg)
{
    Serial.println(String(msg));
}

void Comunications::alert(String msg)
{
    send("ALERT:" + msg);
}

void Comunications::data(String msg[], uint8_t size)
{
        String msgs;
        msgs = msg[0];
        for (size_t i = 1; i < size ; i++)
        {
            msgs += "," + msg[i];
        }
        send("DATA:" + msgs);
}
