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
    float speedInsufflation,
    float speedExsufflation,
    int ventilationCyle_WaitTime)
{

    _init(
        hal,
        mlTidalVolume,
        secTimeoutInsufflation,
        secTimeoutExsufflation,
        speedInsufflation,
        speedExsufflation,
        ventilationCyle_WaitTime,
        LPM_FLUX_TRIGGER_VALUE_NONE);
}

MechVentilation::MechVentilation(
    ApolloHal *hal,
    int mlTidalVolume,
    float secTimeoutInsufflation,
    float secTimeoutExsufflation,
    float speedInsufflation,
    float speedExsufflation,
    int ventilationCyle_WaitTime,
    float lpmFluxTriggerValue)
{
    _init(
        hal,
        mlTidalVolume,
        secTimeoutInsufflation,
        secTimeoutExsufflation,
        speedInsufflation,
        speedExsufflation,
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
        insuflationBefore();
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
    float speedInsufflation,
    float speedExsufflation,
    int ventilationCyle_WaitTime,
    float lpmFluxTriggerValue)
{
    /* Set configuration parameters */
    hal = hal;
    _cfgmlTidalVolume = mlTidalVolume;
    _cfgSecTimeoutInsufflation = secTimeoutInsufflation;
    _cfgSecTimeoutExsufflation = secTimeoutExsufflation;
    _cfgSpeedInsufflation = speedInsufflation;
    _cfgSpeedExsufflation = speedExsufflation;
    _cfgLpmFluxTriggerValue = lpmFluxTriggerValue;

    /* Initialize internal state */
    _currentState = State::Wait;
    _secTimerCnt = 0;
    _secTimeoutInsufflation = 0;
    _secTimeoutExsufflation = 0;
    _speedInsufflation = 0;
    _speedExsufflation = 0;
}

void MechVentilation::_setState(State state)
{
    //_previousState = _currentState;
    _currentState = state;
}

void MechVentilation::stateNext()
{
    switch (_currentState)
    {
    case State::Wait:
        _currentState = State::InsuflationBefore;
        break;
    case State::InsuflationBefore:
        _currentState = State::InsufaltionProcess;
        break;
    case State::InsufaltionProcess:
        _currentState = State::InsuflationAfter;
        break;
    case State::InsuflationAfter:
        _currentState = State::ExsufflationBefore;
        break;
    case State::ExsufflationBefore:
        _currentState = State::ExsufflationProcess;
        break;
    case State::ExsufflationProcess:
        _currentState = State::ExsufflationAfter;
        break;
    case State::ExsufflationAfter:
        _currentState = State::Wait;
        break;
    default:
        break;
    }
}

void MechVentilation::wait()
{
    //Detecta aspiración del paciente
    if (false)
    {
        /** @todo Pendiente desarrollo */
    }

    //Se lanza por tiempo
    unsigned long now = millis();
    if ((lastExecution + currentWaitTriggerTime) > now)
    {
        lastExecution = now;
        stateNext();
    }
}
void MechVentilation::insuflationBefore()
{
    /**
     *  @todo Decir a la válvula que se abra
     * 
    */
}
void MechVentilation::insufaltionProcess()
{
    //El proceso de insuflación está en marcha, esperamos al sensor de medida o tiempo de insuflación max
    /** @todo conectar sesor ml/min */
    float volumensensor = 0;
    unsigned long now = millis();
    if (volumensensor >= _cfgmlTidalVolume || (now - lastExecution) >= _cfgSecTimeoutInsufflation)
    {
        /** @todo Paramos la insuflación */
        stateNext();
    }
}
void MechVentilation::insuflationAfter()
{
    unsigned long now = millis();
    if ((now - lastExecution) >= _cfgSecTimeoutInsufflation)
    {
        stateNext();
    }
}
void MechVentilation::exsufflationBefore()
{
    /** @todo Abrimos válvulas de salida */
    stateNext();
}
void MechVentilation::exsufflationProcess()
{
    unsigned long now = millis();
    if ((now - lastExecution + _cfgSecTimeoutInsufflation) >= _cfgSecTimeoutExsufflation)
    {
        stateNext();
    }
}
void MechVentilation::exsufflationAfter()
{
    /** @todo Cerramos valvula de salida? */
    stateNext();
}