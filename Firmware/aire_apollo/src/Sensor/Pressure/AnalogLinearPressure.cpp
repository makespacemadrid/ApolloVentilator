#include "AnalogLinearPressure.h"

AnalogLinearPressure::AnalogLinearPressure(uint8_t pin_, int min_, int max_){
    this->pin = pin_;
    this->min = min_;
    this->max = max_;
}

bool AnalogLinear::begin(){
    pinMode(this->pin, INPUT);
    return true;
}

float AnalogLinear::readPascal(){
    return this->readBar() * 100000;
}

float AnalogLinear::readBar(){
    return (analogRead(this->pin) / (this->max - this->min))*1023;
}