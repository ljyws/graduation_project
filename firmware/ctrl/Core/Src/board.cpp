#include "board.h"

extern "C" void SystemClock_Config(void);


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

    return 1;
}
