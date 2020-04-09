#include "LocalDisplay.h"
#include "Arduino.h"
#include "../../include/defaults.h"

LocalDisplay::LocalDisplay()
{
  //   this->_Flecha[] = {
  //       B00000,
  //       B00100,
  //       B01100,
  //       B11111,
  //       B11111,
  //       B01100,
  //       B00100,
  //       B00000};

  //   this->_Cruz[] = {
  //       B00000,
  //       B01110,
  //       B01110,
  //       B11111,
  //       B11111,
  //       B01110,
  //       B01110,
  //       B00000};
}

/**
 * @brief Escribe en una linea de la LocalDisplay un mensaje.
 *
 * @param line línea donde se escribe, puede ser 0 o 1
 * @param message mensaje a escribir (max. 16 caracteres)
 * @param offsetLeft posicion inicial desde la que se empieza a escribit
 */
void LocalDisplay::writeLine(int line, String message, int offsetLeft)
{
  lcd.setCursor(0, line);
  lcd.print("");
  lcd.setCursor(offsetLeft, line);
  lcd.print(message);
}

/**
 * @brief Deja el Display en blanco, sin caracteres.
 */
void LocalDisplay::clear()
{
  lcd.clear();
}

void LocalDisplay::init()
{
#ifdef LCD_DIR
  lcd.init();
  lcd.backlight();
#else
  lcd.begin(20, 4);
#endif // DEBUG
  //Creamos el icono flecha
  //lcd.createChar(0, this->_Flecha);
  //lcd.createChar(1, this->_Cruz);
}
