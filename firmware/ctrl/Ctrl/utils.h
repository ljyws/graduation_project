#ifndef __UTILS_H__
#define __UTILS_H__

#include "cstdint"
#include "stm32f4xx.h"
#include <tuple>
#include "board.h"

#ifdef M_PI
#undef M_PI
#endif

// Math Constants
constexpr float M_PI = 3.14159265358979323846f;
constexpr float one_by_sqrt3 = 0.57735026919f;
constexpr float two_by_sqrt3 = 1.15470053838f;
constexpr float sqrt3_by_2 = 0.86602540378f;


int mod(const int dividend, const int divisor);
float fmodf_pos(float x, float y);
float wrap_pm(float x, float pm_range);
float wrap_pm_pi(float x);
uint32_t micros(void);
void delay_us(uint32_t us);
std::tuple<float, float, float, bool> SVM(float alpha, float beta);

#endif
