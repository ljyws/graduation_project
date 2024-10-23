#include "stm32_spi.h"

bool STM32_SPI::start()
{
    spi_task &task = *task_;

    HAL_StatusTypeDef status = HAL_ERROR;

    task.cs_gpio.write(false);
//    if (hspi_->hdmatx->State != HAL_DMA_STATE_READY || hspi_->hdmarx->State != HAL_DMA_STATE_READY)
//        status = HAL_BUSY;
    if (task.tx_buf && task.rx_buf)
        status = HAL_SPI_TransmitReceive(hspi_, (uint8_t*)task.tx_buf, task.rx_buf, task.len,100);
    else if (task.tx_buf)
        status = HAL_SPI_Transmit(hspi_, (uint8_t*)task.tx_buf, task.len,100);
    else if (task.rx_buf)
        status = HAL_SPI_Receive(hspi_, task.rx_buf, task.len,100);
    task.cs_gpio.write(true);
    // if (status != HAL_OK)
    //     task.cs_gpio.write(true);

    return status == HAL_OK;
}


bool STM32_SPI::transfer(SPI_InitTypeDef config, STM32_GPIO cs_gpio, const uint8_t *tx_buf, uint8_t *rx_buf, size_t len, uint32_t timeout_ms)
{
    volatile uint8_t result = 0xff;

    spi_task _task =
    {
        .config = config,
        .cs_gpio = cs_gpio,
        .tx_buf = tx_buf,
        .rx_buf = rx_buf,
        .len = len,
    };
    transfer_async(&_task);

    return false;
}


void STM32_SPI::transfer_async(spi_task* task)
{
    spi_task** ptr = &task_;
    *ptr = task;
    start();
}