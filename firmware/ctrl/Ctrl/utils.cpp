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