#ifndef VENTILATOR_H
#define VENTILATOR_H

#include "Arduino.h"
#include "ApolloStorage.h"
#include "ApolloHal.h"


enum ventilationStatus
{
  statusIdle,
  statusTriggerInspiration,
  statusInspiration,
  statusInspirationPause,
  statusExpiration,
};

class ApolloVentilator
{
public:
  ApolloVentilator(ApolloHal* hal,ApolloStorage* storage) :
  _hal(hal),_storage(storage)
  {

  }

  void update()
  {

  }

  void ventilationUpdate()
  {

  }


protected:
    ventilationStatus _status;
    ApolloHal*        _hal;
    ApolloStorage*    _storage;


};


#endif
