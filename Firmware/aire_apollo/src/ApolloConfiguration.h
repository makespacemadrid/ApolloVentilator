#ifndef _APOLLO_CONFIGURATION_H
#define _APOLLO_CONFIGURATION_H

#include "../include/defaults.h"
#include "Arduino.h"

class ApolloConfiguration
{
public:
    ApolloConfiguration();
    int getMlTidalVolumen() { return this->mlTidalVolume; };
    int getPressionPeep() { return this->pressionPeep; };
    int getPorcentajeInspiratorio() { return this->porcentajeInspiratorio; };
    int getPorcetajeFiO2() { return this->porcentajeFiO2; };
    int getRpm() { return this->rpm; };
    int getLpmTriggerInspiration() { return this->lpmTriggerInspiration; };
    int getSexo() { return this->sexo; };
    int getWeight() { return this->weight; };
    int getHeight() { return this->height; };

    int update() { return this->updated; }; //Return true if the configuration has been updated
    bool begin();
    // Set tidal volume
    void setTidalVolume(float mlTidalVolume);
    //Set RPM
    void setRpm(int rpm);
    //Set Porcentaje inspiratorio
    void setPorcentajeInspiratorio(int porcentajeInspiratorio);
    // Set presion peep
    void setPressionPeep(float presionPeep);

    void setSexo(int sexo);

    void setWeight(int weight);

    void setHeight(int height);

    String getConfig();
    bool parseConfig(char *strings);
    /**
     * @brief estima el volumen tidal en función de estatura y sexo, en ml.
     *
     * @param estatura en cm, del paciente
     * @param sexo 0: varón, 1: mujer, sexo del paciente
     * @return *volumenTidal volumen tidal estimado, en mililitros
     */
    int static calcularVolumenTidal(int estatura, int sexo, float mlByKgWeight = DEFAULT_ML_POR_KG_DE_PESO_IDEAL)
    {
        float peso0, pesoIdeal;
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
    int weight = DEFAULT_WEIGHT;
    int height = DEFAULT_HEIGHT;
    int sexo = DEFAULT_GENDER;
    int rpm = DEFAULT_RPM;
    int mlTidalVolume;
    int pressionPeep = DEFAULT_CMH20_PEEP;
    int porcentajeInspiratorio = DEFAULT_POR_INSPIRATORIO;
    int porcentajeFiO2 = DEFAULT_POR_FIO2;
    int lpmTriggerInspiration = DEFAULT_LPM_FLUX_TRIGGER_VALUE;
    void calcularCiclo();

    float secCiclo = 0;
    float secTimeInsufflation = 0;
    float secTimeExsufflation = 0;

    bool updated = false;
    bool ready = false;
};

#endif //_APOLLO_CONFIGURATION_H