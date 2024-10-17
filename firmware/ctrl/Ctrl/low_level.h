#ifndef __LOW_LEVEL_H__
#define __LOW_LEVEL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <cmsis_os.h>
#include <stdbool.h>
#include <adc.h>

extern "C" {
    void vbus_sense_adc_cb(uint32_t adc_value);
    void pwm_in_cb(TIM_HandleTypeDef *htim);
}

void start_adc_pwm();


#ifdef __cplusplus
}
#endif

#endif