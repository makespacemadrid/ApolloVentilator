#include "ApolloValve.h"
#include <Arduino.h>

ApolloValve::ApolloValve()
{}

ApolloValve::~ApolloValve()
{}

void ApolloValve::open()
{}

void ApolloValve::close()
{}

//
// To be able to open EV just 10%
// in case the valve can do it
//
void setOpeningDegree(int value)
{}

//
// returns valve state as intener
// If using a full open/full close valve, 1 means open 0 means close
// If using other type of valve, resturn openning degree in %
//
int ApolloValve::readState()
{}