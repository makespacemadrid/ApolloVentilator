#ifndef SubmarineNema17_H
#define  SubmarineNema17_H

#include<Arduino.h>
#include "../ApolloValve.h"

class SubmarineNema17 : public ApolloValve
{

public:

  SubmarineNema17(uint8_t pinEna_,uint8_t pinDir_,uint8_t pinPul_,uint8_t pinFcIni_ = 0,uint8_t pinFcEnd_ = 0);
  bool    begin();
  void    open(uint8_t percent = 100);
  void    close();
  uint8_t status() {return _percent > 0;}

  void    update(); //Required to move de steps


protected:

  uint8_t _percent;
  uint8_t pinEna;
  uint8_t pinDir;
  uint8_t pinPul;
  uint8_t stepEnd;
  uint8_t pinFcIni = 0;
  uint8_t pinFcEnd = 0;

  uint8_t stepDestination = 0;
  uint8_t stepNow = 0;

  bool lastDir;
  bool lastStep;
  unsigned long lastTime;

};

#endif