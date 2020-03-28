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
    int rpm,
    int porcentajeInspiratorio)
{
    _init(
        hal,
        mlTidalVolume,
        rpm,
        porcentajeInspiratorio);
}
void MechVentilation::setRpm(int rpm)
{
    if (rpm >= 10 && rpm <= 30)
    {
        this->_cfgRpm = rpm;
        this->_cfgUpdate = true;
    }
}

void MechVentilation::setTidalVolume(float mlTidalVolume)
{
    if (mlTidalVolume >= 300 && mlTidalVolume <= 600)
    {
        this->_cfgmlTidalVolume = mlTidalVolume;
        this->_cfgUpdate = true;
    }
}

void MechVentilation::setPressionPeep(float pressionPeep)
{
    this->_cfgPresionPeep = pressionPeep;
    this->_cfgUpdate = true;
}

void MechVentilation::setPorcentajeInspiratorio(int porcentajeInspiratorio)
{
    if (porcentajeInspiratorio >= 25 && porcentajeInspiratorio <= 50)
    {
        this->_cfgPorcentajeInspiratorio = porcentajeInspiratorio;
        this->_cfgUpdate = true;
    }
}

// Get tidal volume
float MechVentilation::getTidalVolume()
{
    return this->_cfgmlTidalVolume;
}
//Set RPM
int MechVentilation::getRpm()
{
    return this->_cfgRpm;
}
//Set Porcentaje inspiratorio
int MechVentilation::getporcentajeInspiratorio()
{
    return this->_cfgPorcentajeInspiratorio;
}
// Set presion peep
float MechVentilation::getPressionPeep()
{
    return this->_cfgPresionPeep;
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
    int rpm,
    int porcentajeInspiratorio)
{
    /* Set configuration parameters */
    this->hal = hal;
    this->_cfgmlTidalVolume = mlTidalVolume;
    this->_cfgRpm = rpm,
    this->_cfgPorcentajeInspiratorio = porcentajeInspiratorio;
    this->calcularCiclo(this->_cfgPorcentajeInspiratorio, this->_cfgRpm);
    // this->_cfgSecTimeoutInsufflation = secTimeoutInsufflation;
    // this->_cfgSecTimeoutExsufflation = secTimeoutExsufflation;
    this->_cfgLpmFluxTriggerValue = DEFAULT_LPM_FLUX_TRIGGER_VALUE;

    /* Initialize internal state */
    this->_currentState = State::Wait;
    // this->_secTimerCnt = 0;
    // this->_secTimeoutInsufflation = 0;
    // this->_secTimeoutExsufflation = 0;
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
    if (this->_cfgUpdate)
    {
        this->calcularCiclo(this->_cfgPorcentajeInspiratorio, this->_cfgRpm);
    }

    digitalWrite(5, HIGH);

    //Detecta aspiración del paciente
    if (this->hal->getPresureIns() <= _cfgLpmFluxTriggerValue)
    {
        /** @todo Pendiente desarrollo */
        stateNext();
        digitalWrite(5, LOW);
    }

    //Se lanza por tiempo
    unsigned long now = millis();
    if ((this->lastExecution + this->_cfgSecCiclo) < now)
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
    if (volumensensor >= this->_cfgmlTidalVolume || (now - this->lastExecution) >= (this->_cfgSecTimeInsufflation * 1000))
    {
        /** @todo Paramos la insuflación */
        this->stateNext();
    }
}
void MechVentilation::insuflationAfter()
{
    unsigned long now = millis();
    if ((now - this->lastExecution) >= (this->_cfgSecTimeInsufflation * 1000))
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
    if ((now - lastExecution - (this->_cfgSecTimeInsufflation * 1000)) >= (this->_cfgSecTimeExsufflation * 1000))
    {
        stateNext();
    }
    /**
     * Control de la presión de salida para prevenir baja presión PEEP
     */
    if (this->hal->getPresureExp() <= this->_cfgPresionPeep)
    {
        this->hal->valveExsClose();
    }

    //Detecta aspiración del paciente
    if (this->hal->getPresureIns() <= _cfgLpmFluxTriggerValue)
    {
        /** @todo Pendiente desarrollo */
        _setState(State::InsuflationBefore);
    }
}
void MechVentilation::exsufflationAfter()
{
    /** @todo Cerramos valvula de salida? */
    //this->hal->valveExsClose();
    stateNext();
}

void MechVentilation::calcularCiclo(
    int porcentajeInspiratorio,
    int rpm)
{
    this->_cfgSecCiclo = 60 / rpm; // Tiempo de ciclo en segundos
    this->_cfgSecTimeInsufflation = this->_cfgSecCiclo * porcentajeInspiratorio / 100;
    this->_cfgSecTimeExsufflation = this->_cfgSecCiclo - this->_cfgSecTimeInsufflation;
    Serial.println("tCiclo " + String(this->_cfgSecCiclo, DEC));
    Serial.println("T Ins " + String(this->_cfgSecTimeInsufflation, DEC));
    Serial.println("T Exs " + String(this->_cfgSecTimeExsufflation, DEC));
    this->_cfgUpdate = false;
}
