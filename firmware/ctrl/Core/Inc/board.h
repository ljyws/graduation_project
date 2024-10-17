#ifndef _BOARD_H__
#define _BOARD_H__

#include <stdbool.h>
#include <string>

#include <stm32f4xx_hal.h>
#include <gpio.h>
#include <spi.h>
#include <dma.h>
#include <tim.h>
#include <adc.h>
#include <usb_device.h>
#include <main.h>
#include "cmsis_os.h"
#include <arm_math.h>



#ifdef __cplusplus

#include "utils.hpp"
#include "stm32_gpio.h"
#include "stm32_spi.h"
#include "drv8301.hpp"

using TGateDriver = DRV8301;
using TOpAmp = DRV8301;


#include "motor.hpp"


#endif

#define TIM_TIME_BASE TIM14

// Period in [s]
#define CURRENT_MEAS_PERIOD ( (float)2*TIM_1_8_PERIOD_CLOCKS*(TIM_1_8_RCR+1) / (float)TIM_1_8_CLOCK_HZ )
static const float current_meas_period = CURRENT_MEAS_PERIOD;

// Frequency in [Hz]
#define CURRENT_MEAS_HZ ( (float)(TIM_1_8_CLOCK_HZ) / (float)(2*TIM_1_8_PERIOD_CLOCKS*(TIM_1_8_RCR+1)) )
static const int current_meas_hz = CURRENT_MEAS_HZ;

void system_init();
bool board_init();
void start_timers();


#endif
