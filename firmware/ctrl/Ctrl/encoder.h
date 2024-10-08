#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "stm32_spi.h"

class Encoder
{
public:
    typedef struct
    {

        Encoder *parent = nullptr;
    }config_t;

    Encoder(STM32_SPI *enc_spi);
    void init(void);


    STM32_SPI *spi_;
    config_t config_;
};



#endif

