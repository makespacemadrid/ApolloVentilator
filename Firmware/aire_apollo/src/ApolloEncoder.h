#ifndef ENCODER_H
#define ENCODER_H

#define LATCHSTATE 3
#include "../lib/Encoder-1.4.1/Encoder.h"
class ApolloEncoder : public Encoder
{
public:
    ApolloEncoder(uint8_t pinDT, uint8_t pinCLK, uint8_t pinSW) : Encoder(pinDT, pinCLK)
    {
        _pulsador = pinSW;
    };
    bool readButton();
    bool swapValue(int *valor);
    bool swapValue(bool *valor);
    bool updateValue(int *valor, int delta = 1);
    bool updateValue(float *valor, float delta = 1.0);

protected:
    //Encoder encoder;

private:
    int _pulsador;
    int _oldState;
    bool _flag;
    unsigned long _tiempo;

    int move();
};

#endif // ENCODER_H
