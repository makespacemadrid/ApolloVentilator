#include "StepperNema.h"
#include "trace.h"
StepperNema::StepperNema(uint8_t pinStep_,uint8_t pinDir_, int8_t pinMinEndstop_ , int8_t pin_Enable, bool enabledState ,uint16_t stepsPerRevolution_) :
  stepMotor(stepsPerRevolution_, pinDir_, pinStep_, pin_Enable,-1,-1,-1) ,pinMinEndstop(pinMinEndstop_),maxRPM(RPM),microSteps(MICROSTEPS)
{
  stepMotor.setEnableActiveState(enabledState);
  Serial.println("Nema- step:" +String(pinStep_) + " dir:" + String(pinDir_) + " endstop:" + String(pinMinEndstop));
}



bool StepperNema::moveAwayMinEndStop()
{
  int counter = 0;
  while(counter < stepsMax * 0.1)
  {
    stepMotor.move(1);
    counter++;
    //delay(1);
  }

  if(isMinEndStopPressed())
  {
      Serial.println("Cannot move away min endstop!");
      return false;
  }
  else return true;
}

bool StepperNema::moveTowardsMinEndStop()
{
  int counter = 0;
  while(!isMinEndStopPressed() && counter < stepsMax)
  {
    stepMotor.move(-1);
    counter++;
    //delay(1);
  }

  if(!isMinEndStopPressed())
  {
      Serial.println("Cannot hit min endstop!");
      return false;
  }
  else return true;
}

uint16_t StepperNema::countStepsToHome()
{
  uint16_t count = 0;
  while(!isMinEndStopPressed() && count < stepsMax)
  {
    stepMotor.move(-1);
    count++;
    //delay(1);
  }
  return count;
}

bool StepperNema::home()
{
  Serial.print("HOMING....");
  if(pinMinEndstop < 0)
  {
    Serial.println("No endStop!");
    return true; //Nothing to do here, si no hay final de carrera le damos hacia el 0 unos segundos para que choque?
  }

  int endStopHit = 0;
  if(isMinEndStopPressed()) // Si Ya estamos pisando el final de carrera nos retiramos un poco
  {
    Serial.print("RETRACT...");
    endStopHit++;
    if(!moveAwayMinEndStop())
    {
      Serial.println("FAIL!");
      return false; //Si despues de retirarnos no hemos dejado de pisar el endstop reportamos error
    }
  }

  Serial.print("GO FOR ENDSTOP...");
  if(moveTowardsMinEndStop()) // Nos acercamos al final de carrera!
  {
    endStopHit++;
  }
  else
  {
    Serial.println("FAIL!");
    return false; //Si despues de retirarnos no hemos dejado de pisar el endstop reportamos error
  }

  if(endStopHit == 2) //Si inicialmente estabamos en el final de carrera ya lo hemos golpeado dos veces y hemos terminado
  {
    Serial.println("OK!");
    return true;
  }
  else
  {
    Serial.print("RETRACT...");
    if(!moveAwayMinEndStop())
    {
      Serial.println("FAIL!");
      return false; //Si despues de retirarnos no hemos dejado de pisar el endstop reportamos error
    }
    Serial.print("GO FOR ENDSTOP...");
    if(moveTowardsMinEndStop()) // Nos acercamos al final de carrera!
    {
      Serial.println("OK!");
      return true;
    }
  }

  Serial.println("FAIL!");
  return false;

}

double StepperNema::status()
{
  //return target();
  double p = 0;
//  if(openPos > closePos)
    p = map(lastPos,closePos,openPos,0,100);
//  else
//    p = map(this->lastPos,openPos,closePos,0,100);
    return p;
}

double StepperNema::target()
{
  return percent;
}

bool StepperNema::test()
{

  Serial.print("TESTING....");
  uint32_t maxPos;
//  uint32_t minPos;




for(int t = 0 ; t < 5 ; t++)
{
  open(100,true);
  delay(100);
  close(true);
  delay(100);
}
if(openPos > closePos)
{
  maxPos = openPos;
//    minPos = maxPos;
  open(100,true);
}
else
{
  maxPos = closePos;
//    minPos = openPos;
    close(true);
}

//  stepMotor.move(maxPos);
//  stepMotor.move(-maxPos);
//  stepMotor.move(maxPos);
//  stepMotor.move(-maxPos);
//  stepMotor.move(maxPos);
//  stepMotor.move(-maxPos);
//  stepMotor.move(maxPos);
//  stepMotor.move(-maxPos);
//  stepMotor.move(maxPos);
//  stepMotor.move(-maxPos);
//  stepMotor.move(maxPos);
//  stepMotor.move(-maxPos);
//  stepMotor.move(maxPos);


  if(pinMinEndstop < 0)
  {
    Serial.println("No endStop!");
    return true; //Nothing to do here
  }

  uint32_t stepsBackToHome = countStepsToHome();
  Serial.println("STEPS: "+String(maxPos) + " BACK: " + String(stepsBackToHome));
  int32_t d = maxPos-stepsBackToHome;
  uint32_t difference = abs(d);
  uint32_t maxError = stepsMax * MAX_STEPPER_ERROR;
  Serial.println("diff: "+String(difference) + " max: " + String(maxError));

  if(difference > maxError)
  {
    Serial.println("UNRELIABLE STEPPER");
    return false;
  }
  else
  {
    lastPos = 0;
    return true;
  }

}

