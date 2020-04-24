#include "ApolloHal.h"
#include <Arduino.h>
#define DEBUG
#include "trace.h"

ApolloHal::ApolloHal(ApolloPressureSensor *preSensor, ApolloFlowSensor *entryFlowSensor, ApolloFlowSensor *exitFlowSensor, ApolloValve *entryEV, ApolloValve *exitEV, ApolloAlarms *alarms) :
    pidPressureIns_ (&this->currentPressureIns_, &this->statusPressureIns_, &this->pressureInsTarget_, c_consKp, c_consKi, c_consKd, DIRECT),
    pidPressureExs_ (&this->currentPressureExs_, &this->statusPressureExs_, &this->pressureExsTarget_, c_consKp, c_consKi, c_consKd, REVERSE),
    pidFlowIns_     (&this->currentFlowIns_    , &this->statusFlowIns_    , &this->flowInsTarget_    , c_consKp, c_consKi, c_consKd, DIRECT)
//pidPressureIns_ (&this->currentPressureIns_, &this->pressureInsTarget_,&this->statusPressureIns_,0,100, c_consKp, c_consKi, c_consKd),
//pidPressureExs_ (&this->currentPressureExs_, &this->pressureExsTarget_,&this->statusPressureExs_,0,100, c_consKp, c_consKi, c_consKd),
//pidFlowIns_     (&this->currentFlowIns_    , &this->flowInsTarget_    ,&this->statusFlowIns_    ,0,100, c_consKp, c_consKi, c_consKd)
{
  this->entryPressureSensor_ = preSensor;
  this->entryFlowSensor_ = entryFlowSensor;
  this->exitFlowSensor_ = exitFlowSensor;
  this->entryEV_ = entryEV;
  this->exitEV_ = exitEV;
  this->alarms_ = alarms;
  this->initializePidFlowIns();
  this->initializePidPressureIns();
  this->initializePidPressureExs();
}

ApolloHal::~ApolloHal()
{
}

/**
 * @brief Initialize and check all sensor
 *
 * @return true if all sensor are present a initilized
 * @return false if something goes wrong
 */
bool ApolloHal::begin()
{

  bool status = true;

  if (!this->entryFlowSensor_->begin())
  {
    TRACE("ERROR FLOW-IN!");
    status = false;
  }

  if (!this->exitFlowSensor_->begin())
  {
    TRACE("ERROR FLOW-OUT");
    status = false;
  }

  if (!this->exitEV_->begin())
  {
    TRACE("ERROR VALVE-OUT");
    status = false;
    return false;
  }

  exitEV_->waitOpen();

  if (!this->entryEV_->begin())
  {
    TRACE("ERROR VALVE-IN");
    status = false;
    return false;
  }

  entryEV_->waitClose();
  delay(1000);

  TRACE("VERIFY PRESION!");
  // Close Entry Valve and open exit valve and wait 3 sec to empty the pressure in the system
  // This it's neccesary to reset 0 the presure in the sensor

  //this->entryEV_->close();
  //delay(1000);
  //this->exitEV_->open();
  //delay(3000);

  if (!this->entryPressureSensor_->begin())
  {
    TRACE("ERROR PRESION!");
    status = false;
  }

  return status;
}

void ApolloHal::ISR1ms()
{
  this->entryFlowSensor_->update();
  this->exitFlowSensor_->update();
  this->entryPressureSensor_->update();
  this->entryEV_->update();
  this->exitEV_->update();

}

void ApolloHal::setFlow(float flow, float pressure)
{
}


/**
 * Inspiration pressure
 * @param bool cache Return value of cache or not; Default false;
 *
 */
double ApolloHal::getPresureIns(bool cache)
{
  if (cache)
  {
    return this->currentPressureIns_;
  }
  else
  {
    return this->entryPressureSensor_->readCMH2O();
  }
}
/**
 * Exsiration pressure
 * @param bool cache Return value of cache or not; Default false;
 *
 */
double ApolloHal::getPresureExs(bool cache)
{
  if (cache)
  {
    return this->currentPressureExs_;
  }
  else
  {
    return this->entryPressureSensor_->readCMH2O();
  }
}

void ApolloHal::valveInsOpen(uint8_t percent)
{
  this->entryEV_->open(percent);
}

void ApolloHal::valveInsClose()
{
  this->enablePressureIns_ = false;
  this->entryEV_->close();
}

void ApolloHal::valveExsOpen(uint8_t percent)
{
  this->exitEV_->open(percent);
}

void ApolloHal::valveExsClose()
{
  this->enablePressureExs_ = false;
  this->exitEV_->close();
}



void ApolloHal::initializePidPressureIns()
{
  this->pidPressureIns_.SetTunings(this->c_consKp, this->c_consKi, this->c_consKd);
  this->pidPressureIns_.SetMode(AUTOMATIC);
//  pidPressureIns_.setBangBang(25);
//  pidPressureIns_.setTimeStep(2);
  //this->pidPressureIns_.setOutputRange(0.0, 100.0);
}
void ApolloHal::initializePidPressureExs()
{
  this->pidPressureExs_.SetTunings(c_consKp, this->c_consKi, this->c_consKd);
  this->pidPressureExs_.SetMode(AUTOMATIC);
  //pidPressureExs_.setBangBang(25);
  //pidPressureExs_.setTimeStep(2);
  //this->pidPressureIns_.setOutputRange(0.0, 100.0);
}
void ApolloHal::initializePidFlowIns()
{
//  this->pidFlowIns_.SetTunings(this->c_consKp, this->c_consKi, this->c_consKd);
//  this->pidFlowIns_.SetMode(AUTOMATIC);
}

