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
    MX_DMA_Init();
    MX_SPI1_Init();
    MX_TIM1_Init();
    MX_ADC1_Init();
    MX_ADC2_Init();

    MX_USB_Device_Init();

    return 1;
}


void start_timers()
{
    // hadc1.Instance->CR2 &= ~(ADC_CR2_EXTEN | ADC_CR2_JEXTEN);
    // hadc1.Instance->CR2 |= (ADC_EXTERNALTRIGCONVEDGE_FALLING | ADC_EXTERNALTRIGCONVEDGE_FALLING);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, 4200);
    HAL_TIM_Base_Start_IT(&htim1);
}

uint16_t adc_measurements_[3] = {0};
void start_adcs()
{
    // __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_EOC);
    // __HAL_ADC_CLEAR_FLAG(&hadc1, ADC_FLAG_OVR);
    // __HAL_ADC_CLEAR_FLAG(&hadc2, ADC_FLAG_JEOC);
    // __HAL_ADC_CLEAR_FLAG(&hadc2, ADC_FLAG_OVR);
    HAL_ADC_Start_DMA(&hadc2, reinterpret_cast<uint32_t *>(adc_measurements_),3);
    HAL_ADCEx_InjectedStart(&hadc1);

}


bool fetch_and_reset_adcs(std::optional<Iph_ABC_t> *current)
{
    // bool all_adcs_done = (ADC1->SR & (ADC_SR_EOC | ADC_SR_JEOC)) == (ADC_SR_EOC | ADC_SR_JEOC);
    // if (!all_adcs_done)
    //     return false;

    vbus_sense_adc_cb(adc_measurements_[0]);


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

    motor.pwm_update_cb();
}
