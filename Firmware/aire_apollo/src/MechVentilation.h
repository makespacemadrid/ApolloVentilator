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

/** States of the mechanical ventilation. */
enum State
{
    Wait = 0, //Espera a que lance el trigger o al tiempo

    InsuflationBefore = 1,
    InsufaltionProcess = 2, //Proceso mientras insufla aire
    InsuflationAfter = 3,

    ExsufflationBefore = 4,
    ExsufflationProcess = 5, //Proceso de exsuflación aire
    ExsufflationAfter = 6,

};
/** Trigger of the proccess mechanical ventilation. */
enum Trigger
{
    Timer = 0,              //Lanzado por tiempo definido
    PattientInpiration = 1, //Lanzado por el paciente al inspirar
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
     * @param[ApolloHal]    hal                       Hal object.
     * @param[int]          mlTidalVolume             Tidal volume in millilitres.
	 * @param[float]        secTimeoutInsufflation    Insufflation timeout in seconds.
	 * @param[float]        secTimeoutExsufflation    Exsufflation timeout in seconds.
     *
	 */
    MechVentilation(
        ApolloHal *hal,
        int mlTidalVolume,
        int rpm,
        int porcentajeInspiratorio);
    /**,        
        float secTimeoutInsufflation,
        float secTimeoutExsufflation,
        int ventilationCyle_WaitTime);*/

    /* Setters/getters */

    // Set tidal volume
    void setTidalVolume(float mlTidalVolume);
    //Set RPM
    void setRpm(int rpm);
    //Set Porcentaje inspiratorio
    void setporcentajeInspiratorio(int porcentajeInspiratorio);
    // Set presion peep
    void setPressionPeep(float presionPeep);

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
    /**
     * @brief estima el volumen tidal en función de estatura y sexo, en ml.
     *
     * @param estatura en cm, del paciente
     * @param sexo 0: varón, 1: mujer, sexo del paciente
     * @return *volumenTidal volumen tidal estimado, en mililitros
     */
    int static MechVentilation::calcularVolumenTidal(int estatura, int sexo, float mlByKgWeight = DEFAULT_ML_POR_KG_DE_PESO_IDEAL)
    {
        float peso0, pesoIdeal, volumenEstimado;
        if (sexo == 0)
        { // Varón
            peso0 = 50.0;
        }
        else if (sexo == 1)
        { // Mujer
            peso0 = 45.5;
        }
        pesoIdeal = peso0 + 0.91 * (estatura - 152.4); // en kg

        return ((int)(round(pesoIdeal * mlByKgWeight)));
    }

private:
    /** Initialization. */
    void _init(
        ApolloHal *hal,
        int mlTidalVolume,
        int rpm,
        int porcentajeInspiratorio);

    /** Set state. */
    void _setState(State state);

    /* Configuration parameters */
    ApolloHal *hal;

    /** Tidal volume in millilitres. */
    float _cfgmlTidalVolume;
    /** Respiraciones por minuto */
    int _cfgRpm;
    /** Porcentaje tiempo inspiración */
    int _cfgPorcentajeInspiratorio;

    /** Flux trigger value in litres per minute. */
    float _cfgLpmFluxTriggerValue;
    /**  Ciclo time in seconds. */
    float _cfgSecCiclo;
    /**  Insufflation time in seconds. */
    float _cfgSecTimeInsufflation;
    /* Exsufflation time in seconds. */
    float _cfgSecTimeExsufflation;
    /* Presion peep (presión mínima en pulmones a la salida). */
    float _cfgPresionPeep;

    /* Internal state */
    /** Current state. */
    State _currentState;
    /** Timer counter in seconds. */
    uint64_t _secTimerCnt;
    /**  Insufflation timeout in seconds. */
    float _secTimeoutInsufflation;
    /** Exsufflation timeout in seconds. */
    float _secTimeoutExsufflation;
    /** Insufflation speed. @todo Denote units. */
    float _speedInsufflation;
    /** Exsufflation speed. @todo Denote units. */
    float _speedExsufflation;
    bool _running = false;
    bool _sensor_error_detected;
    bool _startWasTriggeredByPatient = false;
    float _currentVolume = 0;

    /** Estimated flux accross the bmes. @todo Denote units. */
    //float _flux;

    /* @todo PID stuff */

    int totalCyclesInThisState = 0;
    int currentTime = 0;
    int flowSetpoint = 0;

    unsigned long lastExecution = 0;

    void wait();
    void insuflationBefore();
    void insufaltionProcess();
    void insuflationAfter();
    void exsufflationBefore();
    void exsufflationProcess();
    void exsufflationAfter();

    void stateNext();

    void calcularCiclo(int porcentajeInspiratorio, int rpm);
};

#endif /* INC_MECHANICAL_VENTILATION_H */
