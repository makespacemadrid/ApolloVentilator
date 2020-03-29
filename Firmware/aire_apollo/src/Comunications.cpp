#include "Comunications.h"
#include "ApolloConfiguration.h"
#include "Arduino.h"

Comunications::Comunications(ApolloConfiguration *config)
{
    this->config = config;
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
    for (size_t i = 1; i < size; i++)
    {
        msgs += "," + msg[i];
    }
    send("DATA:" + msgs);
}
void Comunications::debug(String module, String msg)
{
    send("DEBUG:" + module + ":" + msg);
}

void Comunications::serialRead()
{
    String string = "";
    if (Serial.available())
    {
        string = Serial.readString(); // read the incoming data as string
    }
    if (string == "")
    {
        return;
    }

    String ipmsg[16];
    char received[50];
    string.toCharArray(received, 50);
    byte index = 0;
    char strings[10];
    char *ptr = NULL;
    ptr = strtok(received, ":,"); // takes a list of delimiters
    while (ptr != NULL)
    {
        strings[index] = ptr;
        index++;
        ptr = strtok(NULL, ":,"); // takes a list of delimiters
    }

    // for (int n = 0; n < index; n++)
    // {
    //     Serial.println(String(n) + " " + String(strings[n]));
    //     Serial.flush();
    // }
    if (!String(strings[0]).equals("CONFIG"))
    {
        this->config->parseConfig(strings);
        return;
    }
    if (!String(strings[0]).equals("GETCONFIG"))
    {
        this->send(config->getConfig());
        return;
    }
}