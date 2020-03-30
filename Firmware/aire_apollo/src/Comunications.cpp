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

bool Comunications::serialRead()
{
    if (Serial.available() == 0)
    {
        return false;
    }
    String string = Serial.readString(); // read the incoming data as string
    Serial.println("RECEIBED->" + string);
    String ipmsg[16];
    char received[50];
    string.toCharArray(received, 50);
    byte index = 0;
    String strings[10];
    char *ptr = NULL;
    ptr = strtok(received, ":,"); // takes a list of delimiters
    while (ptr != NULL)
    {
        strings[index] = String(ptr);
        index++;
        ptr = strtok(NULL, ":,"); // takes a list of delimiters
    }

    // for (int n = 0; n < index; n++)
    // {
    //     Serial.println(String(n) + " " + String(strings[n]));
    //     Serial.flush();
    // }
    if (strings[0] == String("CONFIG"))
    {
        this->config->parseConfig(strings);
        return true;
    }
    if (strings[0] == String("GETCONFIG"))
    {
        this->send(config->getConfig());
        return false;
    }
}