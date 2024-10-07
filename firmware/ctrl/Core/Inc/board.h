#ifndef _BOARD_H__
#define _BOARD_H__

#include <stm32f4xx_hal.h>
#include "gpio.h"
#include "spi.h"
#include "dma.h"
#include "adc.h"
#include "usb_device.h"

void system_init();
bool board_init();
void start_timers();


#endif
