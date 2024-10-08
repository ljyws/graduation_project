#include "main_help.h"

static void rtos_main(void* arg)
{
    for(;;)
    {
        HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port,LED_GREEN_Pin);
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