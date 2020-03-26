/** Mechanical ventilation.
 *
 * @file MechVentilation.cpp
 *
 * This is the mechanical ventilation software module.
 * It handles the mechanical ventilation control loop.
 */
#include <float.h>
#include "MechVentilation.h"
#include "defaults.h"
#include <../lib/TimerOne-1.1.0/TimerOne.h>

/** No trigger. */
#define LPM_FLUX_TRIGGER_VALUE_NONE FLT_MAX

int currentWaitTriggerTime = 0;
int currentStopInsufflationTime = 0;
float currentFlow = 0;
#if DEBUG_STATE_MACHINE
extern String debugMsg[];
extern byte debugMsgCounter;
#endif

MechVentilation::MechVentilation(
    ApolloHal *hal,
    int mlTidalVolume,
    float secTimeoutInsufflation,
    float secTimeoutExsufflation,
    int ventilationCyle_WaitTime)
{

    _init(
        hal,
        mlTidalVolume,
        secTimeoutInsufflation,
        secTimeoutExsufflation,
        ventilationCyle_WaitTime,
        LPM_FLUX_TRIGGER_VALUE_NONE);
}

MechVentilation::MechVentilation(
    ApolloHal *hal,
    int mlTidalVolume,
    float secTimeoutInsufflation,
    float secTimeoutExsufflation,
    int ventilationCyle_WaitTime,
    float lpmFluxTriggerValue)
{
    _init(
        hal,
        mlTidalVolume,
        secTimeoutInsufflation,
        secTimeoutExsufflation,
        ventilationCyle_WaitTime,
        lpmFluxTriggerValue);
}

//TODO: use this method to play a beep in main loop, 1 second long for example.
boolean MechVentilation::getStartWasTriggeredByPatient()
{ //returns true if last respiration cycle was started by patient trigger. It is cleared when read.
    if (_startWasTriggeredByPatient)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void MechVentilation::setTidalVolume(float mlTidalVolume)
{
    _cfgmlTidalVolume = mlTidalVolume;
}

void MechVentilation::setTimeoutInsufflation(float secTimeoutInsufflation)
{
    _cfgSecTimeoutInsufflation = secTimeoutInsufflation;
}

void MechVentilation::setTimeoutExsufflation(float secTimeoutExsufflation)
{
    _cfgSecTimeoutExsufflation = secTimeoutExsufflation;
}

void MechVentilation::setSpeedInsufflation(float speedInsufflation)
{
    _speedInsufflation = _cfgSpeedInsufflation;
}

void MechVentilation::setSpeedExsufflation(float speedExsufflation)
{
    _speedExsufflation = _cfgSpeedExsufflation;
}

void MechVentilation::setVentilationCyle_WaitTime(float speedExsufflation)
{
    //TODO _speedExsufflation = _cfgSpeedExsufflation;
}

void MechVentilation::start(void)
{
    _running = true;
}

void MechVentilation::stop(void)
{
    _running = false;
}

/**
 * Function called by timer every 5 miliseconds
 */
void MechVentilation::update(void)
{
    switch (_currentState)
    {
    case State::Wait:
        wait();
        break;
    case State::InsuflationBefore:
        this->insuflationBefore();
        break;
    case State::InsufaltionProcess:
        insufaltionProcess();
        break;
    case State::InsuflationAfter:
        insuflationAfter();
        break;
    case State::ExsufflationAfter:
        exsufflationAfter();
        break;
    case State::ExsufflationProcess:
        exsufflationProcess();
        break;
    case State::ExsufflationBefore:
        exsufflationBefore();
        break;
    }
}

void MechVentilation::_init(
    ApolloHal *hal,
    int mlTidalVolume,
    float secTimeoutInsufflation,
    float secTimeoutExsufflation,
    int ventilationCyle_WaitTime,
    float lpmFluxTriggerValue)
{
    /* Set configuration parameters */
    this->hal = hal;
    this->_cfgmlTidalVolume = mlTidalVolume;
    this->_cfgSecTimeoutInsufflation = secTimeoutInsufflation;
    this->_cfgSecTimeoutExsufflation = secTimeoutExsufflation;
    this->_cfgLpmFluxTriggerValue = lpmFluxTriggerValue;

    /* Initialize internal state */
    this->_currentState = State::Wait;
    this->_secTimerCnt = 0;
    this->_secTimeoutInsufflation = 0;
    this->_secTimeoutExsufflation = 0;
}

void MechVentilation::_setState(State state)
{
    //_previousState = _currentState;
    this->_currentState = state;
}

void MechVentilation::stateNext()
{
    switch (this->_currentState)
    {
    case State::Wait:
        this->_currentState = State::InsuflationBefore;
        break;
    case State::InsuflationBefore:
        this->_currentState = State::InsufaltionProcess;
        break;
    case State::InsufaltionProcess:
        this->_currentState = State::InsuflationAfter;
        break;
    case State::InsuflationAfter:
        this->_currentState = State::ExsufflationBefore;
        break;
    case State::ExsufflationBefore:
        this->_currentState = State::ExsufflationProcess;
        break;
    case State::ExsufflationProcess:
        this->_currentState = State::ExsufflationAfter;
        break;
    case State::ExsufflationAfter:
        this->_currentState = State::Wait;
        break;
    default:
        break;
    }
}

void MechVentilation::wait()
{
    digitalWrite(5, HIGH);

    //Detecta aspiración del paciente
    if (this->hal->getPresureIns() <= 0)
    {
        /** @todo Pendiente desarrollo */
        stateNext();
        digitalWrite(5, LOW);
    }

    //Se lanza por tiempo
    unsigned long now = millis();
    if ((this->lastExecution + 5000) < now)
    {
        stateNext();
        digitalWrite(5, LOW);
    }
}
void MechVentilation::insuflationBefore()
{
    unsigned long now = millis();
    this->lastExecution = now;
    /**
     *  @todo Decir a la válvula que se abra
     * 
    */
    this->hal->valveExsClose();
    this->hal->valveInsOpen();
    this->stateNext();
}
void MechVentilation::insufaltionProcess()
{
    //El proceso de insuflación está en marcha, esperamos al sensor de medida o tiempo de insuflación max
    /** @todo conectar sesor ml/min */
    float volumensensor = 0;
    unsigned long now = millis();
    if (volumensensor >= this->_cfgmlTidalVolume || (now - this->lastExecution) >= (this->_cfgSecTimeoutInsufflation * 1000))
    {
        /** @todo Paramos la insuflación */
        this->stateNext();
    }
}
void MechVentilation::insuflationAfter()
{
    unsigned long now = millis();
    if ((now - this->lastExecution) >= (this->_cfgSecTimeoutInsufflation * 1000))
    {
        this->hal->valveInsClose();
        this->stateNext();
    }
}
void MechVentilation::exsufflationBefore()
{
    /** @todo Abrimos válvulas de salida */
    this->hal->valveExsOpen();
    stateNext();
}
void MechVentilation::exsufflationProcess()
{
    unsigned long now = millis();
    if ((now - lastExecution + (_cfgSecTimeoutInsufflation * 1000)) >= (_cfgSecTimeoutExsufflation * 1000))
    {
        stateNext();
    }
}
void MechVentilation::exsufflationAfter()
{
    /** @todo Cerramos valvula de salida? */
    //this->hal->valveExsClose();
    stateNext();
}
