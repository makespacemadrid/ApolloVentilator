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
#include "ApolloConfiguration.h"
#include "ApolloAlarms.h"

MechVentilation::MechVentilation(
    ApolloHal *hal,
    ApolloConfiguration *configuration,
    ApolloAlarms *alarms)
{
    this->hal = hal;
    this->configuration = configuration;
    this->configurationUpdate();
    this->_currentState = State::Wait;
    this->alarms = alarms;
}

void MechVentilation::update(void)
{
    if (this->hal->getPresureIns() > DEFAULT_CMH20_MAX)
    {
        // @todo Alerta por sobrepresion
        this->hal->valveInsClose();
        this->alarms->critical(12, "Presion máxima alcanzada");
    }

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
    if (this->configuration->isUpdated())
    {
        this->configurationUpdate();
    }

    //Detecta aspiración del paciente
    if (this->hal->getPresureIns() <= this->_cfgCmh2oTriggerValue)
    {
        /** @todo Pendiente desarrollo */
        stateNext();
    }

    //Se lanza por tiempo
    unsigned long now = millis();
    if ((this->lastExecution + this->_cfgSecCiclo) < now)
    {
        stateNext();
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
    this->hal->intakeFlowSensor()->resetFlow();
    this->hal->valveExsClose();
    this->hal->valveInsOpen();
    this->stateNext();
}
void MechVentilation::insufaltionProcess()
{
    //El proceso de insuflación está en marcha, esperamos al sensor de medida o tiempo de insuflación max
    /** @todo conectar sesor ml/min */
    unsigned long now = millis();
    switch (this->mode)
    {
    case Mode::Presion:
        if (this->hal->getPresureIns() > this->_cfgPresionPico)
        {
            this->alarms->info(14, "Alcanzada presión pico ");
            this->hal->valveInsClose();
        }
        break;
    case Mode::Flow:
#ifdef INTFLOWSENSOR
        if (this->hal->intakeFlowSensor()->getFlow() >= this->_cfgmlTidalVolume)
        {
            this->stateNext();
        }
#endif
        break;
    }
    if ((now - this->lastExecution) >= (this->_cfgSecTimeInsufflation * 1000))
    {
        this->stateNext();
    }
}
void MechVentilation::insuflationAfter()
{
    if (this->hal->getPresureExp() < this->_cfgPresionPico)
    {
        this->hal->valveExsClose();
        this->alarms->info(14, "No llegamos a presión Pico");
    }

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
    if (this->hal->getPresureExp() <= (this->_cfgPresionPeep - 1) && !this->hal->getValveExsState())
    {
        this->hal->valveExsClose();
        this->alarms->info(13, "Presión debajo de PEEP");
    }

    //Detecta aspiración del paciente
    if (this->hal->getPresureIns() <= this->_cfgCmh2oTriggerValue)
    {
        /** @todo Pendiente desarrollo */
        _setState(State::InsuflationBefore);
        this->alarms->info(99, "Aspiración del paciente");
    }
}
void MechVentilation::exsufflationAfter()
{
    if (this->hal->getPresureExp() < (this->_cfgPresionPeep - 1) && !this->hal->getValveExsState())
    {
        this->hal->valveExsClose();
        this->alarms->info(13, "Presión debajo de PEEP");
    }
    /** @todo Cerramos valvula de salida? */
    //this->hal->valveExsClose();
    stateNext();
}

void MechVentilation::calcularCiclo()
{
    this->_cfgSecCiclo = 60 / this->_cfgRpm; // Tiempo de ciclo en segundos
    this->_cfgSecTimeInsufflation = this->_cfgSecCiclo * this->_cfgPorcentajeInspiratorio / 100;
    this->_cfgSecTimeExsufflation = this->_cfgSecCiclo - this->_cfgSecTimeInsufflation;
    Serial.println("_cfgSecCiclo " + String(this->_cfgSecCiclo, DEC));
    Serial.println("_cfgSecTimeInsufflation " + String(this->_cfgSecTimeInsufflation, DEC));
    Serial.println("_cfgSecTimeExsufflation " + String(this->_cfgSecTimeExsufflation, DEC));
    Serial.flush();
}

void MechVentilation::configurationUpdate()
{
    this->_cfgmlTidalVolume = this->configuration->getMlTidalVolumen();
    this->_cfgPorcentajeInspiratorio = this->configuration->getPorcentajeInspiratorio();
    this->_cfgRpm = String(this->configuration->getRpm()).toInt();
    this->_cfgCmh2oTriggerValue = this->configuration->getPresionTriggerInspiration();
    this->_cfgPresionPeep = this->configuration->getPressionPeep();
    Serial.println("_cfgRpm " + String(this->_cfgRpm));
    Serial.println("_cfgmlTidalVolume " + String(this->_cfgmlTidalVolume));
    Serial.println("_cfgPorcentajeInspiratorio " + String(this->_cfgPorcentajeInspiratorio));
    Serial.println("_cfgCmh2oTriggerValue " + String(this->_cfgCmh2oTriggerValue));
    Serial.println("_cfgPresionPeep " + String(this->_cfgPresionPeep));
    Serial.flush();

    this->calcularCiclo();
    this->configuration->resetUpdated();
}
