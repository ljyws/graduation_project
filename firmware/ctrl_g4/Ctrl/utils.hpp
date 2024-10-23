#ifndef __UTILS_H__
#define __UTILS_H__

#include "cstdint"
#include "stm32g4xx.h"
#include <tuple>

#ifdef M_PI
#undef M_PI
#endif

// Math Constants
#define M_PI (3.14159265358979323846f)
#define one_by_sqrt3 (0.57735026919f)
#define two_by_sqrt3 (1.15470053838f)
#define sqrt3_by_2 (0.86602540378f)

__attribute__((optimize("-fno-finite-math-only")))
inline bool is_nan(float x) {
    return __builtin_isnan(x);
}

// template<typename T>
// constexpr T SQ(T& x){
//     return x * x;
// }
// Round to integer
// Default rounding mode: round to nearest
inline int round_int(float x) {
#ifdef __arm__
    int res;
    asm("vcvtr.s32.f32   %[res], %[x]"
        : [res] "=X" (res)
        : [x] "w" (x) );
    return res;
#else
    return (int)nearbyint(x);
#endif
}

inline float wrap_pm(float x, float y) {
#ifdef FPU_FPV4
    float intval = (float)round_int(x / y);
#else
    float intval = nearbyintf(x / y);
#endif
    return x - intval * y;
}

// Same as fmodf but result is positive and y must be positive
inline float fmodf_pos(float x, float y) {
    float res = wrap_pm(x, y);
    if (res < 0) res += y;
    return res;
}

inline float wrap_pm_pi(float x) {
    return wrap_pm(x, 2 * M_PI);
}


int mod(const int dividend, const int divisor);
uint32_t micros(void);
void delay_us(uint32_t us);
std::tuple<float, float, float, bool> SVM(float alpha, float beta);

#endif
