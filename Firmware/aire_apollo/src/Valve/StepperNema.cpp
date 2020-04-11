#include "StepperNema.h"
#include "trace.h"
StepperNema::StepperNema(uint8_t pinEnable_,uint8_t pinDir_,uint8_t pinStep_,uint8_t pinMinEndStop_ ,uint8_t pinMaxEndStop_ , int closePos_, int startPos_,uint16_t stepsPerRevolution_):
  stepper(stepsPerRevolution_, pinDir_, pinStep_, pinEnable_)
{
  this->pinEna = pinEnable_;
  this->pinDir = pinDir_;
  this->pinPul = pinStep_;

  this->pinFcIni = pinMinEndStop_;
  this->pinFcEnd = pinMaxEndStop_;

  if(closePos_ >0){
    this->stepsMax = closePos_;
  }
  this->startPos = startPos_;

}

bool StepperNema::moveAwayMinEndStop()
{
  int counter = 0;
  while(counter < stepsMax*0.1)
  {
    stepper.move(1);
    counter++;
  }

  if(isMinEndStopPressed())
  {
      TRACE("Cannot move away min endstop!");
      return false;
  }
  else return true;
}

bool StepperNema::moveTowardsMinEndStop()
{
  int counter = 0;
  while(!isMinEndStopPressed() && counter < stepsMax*1.10)
  {
    stepper.move(-1);
    counter++;
  }
  if(!isMinEndStopPressed())
  {
      TRACE("Cannot hit min endstop!");
      return false;
  }
  else return true;
}

uint16_t StepperNema::countStepsToHome()
{
  uint16_t count = 0;
  while(!isMinEndStopPressed() && count < stepsMax*1.10)
  {
    stepper.move(-1);
    count++;
  }
  return count;
}

bool StepperNema::home()
{
  Serial.print("HOMING....");
  if(this->pinFcIni <= 0)
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

bool StepperNema::begin()
{
    this->stepper.begin(RPM, MICROSTEPS);
    TRACE("Inicio Nema"+String(this->pinDir)+" "+String(this->pinPul));
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    this->stepper.setEnableActiveState(LOW);
    this->stepper.enable();
    //stepper.setSpeedProfile(stepper.LINEAR_SPEED, MOTOR_ACCEL, MOTOR_DECEL); //Probar bien antes de activar

    if(this->pinFcEnd > 0) pinMode(this->pinFcEnd,INPUT_PULLUP);
    if(this->pinFcIni > 0)
      pinMode(this->pinFcIni,INPUT_PULLUP);
    else
      return true; //Si no hay final de carrera de minimo no hacemos nada mas.

    if(home())
    {//Comprobacion del mecanismo. Desde el final de carrera hasta la posicion maxima ida y vuelta
    //Los pasos para ir y para volver deben de ser (casi) los mismo o el mecanismo no esta bien
      stepper.move(stepsMax);
      uint32_t stepsBackToHome = countStepsToHome();
      Serial.println("STEPS: "+String(stepsMax) + " BACK: " + String(stepsBackToHome));
      int32_t d = stepsMax-stepsBackToHome;
      uint32_t difference = abs(d);
      uint32_t maxError = stepsMax * 0.01;
      Serial.println("diff: "+String(difference) + " max: " + String(maxError));

      if(difference > maxError)
      {
        Serial.println("UNRELIABLE STEPPER");
        return false;
      }
    }
    else
    {
      TRACE("Stepper Homing error!");
      return false;
    }
    stepper.move(startPos); // Diria que esto ya esta sucediendo en otra parte, lo dejo comentado hasta que pueda revisarlo
    lastStep = startPos;
    return true;
}

bool StepperNema::calibrate()
{
  if(this->pinFcIni != 0 && this->pinFcEnd != 0){
    while(digitalRead(this->pinFcIni) != LOW){
      //Desplazamos el stepper al inicio
      this->stepper.move(-1);
    }
    this->lastStep = 0;
    while(digitalRead(this->pinFcIni) != LOW){
      //Desplazamos el stepper al final para autocalibrado
      this->stepper.move(1);
      this->lastStep++;
    }
    this->stepsMax = this->lastStep;
  }
  return true;
}

void StepperNema::open(double percent)
{
    this->percent = constrain(percent,0.0,100.0);
    this->stepDestination = (this->stepsMax * (percent / 100))+this->startPos;
    int mover = this->stepDestination - this->lastStep;
     if(mover>0){
        this->lastDir=1; //derechas
      }else{
        this->lastDir=0; //izquierdas
      }

    //this->stepper.startMove(mover); // Mientras se resuelve el problema de la velocidad movemos bloqueando
    this->lastStep += mover;
    this->stepper.move(mover);
    //Serial.println("Stepper "+String(mover)+" "+String(this->stepDestination)+" "+String(this->lastStep)+" "+String(percent));
}

void StepperNema::close()
{
  this->open(0);
}

void StepperNema::update(){
  if(this->pinFcIni != 0 && isMinEndStopPressed() && !this->lastDir){
    stepper.stop();
  }

  if(this->pinFcEnd != 0 && isMaxEndStopPressed() && this->lastDir){
    stepper.stop();
  }

  unsigned wait_time_micros = stepper.nextAction();
    if(wait_time_micros > 0){
      if(this->lastDir){
        this->lastStep++;
      }else{
        this->lastStep--;
      }
    }
  //Serial.println("stepper->"+String(this->stepDestination)+","+String(this->lastDir)+","+String(this->lastStep));
}
