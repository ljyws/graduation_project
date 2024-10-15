
#include "main_help.h"

STM32_GPIO led_green(LED_GREEN_GPIO_Port,LED_GREEN_Pin);


STM32_SPI spi1_arbiter{&hspi1};
STM32_SPI spi3_arbiter{&hspi3};
Encoder mt6825 = {&spi1_arbiter};
DRV8301 driver{
    &spi3_arbiter,
    {DRV_CS_GPIO_Port,DRV_CS_Pin},
    {DRV_EN_GPIO_Port,DRV_EN_Pin},
    {DRV_FAULT_GPIO_Port,DRV_FAULT_Pin}
};

uint8_t test_rx;
uint8_t test_tx = 0x08;
static void rtos_main(void* arg)
{
    led_green.config(GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
    mt6825.init();
    for(;;)
    {
        mt6825.sample_now();
//        mt6825.abs_spi_cb();
        osDelay(10);
    }
}

extern "C" int main(void)
{
    system_init();
    board_init();
    osKernelInitialize();
    osThreadId_t rtosMainTaskHandle;
    const osThreadAttr_t rtosMainTask_attributes = {
            .name = "rtos_main_task",
            .stack_size = 128 * 4,
            .priority = (osPriority_t) osPriorityNormal,
    };
    rtosMainTaskHandle = osThreadNew(rtos_main, NULL, &rtosMainTask_attributes);
    osKernelStart();
}