void ApolloHal::setPressureInsTarget(double pressure)
{
//    Serial.println("SetPressure : " + String(pressure));Serial.flush();
  this->enablePressureIns_ = true;
  this->pressureInsTarget_ = pressure;
}
void ApolloHal::setPressureExsTarget(double pressure)
{
  this->enablePressureExs_ = true;
  this->pressureExsTarget_ = pressure;
}
void ApolloHal::setFlowInsTarget(double flow)
{
  this->enableFlowIns_ = true;
  this->flowInsTarget_ = flow;
}

void ApolloHal::updateSensors()
{
  this->currentPressureIns_ = this->getPresureIns(false);
  this->currentPressureExs_ = this->currentPressureIns_;
  this->statusPressureIns_  = this->getEntryValveTarget();
//  this->statusPressureExs_  = (100-this->exitEV_->status()); //OJO esto tiene que cuadrar con el pid compute!!
  this->statusPressureExs_  = this->getExitValveTarget();
  //Serial.println(this->statusPressureExs_);
  //this->entryEV_->update();
  //this->exitEV_->update();
}

void ApolloHal::pidCompute()
{
//  Serial.println("Update pids");Serial.flush();
  this->pidPressureInsCompute();
  this->pidPressureExsCompute();
  this->pidFlowInsCompute();
}

void ApolloHal::pidPressureInsCompute()
{
  if (!this->enablePressureIns_)
  {
    return;
  }
//  float aggKp   = 2  , aggKi  = 0.00 , aggKd  = 0.5;
    float consKp  = 1.10  ,  consKi = 0.01, consKd = 0.05;

//  double gap = abs(this->pressureInsTarget_ - this->getPresureIns(true)); //distance away from setpoint
//  if (gap < 5)
//  { //we're close to setpoint, use conservative tuning parameters
    this->pidPressureIns_.SetTunings(consKp, consKi, consKd);
//  }
//  else
//  {
    //we're far from setpoint, use aggressive tuning parameters
//    this->pidPressureIns_.SetTunings(aggKp, aggKi, aggKd);
//  }
  this->pidPressureIns_.Compute();
//  pidPressureIns_.run();

  this->statusPressureIns_ = constrain(this->statusPressureIns_,0.0,100.0);
  this->entryEV_->open(this->statusPressureIns_);

//  Serial.println("pidPressureIns: current:" + String(this->currentPressureIns_) + " Target:" + String(this->pressureInsTarget_) + " Output:" + String(this->statusPressureIns_));
}

void ApolloHal::pidPressureExsCompute()
{
  if (!this->enablePressureExs_)
  {
    return;
  }

//  float aggKp = 5    , aggKi = 0.0 , aggKd = 0;
  float consKp = 15, consKi = 0.0, consKd = 0;

//  double gap = abs(this->pressureExsTarget_ - this->getPresureExs(true)); //distance away from setpoint
//  if (gap < 10)
//  { //we're close to setpoint, use conservative tuning parameters
    this->pidPressureExs_.SetTunings(consKp, consKi, consKd);
//  }
//  else
//  {
    //we're far from setpoint, use aggressive tuning parameters
//    this->pidPressureExs_.SetTunings(aggKp, aggKi, aggKd);
//  }

  this->pidPressureExs_.Compute();
//  pidPressureExs_.run();
 // this->pidPressureExs_.run();
  this->statusPressureExs_ =  constrain(this->statusPressureExs_,0.0,100.0);
//  this->exitEV_->open(100-this->statusPressureExs_); //OJO esto tiene que cuadrar con el update sensors!!!
  this->exitEV_->open(statusPressureExs_);

}

void ApolloHal::pidFlowInsCompute()
{
  if (!this->enableFlowIns_)
  {
    return;
  }

//  float aggKp =  5  , aggKi = 0.0, aggKd = 0;
//  float consKp = 1  , consKi = 0.0,consKd = 0;

//  double gap = abs(this->flowInsTarget_ - this->getPresureIns(true)); //distance away from setpoint
//  if (gap < 10)
//  { //we're close to setpoint, use conservative tuning parameters
//    this->pidFlowIns_.SetTunings(consKp, consKi, consKd);
//  }
//  else
//  {
//    //we're far from setpoint, use aggressive tuning parameters
//    this->pidFlowIns_.SetTunings(aggKp, aggKi, aggKd);
//  }

  this->pidFlowIns_.Compute();
  //this->pidFlowIns_.run();
  this->statusFlowIns_ = constrain(this->statusFlowIns_,0.0,100.0);
  this->entryEV_->open(this->statusFlowIns_);
  //Serial.println("pidFlowIns: current:" + String(this->currentFlowIns_) + " Target:" + String(this->flowInsTarget_) + " Output:" + String(this->statusFlowIns_));
}
