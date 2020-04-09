/**
 * Mechanical ventilation.
 *
 * @file MechVentilation.h
 *
 * This is the mechanical ventilation software module.
 * It handles the mechanical ventilation control loop.
 */
#ifndef INC_MECHANICAL_VENTILATION_H
#define INC_MECHANICAL_VENTILATION_H

#include <inttypes.h>
#include "defaults.h"
#include "ApolloHal.h"
#include "ApolloConfiguration.h"
#include "ApolloAlarms.h"

/** States of the mechanical ventilation. */
enum State
{
    Wait = 0, //Espera a que lance el trigger o al tiempo

    InsuflationPre = 1,
    InsufaltionProcess = 2, //Proceso mientras insufla aire
    InsuflationPost = 3,

    ExsufflationPre = 4,
    ExsufflationProcess = 5, //Proceso de exsuflación aire
    ExsufflationPost = 6,

};
/** Trigger of the proccess mechanical ventilation. */
enum Trigger
{
    Timer = 0,              //Lanzado por tiempo definido
    PattientInpiration = 1, //Lanzado por el paciente al inspirar
};

enum Mode
{
    Pressure = 0,
    Flow = 1,
    Pause = 99
};

/**
 * This is the mechanical ventilation class.
 */
class MechVentilation
{
public:
    /**
	 * Constructor
	 *
     * @param[ApolloHal]            hal                       Hal object.
     * @param[ApolloConfiguration]  config                    Configuration Object
     *
	 */
    MechVentilation(
        ApolloHal *hal,
        ApolloConfiguration *configuration,
        ApolloAlarms *alarms);

    /**,
        float secTimeoutInsufflation,
        float secTimeoutExsufflation,
        int ventilationCyle_WaitTime);*/

    /**
     * Update mechanical ventilation.
     *
     * If any control variable were to change, new value
     * would be applied at the beginning of the next ventilation
     * cycle.
     *
     * @note This method must be called on the main loop.
     */
    void update(void);
    State getStatus()  {return this->_currentState;};

private:
    /** Initialization. */
    void _init(
        ApolloHal *hal,
        int mlTidalVolume,
        int rpm,
        int porcentajeInspiratorio);

    /** Set state. */
    void _setState(State state);

    ApolloHal *hal;

    ApolloConfiguration *configuration;

    ApolloAlarms *alarms;

    /** Tidal volume in millilitres. */
    float _cfgmlTidalVolume;
    /** Respiraciones por minuto */
    int _cfgRpm;
    /** Porcentaje tiempo inspiración */
    int _cfgPorcentajeInspiratorio;

    /** Pression trigger value in litres per minute. */
    double _cfgCmh2oTriggerValue;
    /**  Ciclo time in seconds. */
    float _cfgSecCiclo;
    /**  Insufflation time in seconds. */
    float _cfgSecTimeInsufflation;
    /* Exsufflation time in seconds. */
    float _cfgSecTimeExsufflation;
    /* Presion peep (presión mínima en pulmones a la salida). CMH2O */
    double _cfgPresionPeep;
    /* Presión pico de insuflación CMH2O */
    double _cfgPresionPico;
    /* Presión meseta de insuflación CMH2O */
    double _cfgPresionMeseta;
    /* Presión Máxima de seguridad de insuflación CMH2O */
    double _cfgPresionMax;

    /** Current state. */
    State _currentState;
    /** Timer counter in seconds. */
    uint64_t _secTimerCnt;
    /**  Insufflation timeout in seconds. */
    float _secTimeoutInsufflation;
    /** Exsufflation timeout in seconds. */
    float _secTimeoutExsufflation;

    Mode mode = Mode::Pressure;
    Mode lastMode = Mode::Pressure;

    unsigned long lastExecution = 0;

    void wait();
    void insuflationPre();
    void insufaltionProcess();
    void insuflationPost();
    void exsufflationPre();
    void exsufflationProcess();
    void exsufflationPost();

    void stateNext();

    void calculateCicle();

    void configurationUpdate();
};

#endif /* INC_MECHANICAL_VENTILATION_H */