bool StepperNema::begin()
{
  if(openPos > closePos)
    this->stepsMax = openPos*1.1;
  else
    this->stepsMax = closePos*1.1;

    stepMotor.begin(maxRPM,microSteps);
    Serial.println("Nema- MaxRPM  maxrpm:" + String(maxRPM) + " microsteps:" + String(microSteps));
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    this->stepMotor.enable();
    stepMotor.setSpeedProfile(stepMotor.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL); //Probar bien antes de activar

    if(this->pinMaxEndstop) pinMode(this->pinMaxEndstop,maxEndstopPinMode);
    if(this->pinMinEndstop)
      pinMode(this->pinMinEndstop,minEndstopPinMode);
    else
      return true; //Si no hay final de carrera de minimo no hacemos nada mas.

    if(home())
    {//Comprobacion del mecanismo. Desde el final de carrera hasta la posicion maxima ida y vuelta varias veces.
    //Los pasos para ir y para volver deben de ser (casi) los mismo o el mecanismo no esta bien
      //return true;
      lastPos = 0;
      return true;
    }
    else
    {
      Serial.println("Stepper Homing error!");
      return false;
    }
}

bool StepperNema::calibrate()
{
  /*
  if(this->pinFcIni != 0 && this->pinFcEnd != 0){
    while(digitalRead(this->pinFcIni) != LOW){
      //Desplazamos el stepper al inicio
      this->stepMotor.move(-1);
    }
    this->lastStep = 0;
    while(digitalRead(this->pinFcIni) != LOW){
      //Desplazamos el stepper al final para autocalibrado
      this->stepMotor.move(1);
      this->lastStep++;
    }
    this->stepsMax = this->lastStep;
  }
*/
  return true;
}

void StepperNema::open(double openPercent, bool wait)
{
//
    blockUpdate = true;
//    this->stepMotor.stop();
//    Serial.println("NEMA-OPEN");Serial.flush();
    this->percent = constrain(openPercent,0.0,100.0);
//    this->stepDestination = (this->stepsMax * (percent / 100))+this->startPos;
//    Serial.println("NEMA-PERCENT"+String(percent));Serial.flush();
//    if(openPos > closePos)
      this->stepDestination = map(this->percent,0,100,closePos,openPos);
//    else
//      this->stepDestination = map(this->percent,0,100,closePos,openPos);
//    Serial.println("NEMA-DESTINATION:"+String(stepDestination));Serial.flush();
     int32_t mover = this->stepDestination - this->lastPos;
     if(mover>0){
        this->lastDir=1; //derechas
      }else{
        this->lastDir=0; //izquierdas
      }
//    Serial.println("NEMA-MOVER:"+String(mover));Serial.flush();

    if(wait)
    {
      this->stepMotor.move(mover);
      lastPos = stepDestination;
    }
    else
    {
      this->stepMotor.startMove(mover);
    }

    blockUpdate = false;
}

void StepperNema::close(bool wait)
{
  this->open(0,wait);
}

void StepperNema::update(){
//check limits!!!!!
/*
  if(this->pinFcIni != 0 && isMinEndStopPressed() && !this->lastDir){
    stepMotor.stop();
  }

  if(this->pinFcEnd != 0 && isMaxEndStopPressed() && this->lastDir){
    stepMotor.stop();
  }
*/
}


void StepperNema::highFreqUpdate(){

  //if(nextActionTime - millis() > 30) return;
  if(blockUpdate) return;
  //Serial.println("UPDATE STTEPER");Serial.flush();
  blockUpdate = true;
  //interrupts();//WTF! Sin esto se bloquea por que sin interrupciones no funciona delay y la clase del stepper llama a un delay si todavia no es tiempo de dar un paso.
  if(stepMotor.nextAction() < 1)
  {
    blockUpdate = false;
    return;
  }
  else
  {
    if(this->lastDir){
      this->lastPos++;
    }else{
      this->lastPos--;
    }
  }
  blockUpdate = false;
//  Serial.println("stepper->"+String(this->stepDestination)+","+String(this->lastDir)+","+String(this->lastStep));
}
