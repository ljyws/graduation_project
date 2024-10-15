#include "stm32_spi.h"

bool STM32_SPI::start()
{
    spi_task &task = *task_;

    HAL_StatusTypeDef status = HAL_ERROR;

//    if (hspi_->hdmatx->State != HAL_DMA_STATE_READY || hspi_->hdmarx->State != HAL_DMA_STATE_READY)
//        status = HAL_BUSY;
//    else if (task.tx_buf && task.rx_buf)
    status = HAL_SPI_TransmitReceive(hspi_, (uint8_t*)task.tx_buf, (uint8_t*)task.rx_buf, task.len,100);
//    else if (task.tx_buf)
//        status = HAL_SPI_Transmit(hspi_, (uint8_t*)task.tx_buf, task.len,100);
//    else if (task.rx_buf)
//        status = HAL_SPI_Receive(hspi_, task.rx_buf, task.len,100);

    if (status != HAL_OK)
        task.cs_gpio.write(true);

    return status == HAL_OK;
}


bool STM32_SPI::transfer(SPI_InitTypeDef config, STM32_GPIO cs_gpio, const uint8_t *tx_buf, uint8_t *rx_buf, size_t len, uint32_t timeout_ms)
{
    volatile uint8_t result = 0xff;

    spi_task task =
    {
        .config = config,
        .cs_gpio = cs_gpio,
        .tx_buf = tx_buf,
        .rx_buf = rx_buf,
        .len = len,
    };

    if(!start())
        return true;
    else
        return false;
}


void STM32_SPI::transfer_async(spi_task* task)
{
    spi_task** ptr = &task_;
    *ptr = task;
    start();
}