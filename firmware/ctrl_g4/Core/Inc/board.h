#ifndef _BOARD_H__
#define _BOARD_H__

#include <stdbool.h>

#include <stm32g4xx_hal.h>
#include <gpio.h>
#include <spi.h>
#include <dma.h>
#include <tim.h>
#include <adc.h>
#include <usb_device.h>
#include "usb_device.h"
#include <usbd_cdc_if.h>
#include <main.h>

#include "cmsis_os.h"

#include "arm_math.h"
#include "vofa.h"

#define SHUNT_RESISTANCE (0.0005)


#ifdef __cplusplus

#include "utils.hpp"
#include "stm32_gpio.h"
#include "stm32_spi.h"
#include "drv8301.hpp"



#include "motor.hpp"
#include "encoder.hpp"


extern Axis axis;
extern Motor motor;
extern Encoder encoder;




#endif

#define TIM_TIME_BASE TIM6

// Period in [s]
#define CURRENT_MEAS_PERIOD ( (float)2*TIM_1_8_PERIOD_CLOCKS / (float)TIM_1_8_CLOCK_HZ )
static const float current_meas_period = CURRENT_MEAS_PERIOD;

// Frequency in [Hz]
#define CURRENT_MEAS_HZ ( (float)(TIM_1_8_CLOCK_HZ) / (float)(2*TIM_1_8_PERIOD_CLOCKS) )
static const int current_meas_hz = CURRENT_MEAS_HZ;

#define VBUS_S_DIVIDER_RATIO 18.72727273f


// Linear range of the DRV8301 opamp output: 0.3V...5.7V. We set the upper limit
// to 3.0V so that it's symmetric around the center point of 1.65V.
#define CURRENT_SENSE_MIN_VOLT  0.3f
#define CURRENT_SENSE_MAX_VOLT  3.0f


extern uint16_t adc_measurements_[3];

void system_init();
bool board_init();
void start_timers();
void start_adcs();

#ifdef __cplusplus
extern "C"{
#endif

void control_loop_irq_handler(void);


#ifdef __cplusplus
}

#endif

#endif
