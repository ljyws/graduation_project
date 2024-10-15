#ifndef __STM32_GPIO_H__
#define __STM32_GPIO_H__

#include "gpio.h"
#include "stm32f405xx.h"
#include "stdint-gcc.h"
#include "cstdint"

#define STM32_GPIO_MODE             0x00000003U
#define STM32_GPIO_OUTPUT_TYPE      0x00000010U


class STM32_GPIO
{
public:
    static const STM32_GPIO none;
    STM32_GPIO() : port_(nullptr), pin_mask_(0) {}
    STM32_GPIO(GPIO_TypeDef *port, uint16_t pin) : port_(port), pin_mask_(pin){}


    bool config(uint32_t mode, uint32_t pull, uint32_t speed);

    void write(bool val);

    bool read(void);

    uint16_t get_pin_number(void);

    void attach_gpio(void);

    GPIO_TypeDef *port_;
    uint16_t pin_mask_;

};


#endif

