#ifndef GLOBALFUNCTIONS_H
#define GLOBALFUNCTIONS_H

#include <Arduino.h>

//funciones para gestionar el tiempo(gestionando el eventual desbordamiento de contadores)
static unsigned long getMilisecondsFrom(unsigned long timestamp)
{
  unsigned long result;
  unsigned long now = millis();
  if(now > timestamp)
  {
    result = now - timestamp;
  }
  else// si el timestamp es mayor que la marca de tiempo actual es muy probable que se haya desbordado el contador
  {
    result = now;//impreciso!hacer mejor teniendo en cuenta tambien cuanto le quedaba al contador para desbordar!(y algun sitema extra para ratificar que ha desbordado el contador)
  }

  return result;
}

static unsigned long getMicrosecondsFrom(unsigned long timestamp)
{
  unsigned long result;
  unsigned long now = micros();
  if(now > timestamp)
  {
    result = now - timestamp;
  }
  else
  {
    result = now;
  }

  return result;
}

#endif
