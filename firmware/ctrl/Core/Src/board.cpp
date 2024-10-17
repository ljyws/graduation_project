#include "board.h"
#include "main_help.h"

extern "C" void SystemClock_Config(void);

STM32_SPI spi1_arbiter{&hspi1};
STM32_SPI spi3_arbiter{&hspi3};
Encoder mt6825 = {&spi1_arbiter};
DRV8301 driver{
    &spi3_arbiter,
    {DRV_CS_GPIO_Port,DRV_CS_Pin},
    {DRV_EN_GPIO_Port,DRV_EN_Pin},
    {DRV_FAULT_GPIO_Port,DRV_FAULT_Pin}
};

Motor motor = {
    &htim1,
    0b110,
    1.0 / SHUNT_RESISTANCE,
    driver
};

void system_init(void)
{
    HAL_Init();
    SystemClock_Config();
}

bool board_init()
{
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_SPI3_Init();
    MX_SPI1_Init();
    MX_TIM1_Init();

    return 1;
}
