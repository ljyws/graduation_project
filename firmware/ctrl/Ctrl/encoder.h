#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "stm32_spi.h"

class Encoder
{
public:

    static constexpr uint32_t MODE_FLAG_ABS = 0x100;
    enum mode_e
    {
        MODE_SPI_ABS_MT6816 = 256,
        MODE_SPI_ABS_MT6825 = 257,
        MODE_SPI_ABS_AS5047 = 258,
    };
    typedef struct
    {
        mode_e mode = MODE_SPI_ABS_MT6825;
        float bandwith = 1000.0f;
        int32_t phase_offset = 0;
        int32_t cpr = (2048*4);
        int32_t direction = 0;
        Encoder *parent = nullptr;
    }config_t;

    Encoder(STM32_SPI *enc_spi);
    void init(void);
    void abs_spi_cs_pin_init(void);
    void update_pll_gains();


    bool update();



    int32_t pos_abs_ = 0;

    void sample_now();
    bool abs_spi_start_transaction();
    void abs_spi_cb();

    bool abs_spi_pos_updated_ = false;

    STM32_SPI *spi_;
    config_t config_;


    float pll_kp_ = 0.0f;   // [count/s / count]
    float pll_ki_ = 0.0f;   // [(count/s^2) / count]
    mode_e mode_;


    uint16_t abs_spi_dma_tx_[1] = {0xFFFF};
    uint16_t abs_spi_dma_rx_[1];
    STM32_GPIO spi_abs_cs_gpio_;
    STM32_SPI::spi_task spi_task_;
};



#endif

