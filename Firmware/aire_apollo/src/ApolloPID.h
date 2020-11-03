#ifndef APOLLOPID_H
#define APOLLOPID_H

#include "Arduino.h"
#include <PID_v1.h>

struct pidConstants
{
  float kp;
  float ki;
  float kd;
};

class ApolloPID
{
public:
  ApolloPID(pidConstants c)
  {
    _pidConstants = c;
    _input=0,_target=0,_output=0,_outputSUM=0,_directMode=true;
  }
  ApolloPID(double kp = 1, double ki = 0, double kd = 0)
  {
    _pidConstants.kp = kp,_pidConstants.ki = ki,_pidConstants.kd = kd;
    _input=0,_target=0,_output=0,_outputSUM=0,_directMode=true;
  }

  virtual void begin(float minLimit = 0,float maxLimit = 100,bool direct = true,float minOutput = -20,float maxOutput=20)
  {
    _minLimit  = minLimit  , _maxLimit  = maxLimit;
    _minOutput = minOutput , _maxOutput = maxOutput;
    _directMode = direct;
    initLib();
  }

  virtual void update()
  {
    pidCompute();
    if(!_directMode)
    {
      _output = constrain(_output,_minLimit,_maxLimit);
    }
    else
    {
      _outputSUM = constrain(_outputSUM+_output,_minLimit,_maxLimit); //Poner a 0 el output cuando se llege a los limites??
    }
  }

  pidConstants getConstants()               {return _pidConstants;}
  void         setConstants(pidConstants c) {_pidConstants = c;setConstantsLib();}

  float getKp()          {return _pidConstants.kp;}
  float getKi()          {return _pidConstants.ki;}
  float getKd()          {return _pidConstants.kd;}
  void   setKp(float kp) {_pidConstants.kp = kp;setConstantsLib();}
  void   setKi(float ki) {_pidConstants.ki = ki;setConstantsLib();}
  void   setKd(float kd) {_pidConstants.kd = kd;setConstantsLib();}

  double getTarget()        {return _target;}
  double getInput()         {return _input; }
  double getOutput()        {if(_directMode) return _output; else return _outputSUM;}
  void   setTarget(double t){_target = t;}
  void   setInput(double i) {_input = i; }
  void   setOutput(double o){if(_directMode) _output = o; else _outputSUM = o;}
 
protected:
  virtual void pidCompute()      {;}
  virtual void initLib()         {;}
  virtual void setConstantsLib() {;}

  pidConstants _pidConstants;
  double       _input;
  double       _target;
  double       _output;
  double       _outputSUM;
  float        _minLimit;
  float        _maxLimit;
  float        _minOutput;
  float        _maxOutput;
  bool         _directMode;
};


class pidv1lib : public ApolloPID
{
protected:
  virtual void pidCompute()
  {
    _PID->Compute();
  }

  virtual void setConstantsLib() override
  {
    _PID->SetTunings(_pidConstants.kp,_pidConstants.ki,_pidConstants.kd);
  }

  virtual void initLib() override
  {
    _PID = new PID(&_input,&_output,&_target, _pidConstants.kp,_pidConstants.ki, _pidConstants.kd, DIRECT);
    _PID->SetMode(AUTOMATIC);
    if(_directMode) _PID->SetOutputLimits (_minLimit ,_maxLimit);
    else            _PID->SetOutputLimits (_minOutput,_maxOutput);
  }

  PID*    _PID;

};

#endif
