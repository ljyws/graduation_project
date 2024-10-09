#include "main_help.h"

STM32_GPIO led_green(LED_GREEN_GPIO_Port,LED_GREEN_Pin);

static void rtos_main(void* arg)
{
    led_green.config(GPIO_MODE_OUTPUT_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
    for(;;)
    {
        led_green.write(true);
        osDelay(500);
        led_green.write(false);
        osDelay(500);
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