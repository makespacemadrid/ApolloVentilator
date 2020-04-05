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
    this->hal->updateSensors();
    if (this->hal->getPresureIns(true) > DEFAULT_CMH20_MAX)
    {
        // @todo Alerta por sobrepresion
        this->hal->valveExsOpen();
        this->hal->valveInsClose(); 
        this->alarms->critical(12, "Presion máxima alcanzada");
    }

    switch (this->_currentState)
    {
    case State::Wait:
        this->wait();
        break;
    case State::InsuflationPre:
        this->insuflationPre();
        break;
    case State::InsufaltionProcess:
        this->insufaltionProcess();
        break;
    case State::InsuflationPost:
        this->insuflationPost();
        break;
    case State::ExsufflationPre:
        this->exsufflationPre();
        break;
    case State::ExsufflationProcess:
        this->exsufflationProcess();
        break;
    case State::ExsufflationPost:
        this->exsufflationPost();
        break;
    }
    this->hal->pidCompute();
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
        this->_currentState = State::InsuflationPre;
        break;
    case State::InsuflationPre:
        this->_currentState = State::InsufaltionProcess;
        break;
    case State::InsufaltionProcess:
        this->_currentState = State::InsuflationPost;
        break;
    case State::InsuflationPost:
        this->_currentState = State::ExsufflationPre;
        break;
    case State::ExsufflationPre:
        this->_currentState = State::ExsufflationProcess;
        break;
    case State::ExsufflationProcess:
        this->_currentState = State::ExsufflationPost;
        break;
    case State::ExsufflationPost:
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
void MechVentilation::insuflationPre()
{
    unsigned long now = millis();
    this->lastExecution = now; // Remapear la logica con void updateExecutionTime() y uint16_t timeSinceLastExecution() , asi dentro de esas funciones se puede gestionar el desbordamiento del contador (~47dias de ejecucion)
    /**
     *  @todo Decir a la válvula que se abra
    */
    this->hal->resetEntryFlow();
    this->hal->valveExsClose();
    this->hal->setPressureInsTarget(this->_cfgPresionPico);
    this->stateNext();
}

void MechVentilation::insufaltionProcess()
{
    //El proceso de insuflación está en marcha, esperamos al sensor de medida o tiempo de insuflación max
    /** @todo conectar sesor ml/min */
    unsigned long now = millis();
    switch (this->mode)
    {
    case Mode::Pressure:
        this->hal->setPressureInsTarget(this->_cfgPresionPico);
        break;
    case Mode::Flow:
        if (this->hal->getEntryFlow() >= this->_cfgmlTidalVolume)
        {
            this->stateNext();
        }
        break;
    }
    if ((now - this->lastExecution) >= (this->_cfgSecTimeInsufflation * 1000))
    {
        this->stateNext();
    }
}

void MechVentilation::insuflationPost()
{
    if (this->hal->getPresureExs(true) < this->_cfgPresionPico)
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

void MechVentilation::exsufflationPre()
{
    /** @todo Abrimos válvulas de salida */
    this->hal->valveExsOpen(this->_cfgPresionPeep);
    stateNext();
}

void MechVentilation::exsufflationProcess()
{
    unsigned long now = millis();
    if ((now - lastExecution - (this->_cfgSecTimeInsufflation * 1000)) >= (this->_cfgSecTimeExsufflation * 1000))
    {
        stateNext();
    }

    //Detecta aspiración del paciente
    if (this->hal->getPresureIns(true) <= this->_cfgCmh2oTriggerValue)
    {
        /** @todo Pendiente desarrollo */
        _setState(State::InsuflationPre);
        this->alarms->info(99, "Aspiración del paciente");
    }
}
void MechVentilation::exsufflationPost()
{
    if (this->hal->getPresureExs(true) < (this->_cfgPresionPeep - 1) && !this->hal->getExitValveStatus())
    {
        this->hal->valveExsClose();
        this->alarms->info(13, "Presión debajo de PEEP");
    }
    /** @todo Cerramos valvula de salida? */
    //this->hal->valveExsClose();
    stateNext();
}

void MechVentilation::calculateCicle()
{
    this->_cfgSecCiclo = 60 / this->_cfgRpm; // Tiempo de ciclo en segundos
    this->_cfgSecTimeInsufflation = this->_cfgSecCiclo * this->_cfgPorcentajeInspiratorio / 100;
    this->_cfgSecTimeExsufflation = this->_cfgSecCiclo - this->_cfgSecTimeInsufflation;
#ifdef DEBUG
    Serial.println("_cfgSecCiclo "            + String(this->_cfgSecCiclo, DEC));
    Serial.println("_cfgSecTimeInsufflation " + String(this->_cfgSecTimeInsufflation, DEC));
    Serial.println("_cfgSecTimeExsufflation " + String(this->_cfgSecTimeExsufflation, DEC));
    Serial.flush();
#endif // DEBUG
}

void MechVentilation::configurationUpdate()
{
    this->_cfgmlTidalVolume = this->configuration->getMlTidalVolumen();
    this->_cfgPorcentajeInspiratorio = this->configuration->getPorcentajeInspiratorio();
    this->_cfgRpm = String(this->configuration->getRpm()).toInt();
    this->_cfgCmh2oTriggerValue = this->configuration->getPresionTriggerInspiration();
    this->_cfgPresionPeep = this->configuration->getPressionPeep();
    this->_cfgPresionMax = this->configuration->getPressionMax();
    this->_cfgPresionMeseta = this->configuration->getPressionMeseta();
    this->_cfgPresionPico = this->configuration->getPressionPico();
    Serial.println("_cfgRpm " + String(this->_cfgRpm));
    Serial.println("_cfgmlTidalVolume " + String(this->_cfgmlTidalVolume));
    Serial.println("_cfgPorcentajeInspiratorio " + String(this->_cfgPorcentajeInspiratorio));
    Serial.println("_cfgCmh2oTriggerValue " + String(this->_cfgCmh2oTriggerValue));
    Serial.println("_cfgPresionPeep " + String(this->_cfgPresionPeep));
    Serial.println("_cfgPresionMeseta " + String(this->_cfgPresionMeseta));
    Serial.println("_cfgPresionPico " + String(this->_cfgPresionPico));
    Serial.println("_cfgPresionMax " + String(this->_cfgPresionMax));

    Serial.flush();
    this->calculateCicle();
    this->configuration->resetUpdated();
}
