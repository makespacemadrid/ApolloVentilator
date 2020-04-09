#ifndef PANTALLA_H
#define PANTALLA_H

#include "../include/defaults.h"
#include "../lib/LiquidCrystal_I2C/LiquidCrystal_I2C.h"

class LocalDisplay
{
public:
  LocalDisplay();
  void writeLine(int line, String message = "", int offsetLeft = 0);
  void clear();
  void init();

private:
  LiquidCrystal_I2C lcd = LiquidCrystal_I2C(LCD_DIR, 20, 4);
};

#endif // PANTALLA_H
