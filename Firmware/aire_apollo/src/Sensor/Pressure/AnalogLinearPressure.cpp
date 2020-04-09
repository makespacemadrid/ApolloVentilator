#include "AnalogLinearPressure.h"

AnalogLinearPressure::AnalogLinearPressure(uint8_t pin_, int min_, int max_){
    this->pin = pin_;
    this->min = min_;
    this->max = max_;
}

bool AnalogLinearPressure::begin(){
    pinMode(this->pin, INPUT);
    return true;
}

float AnalogLinearPressure::readPascal(){
    return this->readBar() * 100000;
}

float AnalogLinearPressure::readBar(){
    return (analogRead(this->pin) / (this->max - this->min))*1023;
}