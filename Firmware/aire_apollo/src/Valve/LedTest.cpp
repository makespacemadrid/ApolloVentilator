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
    delay(10);
  }
  for(int i=255;i>0;i--){
    analogWrite(this->pin, i);
    delay(10);
  }
  return true;
}

void LedTest::open(uint8_t percent)
{
    if(percent > 100) percent = 100;
    if(percent < 0) percent = 0;
    this->_percent = percent;
    analogWrite(this->pin, ((185 * (percent / 100))+70));
}

void LedTest::close()
{
    this->_percent = 0;
    analogWrite(this->pin, 0);
}

void LedTest::update(){
  return;  
}