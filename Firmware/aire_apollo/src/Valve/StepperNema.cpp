#include "StepperNema.h"
#include "trace.h"
StepperNema::StepperNema(uint8_t pinEna_,uint8_t pinDir_,uint8_t pinPul_,uint8_t pinFcIni_ ,uint8_t pinFcEnd_,int stepsMax_,int startPos_):
  stepper(MOTOR_STEPS, pinDir_, pinPul_, pinEna_)
{
  this->pinEna = pinEna_;
  this->pinDir = pinDir_;
  this->pinPul = pinPul_;

  this->pinFcIni = pinFcIni_;
  this->pinFcEnd = pinFcEnd_;
  if(stepsMax_ >0){
    this->stepsMax = stepsMax_;
  }
  this->startPos = startPos_;

}

bool StepperNema::begin()
{
    this->stepper.begin(RPM, MICROSTEPS);
    TRACE("Inicio Nema"+String(this->pinDir)+" "+String(this->pinPul));
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    this->stepper.setEnableActiveState(LOW);
    this->stepper.enable();

  if(this->pinFcIni != 0){
    pinMode(this->pinFcIni, INPUT_PULLUP);
    while(digitalRead(this->pinFcIni) != LOW){//OJO! NC/NO?
      //Desplazamos el stepper al inicio
      this->stepper.move(-99999999);//limitarlo a maxsteps?
    }
    this->stepper.move(startPos);
  }

  if(this->pinFcEnd != 0){
    pinMode(this->pinFcEnd, INPUT_PULLUP);
  }

  return true;
}
bool StepperNema::calibrate()
{
  if(this->pinFcIni != 0 && this->pinFcEnd != 0){
    while(digitalRead(this->pinFcIni) != HIGH){
      //Desplazamos el stepper al inicio
      this->stepper.move(-99999999);
    }
    this->lastStep = 0;
    while(digitalRead(this->pinFcIni) != HIGH){
      //Desplazamos el stepper al final para autocalibrado
      this->stepper.move(9999999);
      this->lastStep++;
    }
    this->stepsMax = this->lastStep;
  }
  return true;
}
void StepperNema::open(double percent)
{
    if(percent > 100) percent = 100;
    if(percent < 0) percent = 0;
    this->percent = percent;
    this->stepDestination = (this->stepsMax - this->startPos) * (int(percent) / 100);
    int mover = this->stepDestination - this->lastStep;
     if(mover>0){
        this->lastDir=1; //derechas
      }else{
        this->lastDir=0; //izquierdas
      }
    this->stepper.startMove(mover);
    Serial.println("Stepper "+String(mover)+" "+String(this->stepDestination)+" "+String(this->lastStep)+" "+String(percent));
}

void StepperNema::close()
{
  this->open(this->startPos);
}

void StepperNema::update(){
  if(this->pinFcIni != 0 && digitalRead(this->pinFcIni) == HIGH){
    stepper.stop();
  }

  if(this->pinFcEnd != 0 && digitalRead(this->pinFcEnd) == HIGH){
    stepper.stop();
  }

unsigned wait_time_micros = stepper.nextAction();
    if(wait_time_micros > 1){
      if(this->lastDir){
        this->lastStep++;
      }else{
        this->lastStep--;
      }
    }
  //Serial.println("stepper->"+String(this->stepDestination)+","+String(this->lastDir)+","+String(this->lastStep));
  return;
}
