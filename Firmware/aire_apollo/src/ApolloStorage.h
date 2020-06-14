#ifndef STORAGE_H
#define STORAGE_H

#include "Arduino.h"
#include "defaults.h"
#include "strings.h"
#include "ApolloPID.h"


enum ventilatorMode
{
    modeAssist,
    modePressure,
    modeFlow
};

enum ventilatorStatus
{
  ventilatorStopped,
  ventilatorRunning,
  ventilatorPaused
};

struct ApolloConfiguration
{
  ventilatorMode    mode;
  ventilatorStatus  status;
  float             rpm;
  float             pMax;
  float             pPeak;
  float             pPeep;
  float             ieRatio;
  float             iPauseRatio;

};


struct ApolloCalibration
{
  bool         calibrated = false;
  pidConstants overPressurePID;
  pidConstants constantPressurePID;
  float        flowToValvePercent[10];
  uint8_t      valveCloseTime[10];
};



class ApolloStorage
{
  public:
    ApolloStorage();

    void begin()
    {
      if(!loadConfig())
        _config = defaultConfig();
      if(!loadCalibration())
        _calibration = defaultCalibration();
    }

    ApolloConfiguration getConfig()                    {return _config;}
    void  setConfig(const ApolloConfiguration& config) {_config = config;}

    ApolloCalibration getCalibration()                 {return _calibration;}
    void  setCalibration(const ApolloCalibration& cal) {_calibration = cal;}

    ApolloConfiguration defaultConfig()
    {
      ApolloConfiguration result;
      result.mode        = modeAssist;
      result.status      = ventilatorStopped;
      result.rpm         = DEFAULT_RPM;
      result.pMax        = DEFAULT_PMAX;
      result.pPeak       = DEFAULT_PPEAK;
      result.pPeep       = DEFAULT_PPEEP;
      result.ieRatio     = DEFAULT_IERATIO;
      result.iPauseRatio = DEFAULT_IPAUSERATIO;
      return result;
    }

    ApolloCalibration defaultCalibration()
    {
      ApolloCalibration result;
      result.calibrated          = false;
      result.overPressurePID     = {5,0,0};
      result.constantPressurePID = {5,0,0};
      return result;
    }

    virtual bool factoryReset()    {return false;}
    virtual bool loadConfig()      {return false;}
    virtual bool saveConfig()      {return false;}
    virtual bool loadCalibration() {return false;}
    virtual bool saveCalibration() {return false;}

  protected:
    ApolloConfiguration _config;
    ApolloCalibration   _calibration;
};

#endif
