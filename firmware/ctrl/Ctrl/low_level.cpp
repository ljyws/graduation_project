#include "board.h"
#include "cmsis_os.h"

#include "main_help.h"


constexpr float adc_ref_voltage = 3.3f;
float vbus_voltage = 12.0f;
float ibus_ = 0.0f;

void start_adc_pwm()
{

    int half_load = TIM_1_8_PERIOD_CLOCKS / 2;
    motor.timer_->Instance->CCR1 = half_load;
    motor.timer_->Instance->CCR2 = half_load;
    motor.timer_->Instance->CCR3 = half_load;
    // Enable PWM outputs (they are still masked by MOE though)
    motor.timer_->Instance->CCER |= (TIM_CCx_ENABLE << TIM_CHANNEL_1);
    motor.timer_->Instance->CCER |= (TIM_CCxN_ENABLE << TIM_CHANNEL_1);
    motor.timer_->Instance->CCER |= (TIM_CCx_ENABLE << TIM_CHANNEL_2);
    motor.timer_->Instance->CCER |= (TIM_CCxN_ENABLE << TIM_CHANNEL_2);
    motor.timer_->Instance->CCER |= (TIM_CCx_ENABLE << TIM_CHANNEL_3);
    motor.timer_->Instance->CCER |= (TIM_CCxN_ENABLE << TIM_CHANNEL_3);

    __HAL_ADC_ENABLE(&hadc1);

    osDelay(2);

    // start_timer();


}

