#ifndef STORAGE_H
#define STORAGE_H

#include "Arduino.h"
#include "defaults.h"
#include "strings.h"

struct ApolloConfiguration
{
  float rpm         = DEFAULT_RPM;
  float pMax        = DEFAULT_PMAX;
  float pPeak       = DEFAULT_PPEAK;
  float pPeep       = DEFAULT_PPEEP;
  float ieRatio     = DEFAULT_IERATIO;
  float iPauseRatio = DEFAULT_IPAUSERATIO;

};


struct ApolloCalibration
{

};

class ApolloStorage
{
  public:
    ApolloStorage();

    const ApolloConfiguration& getConfig()                                  {return _config;}
    void                       setConfig(const ApolloConfiguration& config) {_config = config;}

  protected:
    ApolloConfiguration _config;
    ApolloCalibration   _calibration;
};

#endif
