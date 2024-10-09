#include "encoder.h"

Encoder::Encoder(STM32_SPI *enc_spi) : spi_(enc_spi){}

void Encoder::init()
{
    mode_ = config_.mode;
    spi_task_.config =
    {
        .Mode = SPI_MODE_MASTER,
        .Direction = SPI_DIRECTION_2LINES,
        .DataSize = SPI_DATASIZE_16BIT,
        .CLKPolarity = (mode_ == MODE_SPI_ABS_MT6825||mode_ == MODE_SPI_ABS_MT6816)?SPI_POLARITY_HIGH:SPI_POLARITY_LOW,
        .CLKPhase = SPI_PHASE_2EDGE,
        .NSS = SPI_NSS_SOFT,
        .BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16,
        .FirstBit = SPI_FIRSTBIT_MSB,
        .TIMode = SPI_TIMODE_DISABLE,
        .CRCCalculation = SPI_CRCCALCULATION_DISABLE,
        .CRCPolynomial = 10,
    };

    if(mode_ & MODE_FLAG_ABS)
    {
        abs_spi_cs_pin_init();

    }

}

void Encoder::update_pll_gains()
{
    pll_kp_ = 2.0f * config_.bandwith;
    pll_ki_ = 0.25f * (pll_kp_ * pll_kp_);
}

void Encoder::sample_now()
{
    switch (mode_)
    {
        case MODE_SPI_ABS_MT6816:
        case MODE_SPI_ABS_MT6825:
        case MODE_SPI_ABS_AS5047:
        {
            abs_spi_start_transaction();
            break;
        }
        default:break;
    }
}

bool Encoder::abs_spi_start_transaction()
{
    if(mode_ & MODE_FLAG_ABS)
    {
        spi_task_.cs_gpio = spi_abs_cs_gpio_;
        spi_task_.tx_buf = (uint8_t*)abs_spi_dma_tx_;
        spi_task_.rx_buf = (uint8_t*)abs_spi_dma_rx_;
        spi_task_.len = 1;
        spi_->transfer_async(&spi_task_);
    }
    return true;
}

void Encoder::abs_spi_cb()
{
    uint16_t pos;
    switch (mode_)
    {
        case MODE_SPI_ABS_MT6816:
        {
            break;
        }
        case MODE_SPI_ABS_MT6825:
        {
            break;
        }
        case MODE_SPI_ABS_AS5047:
        {
            break;
        }
        default:break;
    }


}

bool Encoder::update()
{
    int32_t delta_enc = 0;
    int32_t pos_abs_latched = pos_abs_;

    switch (mode_)
    {
        case MODE_SPI_ABS_MT6816:
        case MODE_SPI_ABS_MT6825:
        case MODE_SPI_ABS_AS5047:
        {
            abs_spi_start_transaction();
            break;
        }
        default:break;
    }

}


void Encoder::abs_spi_cs_pin_init()
{
    spi_abs_cs_gpio_.config(GPIO_MODE_OUTPUT_PP,GPIO_PULLUP,GPIO_SPEED_LOW);
    spi_abs_cs_gpio_.write(true);
}
