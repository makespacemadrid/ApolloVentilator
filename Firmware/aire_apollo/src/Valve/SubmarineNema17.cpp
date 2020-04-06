#include "SubmarineNema17.h"

SubmarineNema17::SubmarineNema17(uint8_t pinEna_,uint8_t pinDir_,uint8_t pinPul_,uint8_t pinFcIni_ ,uint8_t pinFcEnd_)
{
  this->pinEna = pinEna_;
  this->pinDir = pinDir_;
  this->pinPul = pinPul_;

  this->pinFcIni = pinFcIni_;
  this->pinFcEnd = pinFcEnd_;

}
bool SubmarineNema17::begin()
{
  pinMode(this->pinEna,OUTPUT);
  pinMode(this->pinDir,OUTPUT);
  pinMode(this->pinPul,OUTPUT);
  if(this->pinFcIni){
    pinMode(this->pinFcIni,INPUT_PULLUP);
    while(!digitalRead(this->pinFcIni)){
      //Desplazamos el stepper al inicio
    }
  }
  if(this->pinFcEnd){
    pinMode(this->pinFcEnd,INPUT_PULLUP);
    uint8_t step = 0;
      digitalWrite(this->pinEna,HIGH);
      digitalWrite(this->pinDir,HIGH);
      delayMicroseconds(500);
    while(!digitalRead(this->pinFcEnd)){
      //Desplazamos el stepper al final
      digitalWrite(this->pinPul,HIGH);
      delayMicroseconds(500);
      digitalWrite(this->pinPul,LOW);
      delayMicroseconds(500);
      step++;
    }
    stepEnd = step;
  }else{
    stepEnd = 100; //Si no hay fin de carrera definir el número de pasos al fin de carrera
  }
  return true;
}

void SubmarineNema17::open(uint8_t percent)
{
    if(percent > 100) percent = 100;
    if(percent < 0) percent = 0;
    this->stepDestination = stepEnd * (percent / 100);
}

void SubmarineNema17::close()
{
  this->stepDestination = 0;
}

void SubmarineNema17::update(){
  if(this->stepDestination == this->stepNow){
    return;
  }
  unsigned long now = millis();
  if((now-this->lastTime) < 500){
    return;
  }

  if(this->stepDestination > this->stepNow && !this->lastDir){
      digitalWrite(this->pinDir,HIGH);
      this->lastDir = 1;
      this->lastTime = now;
  }
  if(this->stepDestination < this->stepNow && this->lastDir){
      digitalWrite(this->pinDir,LOW);
      this->lastDir = 0;
      this->lastTime = now;
  }

  if(!this->lastStep){
      digitalWrite(this->pinDir,HIGH);
      this->lastStep = 1;
  }else{
      digitalWrite(this->pinDir,LOW);
      this->lastStep = 0;
  }
  
}