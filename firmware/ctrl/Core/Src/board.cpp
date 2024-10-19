#include "board.h"
#include "main_help.h"

extern "C" void SystemClock_Config(void);

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
    HAL_TIM_Base_Start_IT(motor.timer_);
}

bool fetch_and_reset_adcs(std::optional<Iph_ABC_t> *current)
{
    bool all_adcs_done = (ADC1->SR & ADC_SR_JEOC) == ADC_SR_JEOC
         && (ADC2->SR & (ADC_SR_EOC | ADC_SR_JEOC)) == (ADC_SR_EOC | ADC_SR_JEOC)
         && (ADC3->SR & (ADC_SR_EOC | ADC_SR_JEOC)) == (ADC_SR_EOC | ADC_SR_JEOC);


}

void control_loop_irq_handler(void)
{
    __HAL_TIM_CLEAR_IT(&htim1,TIM_IT_UPDATE);
    HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_7);

    axis.encoder_.sample_now();


}
