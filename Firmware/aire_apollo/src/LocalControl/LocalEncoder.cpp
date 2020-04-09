#include "LocalEncoder.h"
#include "Arduino.h"
// LocalEncoder::LocalEncoder(uint8_t pin1, uint8_t pin2)
// {
// }
/**
 * @brief Modifica un valor dicotómico entre 0 y 1.
 *
 * Cuando se gira el LocalEncoder1, el valor se pasa de 0 a 1 y
 * de 1 a 0 cada vez que se mueve el dial.
 *
 * @param valor valor a actualizar
 */
bool LocalEncoder::swapValue(int *valor)
{
  switch (move())
  {
  case 1:
  case 2:
    *valor = !(*valor);
    return true;
    break;
  default:
    return false;
    break;
  }
}

bool LocalEncoder::swapValue(bool *valor)
{
  switch (move())
  {
  case 1:
  case 2:
    *valor = !(*valor);
    return true;
    break;
  default:
    return false;
    break;
  }
}

int LocalEncoder::move()
{
  long newval = read();
  // Giramos horario (Subimos en el menu)
  if (newval <= (_oldState - 4))
  {
    _oldState = newval;
    return 1;
  }
  if (newval >= (_oldState + 4))
  {
    _oldState = newval;
    return 2;
  }
  return false;
}

/**
 * @brief Altera un valor entero un incremento delta.
 *
 * Incrementa (antihorario) o decrementa (horario) un
 * valor dado, un delta determinado.
 *
 * @param valor valor a actualizar
 * @param delta incremento, por defecto: 1
 */
bool LocalEncoder::updateValue(int *valor, int delta)
{
  switch (move())
  {
  case 1:
    //Sentido horario
    *valor = *valor + delta;
    return true;
    break;
  case 2:
    //Sentido antohorario
    *valor = *valor - delta;
    return true;
    break;
  default:
    return false;
    break;
  }
}

bool LocalEncoder::updateValue(float *valor, float delta)
{
  switch (move())
  {
  case 1:
    //Sentido horario
    *valor = *valor + delta;
    return true;
    break;
  case 2:
    //Sentido antohorario
    *valor = *valor - delta;
    return true;
    break;
  default:
    return false;
    break;
  }
}

/**
 * @brief Lee la señal emitida al pulsar el LocalEncoder1
 *
 * @return true cuando se pulsa
 * @return false si no se ha pulsado
 */
bool LocalEncoder::readButton()
{
  if (digitalRead(_pulsador) != 1)
  {
    if (!_flag)
    {
      _tiempo = millis();
      _flag = true;
    }
    while (digitalRead(_pulsador) == 0)
    {
      if (millis() -
              _tiempo >
          300)
      {
        _flag = false;
        return true;
      }
    }
  }
  return false;
}
