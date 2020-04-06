#include "SubmarineNema17.h"

SubmarineNema17::SubmarineNema17(uint8_t pinEna_,uint8_t pinDir_,uint8_t pinPul_,uint8_t pinFcIni_ ,uint8_t pinFcEnd_):stepper(200, pinDir, pinPul)
{
  this->pinEna = pinEna_;
  this->pinDir = pinDir_;
  this->pinPul = pinPul_;

  this->pinFcIni = pinFcIni_;
  this->pinFcEnd = pinFcEnd_;

}

bool SubmarineNema17::begin()
{
    // Configure stopper pin to read HIGH unless grounded
    pinMode(STOPPER_PIN, INPUT_PULLUP);
    // pinMode(this->pinEna,OUTPUT);
    // digitalWrite(this->pinEna, LOW);

    this->stepper.begin(RPM, MICROSTEPS);

    Serial.println("Inicio Nema"+String(this->pinDir)+" "+String(this->pinPul));
    // if using enable/disable on ENABLE pin (active LOW) instead of SLEEP uncomment next line
    // stepper.setEnableActiveState(LOW);
    //this->stepper.enable();
    //this->stepper.startMove(100 * MOTOR_STEPS * MICROSTEPS);     // in microsteps
    this->stepper.move(3600); //Grades
    this->stepper.move(-3600); //Steps
    return true;





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
    this->stepDestination = 360 * (percent / 100);
    int mover = this->stepDestination - this->lastStep;
     if(mover>0){
        this->lastDir=1; //derechas
      }else{
        this->lastDir=0; //izquierdas
      }    
    this->stepper.startMove(mover);
}

void SubmarineNema17::close()
{
  this->open(0);
}

void SubmarineNema17::update(){
unsigned wait_time_micros = stepper.nextAction();  
    if(wait_time_micros > 100){
      if(this->lastDir){
        this->lastStep++;
      }else{
        this->lastStep--;
      }
    }
  //Serial.println(String(this->stepDestination)+","+String(this->stepNow)+","+String(this->lastDir)+","+String(this->lastStep));
return;







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