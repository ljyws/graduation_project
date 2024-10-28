#include "board.h"
#include "main_help.h"
#include <low_level.h>


extern "C" void SystemClock_Config(void);

// STM32_GPIO led_green(LED_GREEN_GPIO_Port,LED_GREEN_Pin);

STM32_SPI spi1_arbiter{&hspi1};
Encoder encoder = {&spi1_arbiter};

Motor motor = {
    &htim1,
    0b110,
    1.0 / SHUNT_RESISTANCE
};

Controller controller;

Axis axis = {
    encoder,
    controller,
    motor
};

void system_init(void)
{
    HAL_Init();
    SystemClock_Config();
}

bool board_init()
{
    MX_GPIO_Init();
    MX_SPI1_Init();
    MX_TIM1_Init();
    MX_ADC1_Init();
    MX_ADC2_Init();

    MX_USB_Device_Init();

    return 1;
}


void start_timers()
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);    //starts PWM on CH1 pin
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); //start
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);    //starts PWM on CH2 pin
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); //start
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);    //starts PWM on CH3 pin
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3); //start
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 4150);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);

    HAL_TIM_Base_Start_IT(&htim1);
}

uint16_t adc_measurements_[3] = {0};
void start_adcs()
{
    HAL_ADCEx_InjectedStart(&hadc1);
    HAL_ADCEx_InjectedStart(&hadc2);
    __HAL_ADC_ENABLE_IT(&hadc1,ADC_IT_JEOC);
    __HAL_ADC_ENABLE_IT(&hadc2,ADC_IT_JEOC);
}


bool fetch_and_reset_adcs(std::optional<Iph_ABC_t> *current)
{
    // bool all_adcs_done = (ADC1->SR & (ADC_SR_EOC | ADC_SR_JEOC)) == (ADC_SR_EOC | ADC_SR_JEOC);
    // if (!all_adcs_done)
    //     return false;

    vbus_sense_adc_cb(ADC2->JDR1);
    std::optional<float> phA = motor.phase_current_from_adcval(ADC1->JDR1);
    std::optional<float> phB = motor.phase_current_from_adcval(ADC1->JDR2);
    std::optional<float> phC = motor.phase_current_from_adcval(ADC1->JDR3);

    if(phA.has_value() && phB.has_value() && phC.has_value())
        *current = {*phA,*phB,*phC};
    return true;
}



void control_loop_irq_handler(void)
{
    __HAL_TIM_CLEAR_IT(&htim1,TIM_IT_UPDATE);
    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_7);

    ctrl.sampling_cb();

    std::optional<Iph_ABC_t> current;

    fetch_and_reset_adcs(&current);
    // if(!(TIM1->BDTR & TIM_BDTR_MOE_Msk))
    //     current = {0.0f, 0.0f};
    motor.current_meas_cb(current);

    ctrl.control_loop_cb();

    motor.dc_calib_cb(current);

    // motor.pwm_update_cb();
}
