#include "utils.h"
#include "cmath"


int mod(const int dividend, const int divisor)
{
    int r = dividend % divisor;
    if (r < 0) r += divisor;
    return r;
}

float fmodf_pos(float x, float y)
{
    float out = fmodf(x, y);
    if (out < 0.0f)
        out += y;
    return out;
}

float wrap_pm(float x, float pm_range)
{
    return fmodf_pos(x + pm_range, 2.0f * pm_range) - pm_range;
}

float wrap_pm_pi(float x)
{
    return wrap_pm(x, 2 * M_PI);
}

// @brief: Returns number of microseconds since system startup
uint32_t micros(void)
{
    uint32_t ms, cycle_cnt;
    do {
        ms = HAL_GetTick();
        cycle_cnt = TIM_TIME_BASE->CNT;
    } while (ms != HAL_GetTick());

    return (ms * 1000) + cycle_cnt;
}

// @brief: Busy wait delay for given amount of microseconds (us)
void delay_us(uint32_t us)
{
    uint32_t start = micros();
    while (micros() - start < (uint32_t) us) {
        asm volatile ("nop");
    }
}