#include "stm32_gpio.h"



bool STM32_GPIO::config(uint32_t mode, uint32_t pull, uint32_t speed)
{
    if (port_ == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (port_ == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (port_ == GPIOC)
        __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (port_ == GPIOD)
        __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (port_ == GPIOE)
        __HAL_RCC_GPIOE_CLK_ENABLE();
    else if (port_ == GPIOF)
        __HAL_RCC_GPIOF_CLK_ENABLE();
    else if (port_ == GPIOG)
        __HAL_RCC_GPIOG_CLK_ENABLE();
    else if (port_ == GPIOH)
        __HAL_RCC_GPIOH_CLK_ENABLE();
    else
        return false;

    size_t position = get_pin_number();

    uint32_t temp = port_->MODER;
    temp &= ~(GPIO_MODER_MODER0 << (position * 2U));
    temp |= ((mode & GPIO_MODE) << (position * 2U));
    port_->MODER = temp;

    /* In case of Output or Alternate function mode selection */
    if((mode == GPIO_MODE_OUTPUT_PP) || (mode == GPIO_MODE_AF_PP) ||
       (mode == GPIO_MODE_OUTPUT_OD) || (mode == GPIO_MODE_AF_OD))
    {
        /* Check the Speed parameter */
        assert_param(IS_GPIO_SPEED(speed));
        /* Configure the IO Speed */
        temp = port_->OSPEEDR;
        temp &= ~(GPIO_OSPEEDER_OSPEEDR0 << (position * 2U));
        temp |= (speed << (position * 2U));
        port_->OSPEEDR = temp;

        /* Configure the IO Output Type */
        temp = port_->OTYPER;
        temp &= ~(GPIO_OTYPER_OT_0 << position) ;
        temp |= (((mode & STM32_GPIO_OUTPUT_TYPE) >> 4U) << position);
        port_->OTYPER = temp;
    }

    /* Activate the Pull-up or Pull down resistor for the current IO */
    temp = port_->PUPDR;
    temp &= ~(GPIO_PUPDR_PUPDR0 << (position * 2U));
    temp |= ((pull) << (position * 2U));
    port_->PUPDR = temp;

    return true;
}


uint16_t STM32_GPIO::get_pin_number()
{
    uint16_t pin_number = 0;
    uint16_t pin_mask = pin_mask_ >> 1;
    while (pin_mask)
    {
        pin_mask >>= 1;
        pin_number++;
    }
    return pin_number;
}
