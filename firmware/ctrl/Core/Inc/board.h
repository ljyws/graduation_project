#ifndef _BOARD_H__
#define _BOARD_H__

#include <stm32f4xx_hal.h>
#include <gpio.h>
#include <spi.h>
#include <dma.h>
#include <adc.h>
#include <usb_device.h>
#include <main.h>

#include <arm_math.h>
#include <stdbool.h>

#define TIM_TIME_BASE TIM14

// Period in [s]
#define CURRENT_MEAS_PERIOD ( (float)2*TIM_1_8_PERIOD_CLOCKS*(TIM_1_8_RCR+1) / (float)TIM_1_8_CLOCK_HZ )
static const float current_meas_period = 1;

// Frequency in [Hz]
#define CURRENT_MEAS_HZ ( (float)(TIM_1_8_CLOCK_HZ) / (float)(2*TIM_1_8_PERIOD_CLOCKS*(TIM_1_8_RCR+1)) )
static const int current_meas_hz = 1;

void system_init();
bool board_init();
void start_timers();


#endif
