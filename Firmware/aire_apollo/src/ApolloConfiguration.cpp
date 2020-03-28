#include "ApolloConfiguration.h"
#include "Arduino.h"

ApolloConfiguration::ApolloConfiguration(){};

bool ApolloConfiguration::begin()
{
    this->setTidalVolume(this->calcularVolumenTidal(this->height, this->sexo));
    unsigned long init = millis();
    unsigned int waitTime = 10000; //10 segundos de espera a recibir la configuración
    //Leer configuración de la tablet o sacar del epprom
    while (!this->ready)
    {
        unsigned long act = millis();
        if ((act - waitTime) > init)
        {
            break;
        }
        //Esperamos a recibir confguración o X tiempo
    }
    return true;
}

bool ApolloConfiguration::parseConfig(char *strings)
{

    this->setRpm(String(strings[1]).toFloat());
    this->setTidalVolume(String(strings[2]).toFloat());
    this->setPorcentajeInspiratorio(String(strings[3]).toFloat());
    this->setPressionPeep(String(strings[4]).toFloat());
    this->setSexo(String(strings[5]).toInt());
    this->setWeight(String(strings[6]).toFloat());
    this->setHeight(String(strings[7]).toFloat());

    Serial.println("RPM" + String(this->rpm));
    Serial.println("TidalVol" + String(this->mlTidalVolume));
    Serial.println("isnp" + String(this->porcentajeInspiratorio));
    Serial.println("peep" + String(this->pressionPeep));
    Serial.println("sexo " + this->sexo);
    Serial.println("wigth " + this->weight);
    Serial.println("height " + this->height);
    Serial.flush();

    return true;
}

void ApolloConfiguration::setRpm(int rpm)
{
    if (rpm >= 10 && rpm <= 30)
    {
        this->rpm = rpm;
        this->updated = true;
    }
}

void ApolloConfiguration::setTidalVolume(float mlTidalVolume)
{
    if (mlTidalVolume >= 300 && mlTidalVolume <= 600)
    {
        this->mlTidalVolume = mlTidalVolume;
        this->updated = true;
    }
}

void ApolloConfiguration::setPressionPeep(float pressionPeep)
{
    this->pressionPeep = pressionPeep;
    this->updated = true;
}

void ApolloConfiguration::setPorcentajeInspiratorio(int porcentajeInspiratorio)
{
    if (porcentajeInspiratorio >= 25 && porcentajeInspiratorio <= 50)
    {
        this->porcentajeInspiratorio = porcentajeInspiratorio;
        this->updated = true;
    }
}

void ApolloConfiguration::setSexo(int sexo)
{
    if (sexo == 1 || sexo == 0)
    {
        this->sexo = sexo;
        this->updated = true;
    }
}

void ApolloConfiguration::setWeight(int weight)
{
    if (weight > 0 && weight < 300)
    {
        this->weight = weight;
        this->updated = true;
    }
}

void ApolloConfiguration::setHeight(int height)
{
    if (height > 10 && height < 300)
    {
        this->height = height;
        this->updated = true;
    }
}

String ApolloConfiguration::getConfig()
{
    String params;
    params[0] = this->getRpm();
    params[1] = this->getMlTidalVolumen();
    params[2] = this->getPorcentajeInspiratorio();
    params[3] = this->getPressionPeep();
    params[4] = this->getSexo();
    params[5] = this->getHeight();
    params[6] = this->getWeight();

    return params;
}