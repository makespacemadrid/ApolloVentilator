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
    Comunications::send("ALERT:" + msg);
}

void Comunications::data(String msg[])
{
    unsigned long now = millis();
    if ((now - lastLogTime) >= logInterval)
    {
        String msgs;
        for (size_t i = 0; i < msg->length() - 1; i++)
        {
            msgs = msgs + "," + msg[i];
        }
        lastLogTime = now;
        Comunications::send("DATA:" + msgs);
    }
}