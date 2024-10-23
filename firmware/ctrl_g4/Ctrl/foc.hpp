#ifndef __FOC_HPP__
#define __FOC_HPP__
#include "phase_control_law.hpp"
#include "component.hpp"


class FOC : public AlphaBetaFrameController, public ComPonentBase
{
public:

    void update() final;

    void reset() final;

    bool on_measurement(
        std::optional<float> vbus_voltage,
        std::optional<float2D> Ialpha_beta
        ) final;

    bool get_alpha_beta_output(
        std::optional<float2D> *mod_alpha_beta,
        std::optional<float> *ibus) final;


    std::optional<float2D> pi_gains_;
    float I_measured_report_filter_k_ = 1.0f;

    // Inputs
    bool enable_current_control_src_ = false;
    InputPort<float2D> Idq_setpoint_src_;
    InputPort<float2D> Vdq_setpoint_src_;
    InputPort<float> phase_src_;
    InputPort<float> phase_vel_src_;

    uint32_t ctrl_timestamp_; // [HCLK ticks]
    bool enable_current_control_ = false; // true: FOC runs in current control mode using I{dq}_setpoint, false: FOC runs in voltage control mode using V{dq}_setpoint
    std::optional<float2D> Idq_setpoint_; // [A] only used if enable_current_control_ == true
    std::optional<float2D> Vdq_setpoint_; // [V] feed-forward voltage term (or standalone setpoint if enable_current_control_ == false)
    std::optional<float> phase_; // [rad]
    std::optional<float> phase_vel_; // [rad/s]

    // These values (or some of them) are updated inside on_measurement() and get_alpha_beta_output()
    uint32_t i_timestamp_;
    std::optional<float> vbus_voltage_measured_; // [V]
    std::optional<float2D> Ialpha_beta_measured_; // [A, A]
    float Id_measured_; // [A]
    float Iq_measured_; // [A]
    float v_current_control_integral_d_ = 0.0f; // [V]
    float v_current_control_integral_q_ = 0.0f; // [V]
    float final_v_alpha_ = 0.0f; // [V]
    float final_v_beta_ = 0.0f; // [V]
    float power_ = 0.0f; // [W] dot product of Vdq and Idq
};



#endif
