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

void Comunications::data(String msg[])
{
        //Serial.println("PENESSSS!!!!");
        //Serial.println(msg[0]);

        String msgs;
        for (size_t i = 0; i < msg->length() - 1; i++)
        {
            msgs = msgs + "," + msg[i];
        }
        send("DATA:" + msgs);
}
