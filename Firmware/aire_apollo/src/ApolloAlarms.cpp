#include "ApolloAlarms.h"

ApolloAlarms::ApolloAlarms(Comunications *com, uint8_t pin_buzzer, bool invert = false)
{
    this->com = com;
    this->pin_buzzer = pin_buzzer;
    this->invert = invert;
};

bool ApolloAlarms::begin()
{
    pinMode(this->pin_buzzer, OUTPUT);
    this->beep(10, 5, 100);
    this->state = StateAlarm::Silent;
    this->com->alert(0, "Alarms initiated");
    return true;
}

void ApolloAlarms::info(int errn, String msg)
{
    this->com->alert(errn, msg);
    if (StateAlarm::Info > this->state)
    {
        this->state = StateAlarm::Info;
    }
};

void ApolloAlarms::warning(int errn, String msg)
{
    this->com->alert(errn, msg);
    if (StateAlarm::Warning > this->state)
    {
        this->state = StateAlarm::Warning;
    }
};

void ApolloAlarms::error(int errn, String msg)
{
    this->com->alert(errn, msg);
    if (StateAlarm::Error > this->state)
    {
        this->state = StateAlarm::Error;
    }
};

void ApolloAlarms::critical(int errn, String msg)
{
    this->com->alert(errn, msg);
    if (StateAlarm::Critical > this->state)
    {
        this->state = StateAlarm::Critical;
    }
};

void ApolloAlarms::beep(unsigned long time, int times = 1, unsigned long timeWait = 0)
{
    if (this->cicle)
    {
        return;
    }

    if (timeWait == 0)
    {
        timeWait = time;
    }

    this->loop = 0;
    this->loopFin = times;
    this->time = time;
    this->timeWait = timeWait;
    this->action = true;
    this->cicle = true;
    return;
}

void ApolloAlarms::check()
{
    switch (this->state)
    {
    case StateAlarm::Silent:
        this->loopFin = 0;
        this->cicle = false;
        break;
    case StateAlarm::Info:
        this->beep(100, 1, 1000);
        this->state = StateAlarm::Silent;
        break;
    case StateAlarm::Warning:
        this->beep(200, 3, 300);
        break;
    case StateAlarm::Error:
        this->beep(200, 3, 100);
        break;
    case StateAlarm::Critical:
        //No sería necesario ya que en estado Crítico el sonido siempre estará sonando
        this->beep(999, 99);
        break;
    default:
        break;
    }
    this->sound();
}

void ApolloAlarms::alarmReset()
{
    this->state = StateAlarm::Silent;
}

void ApolloAlarms::sound()
{
    if (this->loop <= this->loopFin || this->state == StateAlarm::Critical)
    {
        if (this->invert)
        {
            digitalWrite(this->pin_buzzer, !this->action);
        }
        else
        {
            digitalWrite(this->pin_buzzer, this->action);
        }
        unsigned long now = millis();
        unsigned long ntime = (this->action) ? time : timeWait;
        if ((now - this->tini) > ntime && this->state != StateAlarm::Critical)
        {
            this->action = !this->action;
            this->loop++;
            this->tini = now;
        }
    }
    else
    {
        if (this->invert)
        {
            digitalWrite(this->pin_buzzer, 1);
        }
        else
        {
            digitalWrite(this->pin_buzzer, 0);
        }
        this->cicle = false;
    }
}