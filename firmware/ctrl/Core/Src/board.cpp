#include "board.h"
#include "main_help.h"

extern "C" void SystemClock_Config(void);

STM32_GPIO led_green(LED_GREEN_GPIO_Port,LED_GREEN_Pin);

STM32_SPI spi1_arbiter{&hspi1};
STM32_SPI spi3_arbiter{&hspi3};
Encoder encoder = {&spi1_arbiter};
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

Axis axis = {
    encoder,
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
    MX_ADC1_Init();
    MX_SPI3_Init();
    MX_SPI1_Init();
    MX_TIM1_Init();

    MX_USB_DEVICE_Init();

    return 1;
}


void start_timers()
{
    HAL_TIM_Base_Start_IT(&htim1);
    
}

uint16_t adc_measurements_[12] = {0};
void start_adcs()
{
    HAL_ADC_Start_DMA(&hadc1, reinterpret_cast<uint32_t *>(adc_measurements_),12);
}


bool fetch_and_reset_adcs(std::optional<Iph_ABC_t> *current)
{
    vbus_sense_adc_cb(adc_measurements_[3]);


    std::optional<float> phA = motor.phase_current_from_adcval(adc_measurements_[0]);
    std::optional<float> phB = motor.phase_current_from_adcval(adc_measurements_[1]);
    std::optional<float> phC = motor.phase_current_from_adcval(adc_measurements_[2]);

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
