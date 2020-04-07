#include "LedTest.h"

LedTest::LedTest(uint8_t pin_)
{
  this->pin = pin_;
}

bool LedTest::begin()
{
  pinMode(this->pin,OUTPUT);
  for(int i=0;i<255;i++){
    analogWrite(this->pin, i);
    delay(1);
  }
  for(int i=255;i>0;i--){
    analogWrite(this->pin, i);
    delay(1);
  }
  return true;
}

void LedTest::open(double percent)
{
    if(percent > 100) percent = 100;
    if(percent < 0) percent = 0;
    this->percent = percent;
    analogWrite(this->pin, ((255 * (percent / 100))));
}

void LedTest::close()
{
    this->percent = 0;
    analogWrite(this->pin, 0);
}

void LedTest::update(){
  return;  
}