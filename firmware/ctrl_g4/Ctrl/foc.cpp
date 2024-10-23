#include "foc.hpp"
#include "board.h"
#include "utils.hpp"

bool AlphaBetaFrameController::on_measurement(
    std::optional<float> vbus_voltage,
    std::optional<std::array<float, 3> > currents)
{
    std::optional<float2D> Ialpha_beta;
    if(currents.has_value())
    {
        Ialpha_beta =
            {
                (*currents)[0],
                one_by_sqrt3 * ((*currents)[1] - (*currents)[2])
            };
    }

    return on_measurement(vbus_voltage, Ialpha_beta);
}

bool AlphaBetaFrameController::get_output(
    float (&pwm_timings)[3],
    std::optional<float> *ibus)
{
    std::optional<float2D> mod_alpha_beta;
    get_alpha_beta_output(&mod_alpha_beta,ibus);

    auto [tA, tB, tC, success] = SVM(mod_alpha_beta->first, mod_alpha_beta->second);

    pwm_timings[0] = tA;
    pwm_timings[1] = tB;
    pwm_timings[2] = tC;

    return 0;
}

void FOC::reset()
{
    v_current_control_integral_d_ = 0.0f;
    v_current_control_integral_q_ = 0.0f;
    vbus_voltage_measured_ = std::nullopt;
    Ialpha_beta_measured_ = std::nullopt;
    power_ = 0.0f;
}

bool FOC::on_measurement(
    std::optional<float> vbus_voltage,
    std::optional<float2D> Ialpha_beta)
{
    vbus_voltage_measured_ = vbus_voltage;
    Ialpha_beta_measured_ = Ialpha_beta;

    return 0;
}


bool FOC::get_alpha_beta_output(
    std::optional<float2D> *mod_alpha_beta,
    std::optional<float> *ibus)
{
    if(!vbus_voltage_measured_.has_value() || !Ialpha_beta_measured_.has_value())
    {
        return 1;
    }

    auto [Vd, Vq] = *Vdq_setpoint_;
    float phase = *phase_;
    float phase_vel = *phase_vel_;
    float vbus_voltage = *vbus_voltage_measured_;

    std::optional<float2D> Idq;

    if (Ialpha_beta_measured_.has_value())
    {
        auto [Ialpha, Ibeta] = *Ialpha_beta_measured_;
        float I_phase = phase + phase_vel * ((float)(int32_t)(i_timestamp_ - ctrl_timestamp_) / (float)TIM_1_8_CLOCK_HZ);
        float c_I = arm_cos_f32(I_phase);
        float s_I = arm_sin_f32(I_phase);
        Idq =
        {
            c_I * Ialpha + s_I * Ibeta,
            c_I * Ibeta - s_I * Ialpha
        };
        Id_measured_ += I_measured_report_filter_k_ * (Idq->first - Id_measured_);
        Iq_measured_ += I_measured_report_filter_k_ * (Idq->second - Iq_measured_);
    } else
    {
        Id_measured_ = 0.0f;
        Iq_measured_ = 0.0f;
    }

    float mod_to_V = (2.0f / 3.0f) * vbus_voltage;
    float V_to_mod = 1.0f / mod_to_V;
    float mod_d;
    float mod_q;

    if (enable_current_control_)
    {
        auto [p_gain, i_gain] = *pi_gains_;
        auto [Id, Iq] = *Idq;
        auto [Id_setpoint, Iq_setpoint] = *Idq_setpoint_;

        float Ierr_d = Id_setpoint - Id;
        float Ierr_q = Iq_setpoint - Iq;

        // Apply PI control (V{d,q}_setpoint act as feed-forward terms in this mode)
        mod_d = V_to_mod * (Vd + v_current_control_integral_d_ + Ierr_d * p_gain);
        mod_q = V_to_mod * (Vq + v_current_control_integral_q_ + Ierr_q * p_gain);

        float mod_scalefactor = 0.80f * sqrt3_by_2 * 1.0f / std::sqrt(mod_d * mod_d + mod_q * mod_q);
        if (mod_scalefactor < 1.0f)
        {
            mod_d *= mod_scalefactor;
            mod_q *= mod_scalefactor;
            // TODO make decayfactor configurable
            v_current_control_integral_d_ *= 0.99f;
            v_current_control_integral_q_ *= 0.99f;
        } else
        {
            v_current_control_integral_d_ += Ierr_d * (i_gain * current_meas_period);
            v_current_control_integral_q_ += Ierr_q * (i_gain * current_meas_period);
        }

    }else
    {
        mod_d = V_to_mod * Vd;
        mod_q = V_to_mod * Vq;
    }

    // Inverse park transform
    float pwm_phase = phase + phase_vel * ((float)(int32_t)ctrl_timestamp_) / (float)TIM_1_8_CLOCK_HZ;
    float c_p = arm_cos_f32(pwm_phase);
    float s_p = arm_sin_f32(pwm_phase);
    float mod_alpha = c_p * mod_d - s_p * mod_q;
    float mod_beta = c_p * mod_q + s_p * mod_d;

    // Report final applied voltage in stationary frame (for sensorless estimator)
    final_v_alpha_ = mod_to_V * mod_alpha;
    final_v_beta_ = mod_to_V * mod_beta;

    *mod_alpha_beta = {mod_alpha, mod_beta};

    if (Idq.has_value())
    {
        auto [Id, Iq] = *Idq;
        *ibus = mod_d * Id + mod_q * Iq;
        power_ = vbus_voltage * (*ibus).value();
    }

    return 0;
}

void FOC::update()
{
    enable_current_control_ = enable_current_control_src_;
    Idq_setpoint_ = Idq_setpoint_src_.present();
    Vdq_setpoint_ = Vdq_setpoint_src_.present();
    phase_ = phase_src_.present();
    phase_vel_ = phase_vel_src_.present();
}







