#ifndef __UTILS_H__
#define __UTILS_H__

#include "cstdint"
#include "stm32f4xx.h"

#include "board.h"

int mod(const int dividend, const int divisor);
float fmodf_pos(float x, float y);
float wrap_pm(float x, float pm_range);
float wrap_pm_pi(float x);
uint32_t micros(void);
void delay_us(uint32_t us);

#endif
