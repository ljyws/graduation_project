#ifndef __STM32_SPI_H__
#define __STM32_SPI_H__

#include "stm32_gpio.h"
#include "spi.h"

class STM32_SPI
{
public:
    struct spi_task
    {
        SPI_InitTypeDef config;
        STM32_GPIO cs_gpio;
        const uint8_t *tx_buf;
        uint8_t *rx_buf;
        size_t len;
        void (*on_complete)(void*, bool);
        void* on_complete_ctx;
        bool is_in_use = false;
        struct spi_task *next;
    };

    STM32_SPI(SPI_HandleTypeDef *hspi) : hspi_(hspi){}


    void transfer_async(spi_task* task);

    bool transfer(SPI_InitTypeDef config, STM32_GPIO cs_gpio, const uint8_t* tx_buf, uint8_t* rx_buf, size_t len, uint32_t timeout_ms);



private:
    bool start();
    SPI_HandleTypeDef *hspi_;
};




#endif
