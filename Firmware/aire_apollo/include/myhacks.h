#ifndef MYHACKS_H
#define MYHACKS_H
//Use this for your own config overrides (gitignored)
//

//#ifdef  DEFAULT_CMH20_PEEP
//  #undef  DEFAULT_CMH20_PEEP
//#endif
//#define DEFAULT_CMH20_PEEP -1 //Presión mínima en el vaciado //HAck para funcionar sin suministro de aire!

#define DEFAULT_CMH20_MAX     60  //Presión máxima de seguridad
#define DEFAULT_CMH20_PICO    45  //Presión máxima regular
#define DEFAULT_CMH20_MESETA  15  //Presión mínima al insuflar
#define DEFAULT_CMH20_PEEP    15  //Presión mínima en el vaciado

#endif
