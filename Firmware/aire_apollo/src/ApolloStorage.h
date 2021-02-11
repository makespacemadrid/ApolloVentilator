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

enum ventilationStatus
{
  ventilationIdle,
  ventilationTriggerInspiration,
  ventilationInspiration,
  ventilationInspirationPause,
  ventilationExpiration
};

enum ventilatorStatus
{
  ventilatorStopped,
  ventilatorRunning,
  ventilatorPaused,
  ventilatorError
};

enum hardwareStatus
{
  hardwareUNKNOWN,
  hardwareOK,
  hardwareUNCAL,
  hardwareERROR,
  hardwareTESTING,
  hardwareCALIBRATION
};


struct ApolloConfiguration
{
  ventilatorMode    mode;       // Working mode
  ventilatorStatus  status;     // Stop, Running....
  float             rpm;        // Respirations per minute
  float             pMax;       // Max pressure
  float             pPeak;      // Peak pressure
  float             pPeep;      // Peep pressure
  float             pTrigger;   // Trigger pressure
  float             vTidal;     // Tidal volume
  float             ieRatio;
  float             iPause;     // Inspiratory pause

};


struct ApolloCalibration
{
  bool         calibrated = false;
  float        maxSupplyFlow;
  pidConstants overPressurePID;
  pidConstants constantPressurePID;
  pidConstants constantFlowPID;
  pidConstants inspiratoryRisePID;
  pidConstants expiratoryPID;

  float        flowCalibrationTable[CALIBRATION_TABLE_RESOLUTION];
  uint16_t     valveCloseTimeTable [CALIBRATION_TABLE_RESOLUTION];
};



class ApolloStorage
{
  public:
    ApolloStorage()
    {

    }

    void begin()
    {
      if(!loadConfig())
        _config = defaultConfig();
      if(!loadCalibration())
        _calibration = defaultCalibration();
    }

    ApolloConfiguration getConfig()                    {return _config;}
    void  setConfig(const ApolloConfiguration& config) {_config = config;}
    ApolloConfiguration setConfig()                   {return _config;}

    ApolloCalibration getCalibration()                 {return _calibration;}
    void  setCalibration(const ApolloCalibration& cal) {_calibration = cal;}
    ApolloCalibration setCalibration()                {return _calibration;}

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
      result.iPause      = DEFAULT_IPAUSERATIO;
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
