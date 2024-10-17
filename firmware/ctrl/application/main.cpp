
#include "main_help.h"

STM32_GPIO led_green(LED_GREEN_GPIO_Port,LED_GREEN_Pin);




uint8_t test_rx;
uint8_t test_tx = 0x08;
static void rtos_main(void* arg)
{
    led_green.config(GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
    for(;;)
    {
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