#ifndef __OPEN_LOOP_CONTROLLER_H__
#define __OPEN_LOOP_CONTROLLER_H__

#include "component.hpp"
#include "phase_control_law.hpp"
#include <cmath>

class OpenLoopController : public ComPonentBase
{
public:

    void update() final;

    // Config
    float max_current_ramp_ = INFINITY; // [A/s]
    float max_voltage_ramp_ = INFINITY; // [V/s]
    float max_phase_vel_ramp_ = INFINITY; // [rad/s^2]

    // Inputs
    float target_vel_ = 0.0f;
    float target_current_ = 0.0f;
    float target_voltage_ = 0.0f;
    float initial_phase_ = 0.0f;

    OutputPort<float2D> Idq_setpoint_ = {{0.0f, 0.0f}};
    OutputPort<float2D> Vdq_setpoint_ = {{0.0f, 0.0f}};
    OutputPort<float> phase_ = 0.0f;
    OutputPort<float> phase_vel_ = 0.0f;
    OutputPort<float> total_distance_ = 0.0f;
};


#endif
