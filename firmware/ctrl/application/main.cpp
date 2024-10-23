
#include "main_help.h"

osThreadId_t rtosMainTaskHandle;


Ctrl ctrl{};


void Ctrl::sampling_cb()
{
    axis.encoder_.sample_now();
}

void Ctrl::control_loop_cb()
{
    // vofa_start();
    axis.encoder_.phase_.reset();
    axis.encoder_.phase_vel_.reset();
    axis.encoder_.pos_estimate_.reset();
    axis.encoder_.vel_estimate_.reset();
    axis.encoder_.pos_circular_.reset();
    axis.motor_.Vdq_setpoint_.reset();
    axis.motor_.Idq_setpoint_.reset();
    axis.open_loop_controller_.Idq_setpoint_.reset();
    axis.open_loop_controller_.Vdq_setpoint_.reset();
    axis.open_loop_controller_.phase_.reset();
    axis.open_loop_controller_.phase_vel_.reset();
    axis.open_loop_controller_.total_distance_.reset();

    axis.encoder_.update();
    axis.open_loop_controller_.update();
    axis.motor_.update();
    axis.motor_.current_control_.update();
}




static void rtos_main(void* arg)
{
    MX_USB_DEVICE_Init();

    if(axis.encoder_.config_.mode & Encoder::MODE_FLAG_MagnTek)
    {
        axis.encoder_.abs_spi_cs_pin_init();
    }

    axis.motor_.init();
    // //
    axis.encoder_.init();
    //
    HAL_GPIO_WritePin(DRV_EN_GPIO_Port,DRV_EN_Pin,GPIO_PIN_SET);
    start_adc_pwm();

    // //
    for(uint8_t i = 0; i<2000; i++)
    {
        bool motor_ready = axis.motor_.current_meas_.has_value();

        if(motor_ready)
            break;

        osDelay(1);
    }
    // // //
    axis.start_thread();
    //
    osThreadTerminate(rtosMainTaskHandle);
}


extern "C" int main(void)
{
    system_init();
    board_init();
    osDelay(200);
    osKernelInitialize();
    const osThreadAttr_t rtosMainTask_attributes = {
            .name = "rtos_main_task",
            .stack_size = 1024 * 4,
            .priority = (osPriority_t) osPriorityNormal,
    };
    rtosMainTaskHandle = osThreadNew(rtos_main, NULL, &rtosMainTask_attributes);
    osKernelStart();
}