#include "motor.hpp"

#include <pstl/utils.h>
#include <sys/stat.h>

#include "axis.hpp"
#include "main_help.h"


static constexpr auto CURRENT_ADC_LOWER_BOUND =        (uint32_t)((float)(1 << 12) * CURRENT_SENSE_MIN_VOLT / 3.3f);
static constexpr auto CURRENT_ADC_UPPER_BOUND =        (uint32_t)((float)(1 << 12) * CURRENT_SENSE_MAX_VOLT / 3.3f);



struct ResistanceMeasurementControlLaw : AlphaBetaFrameController
{
    void reset() final
    {
        test_voltage_ = 0.0f;
        test_mod_ = std::nullopt;
    }

    bool on_measurement(
        std::optional<float> vbus_voltage,
        std::optional<float2D> Ialpha_beta) final
    {
        if(Ialpha_beta.has_value())
        {
            actual_current_ = Ialpha_beta->first;
            test_voltage_ += (kI *current_meas_period) * (target_current_ - actual_current_);
            I_beta_ += (kIBetaFilt * current_meas_period) * (Ialpha_beta->second - I_beta_);
        }else
        {
            actual_current_ = 0.0f;
            test_voltage_ = 0.0f;
        }

        if(std::abs(test_voltage_) > max_voltage_)
        {
            test_voltage_ = NAN;
            return true;
        }else if(!vbus_voltage.has_value())
        {
            return  true;
        }else
        {
            float vfactor = 1.0f / ((2.0f / 3.0f) * *vbus_voltage);
            test_mod_ = test_voltage_ * vfactor;
            return false;
        }
    }

    bool get_alpha_beta_output(
        std::optional<float2D> *mod_alpha_beta,
        std::optional<float> *ibus) final
    {
        if (!test_mod_.has_value())
        {
            return true;
        } else
        {
            *mod_alpha_beta = {*test_mod_, 0.0f};
            *ibus = *test_mod_ * actual_current_;
            return false;
        }
    }

    float get_resistance()
    {
        return test_voltage_ / target_current_;
    }

    float get_Ibeta()
    {
        return I_beta_;
    }

    const float kI = 1.0f; // [(V/s)/A]
    const float kIBetaFilt = 80.0f;
    float max_voltage_ = 0.0f;
    float actual_current_ = 0.0f;
    float target_current_ = 0.0f;
    float test_voltage_ = 0.0f;
    float I_beta_ = 0.0f; // [A] low pass filtered Ibeta response
    std::optional<float> test_mod_ = NAN;
};


struct InductanceMeasurementControlLaw : AlphaBetaFrameController
{
    void reset() final
    {
        attached_ = false;
    }

    bool on_measurement(
            std::optional<float> vbus_voltage,
            std::optional<float2D> Ialpha_beta) final
    {
        if (!Ialpha_beta.has_value())
        {
            return true;
        }

        float Ialpha = Ialpha_beta->first;

        if (attached_)
        {
            float sign = test_voltage_ >= 0.0f ? 1.0f : -1.0f;
            deltaI_ += -sign * (Ialpha - last_Ialpha_);
        } else
        {
            attached_ = true;
        }

        last_Ialpha_ = Ialpha;

        return false;
    }

    bool get_alpha_beta_output(
            std::optional<float2D>* mod_alpha_beta,
            std::optional<float>* ibus) final
    {
        test_voltage_ *= -1.0f;
        // float vfactor = 1.0f / ((2.0f / 3.0f) * vbus_voltage);
        float vfactor = 1.0f / ((2.0f / 3.0f) * 12.0f);
        *mod_alpha_beta = {test_voltage_ * vfactor, 0.0f};
        *ibus = 0.0f;
        return false;
    }

    float get_inductance()
    {
        float dt = (float)(last_input_timestamp_ - start_timestamp_) / (float)TIM_1_8_CLOCK_HZ; // at 216MHz this overflows after 19 seconds
        return std::abs(test_voltage_) / (deltaI_ / dt);
    }

    // Config
    float test_voltage_ = 0.0f;

    // State
    bool attached_ = false;
    float sign_ = 0;

    // Outputs
    uint32_t start_timestamp_ = 0;
    float last_Ialpha_ = NAN;
    uint32_t last_input_timestamp_ = 0;
    float deltaI_ = 0.0f;
};

Motor::Motor(TIM_HandleTypeDef *timer,
        uint8_t current_sensor_mask,
        float shunt_conductance,
        DRV8301 driver)
            :
            timer_(timer),
            current_sensor_mask_(current_sensor_mask),
            shunt_conductance_(shunt_conductance),
            driver_(driver)
{
    apply_config();
}


bool Motor::arm(PhaseControlLaw<3> *control_law)
{
    control_law_ = control_law;
    if(control_law_) {
        control_law_->reset();
    }

    return true;
}

void Motor::apply_pwm_timings(uint16_t timings[3], bool tentative)
{
    TIM_HandleTypeDef *htim = timer_;
    TIM_TypeDef *tim = htim->Instance;

    // tim->CCR1 = timings[0];
    // tim->CCR2 = timings[1];
    // tim->CCR3 = timings[2];


}


bool Motor::disarm(bool *p_was_armed)
{
    bool was_armed;

    was_armed = is_armed_;
    is_armed_ = false;

    TIM_HandleTypeDef *timer = timer_;
    timer->Instance->BDTR &= ~TIM_BDTR_AOE;
    __HAL_TIM_MOE_DISABLE_UNCONDITIONALLY(timer);
    control_law_ = nullptr;


    if(p_was_armed)
        *p_was_armed = was_armed;

    return true;
}

void Motor::update_current_controller_gains()
{
    float p_gain = config_.current_control_bandwidth * config_.phase_inductance;
    float plant_pole = config_.phase_resistance / config_.phase_inductance;
    current_control_.pi_gains_ = {p_gain, plant_pole * p_gain};
}

bool Motor::apply_config()
{
    config_.parent = this;
    is_calibrated = config_.pre_calibrated;
    update_current_controller_gains();
    return true;
}

bool Motor::init()
{
    constexpr float kMargin = 0.90f;
    constexpr float max_output_swing = 1.35f; // [V] out of amplifier
    float max_unity_gain_current = kMargin * max_output_swing * shunt_conductance_; // [A]
    float requested_gain = max_unity_gain_current / config_.requested_current_range; // [V/V]

    float actual_gain;
    if(!driver_.config(requested_gain,&actual_gain))
        return false;

    phase_current_rev_gain_ = 1.0f / actual_gain;
    // Clip all current control to actual usable range
    max_allowed_current_ = max_unity_gain_current * phase_current_rev_gain_;

    max_dc_calib_ = 0.1f * max_allowed_current_;

    if(!driver_.init())
        return false;

    return true;
}

std::optional<float> Motor::phase_current_from_adcval(uint32_t adc_value)
{
    if(adc_value < CURRENT_ADC_LOWER_BOUND || adc_value > CURRENT_ADC_UPPER_BOUND)
    {
        return std::nullopt;
    }

    int adcval_bal = (int)adc_value - (2048);
    float amp_out_volt = (3.3f / (float)(4096)) * (float)adcval_bal;
    float shunt_volt = amp_out_volt * phase_current_rev_gain_;
    float current = shunt_volt * shunt_conductance_;
    return current;
}



bool Motor::measure_phase_resistance(float test_current, float max_voltage)
{
    ResistanceMeasurementControlLaw control_law;
    control_law.target_current_ = test_current;
    control_law.max_voltage_ = max_voltage;

    arm(&control_law);
    for (size_t i = 0; i < 3000; ++i)
    {
        osDelay(1);
    }
    bool success = is_armed_;
    disarm();

    config_.phase_resistance = control_law.get_resistance();
    if (is_nan(config_.phase_resistance))
    {
        success = false;
    }

    float I_beta = control_law.get_Ibeta();
    if (is_nan(I_beta) || (abs(I_beta) / test_current) > 0.1f)
    {
        success = false;
    }

    return success;
}


bool Motor::measure_phase_inductance(float test_voltage)
{
    InductanceMeasurementControlLaw control_law;
    control_law.test_voltage_ = test_voltage;

    arm(&control_law);

    for (size_t i = 0; i < 1250; ++i)
    {
        osDelay(1);
    }

    bool success = is_armed_;

    disarm();

    config_.phase_inductance = control_law.get_inductance();

    // TODO arbitrary values set for now
    if (!(config_.phase_inductance >= 2e-6f && config_.phase_inductance <= 4000e-6f))
    {
        success = false;
    }

    return success;
}

bool Motor::run_calibration()
{
    float R_calib_max_voltage = config_.resistance_calib_max_voltage;
        if (!measure_phase_resistance(config_.calibration_current, R_calib_max_voltage))
            return false;
        if (!measure_phase_inductance(R_calib_max_voltage))
            return false;

    update_current_controller_gains();

    is_calibrated_ = true;
    return true;
}


void Motor::current_meas_cb(std::optional<Iph_ABC_t> current)
{
    bool dc_calib_valid = (dc_calib_running_since_ >= config_.dc_calib_tau * 7.5f)
                       && (abs(dc_calib_.phA) < max_dc_calib_)
                       && (abs(dc_calib_.phB) < max_dc_calib_)
                       && (abs(dc_calib_.phC) < max_dc_calib_);

    if (armed_state_ == 1 || armed_state_ == 2)
    {
        current_meas_ = {0.0f, 0.0f, 0.0f};
        armed_state_ += 1;
    } else if (current.has_value() && dc_calib_valid)
    {
        current_meas_ =
        {
            current->phA - dc_calib_.phA,
            current->phB - dc_calib_.phB,
            current->phC - dc_calib_.phC
        };
    } else
    {
        current_meas_ = std::nullopt;
    }

    if(current_meas_.has_value())
    {
        float Itrip = effective_current_lim_ + config_.current_lim_margin;
        float Inorm_sq = 2.0f / 3.0f * (SQ(current_meas_->phA)
                                      + SQ(current_meas_->phB)
                                      + SQ(current_meas_->phC));

        if(Inorm_sq > SQ(Itrip)) {

        }else if(is_armed_)
        {}

        if(control_law_)
        {
            control_law_->on_measurement(vbus_voltage,
                                            current_meas_.has_value()?
                                            std::make_optional(std::array<float,3>{current_meas_->phA, current_meas_->phB, current_meas_->phC}):
                                            std::nullopt);
        }
    }
}

void Motor::dc_calib_cb(std::optional<Iph_ABC_t> current)
{
    const float dc_calib_period = static_cast<float>(2 * TIM_1_8_PERIOD_CLOCKS) / TIM_1_8_CLOCK_HZ;


    if (current.has_value())
    {
        const float calib_filter_k = std::min(dc_calib_period / config_.dc_calib_tau, 1.0f);
        dc_calib_.phA += (current->phA - dc_calib_.phA) * calib_filter_k;
        dc_calib_.phB += (current->phB - dc_calib_.phB) * calib_filter_k;
        dc_calib_.phC += (current->phC - dc_calib_.phC) * calib_filter_k;
        dc_calib_running_since_ += dc_calib_period;
    } else {
        dc_calib_.phA = 0.0f;
        dc_calib_.phB = 0.0f;
        dc_calib_.phC = 0.0f;
        dc_calib_running_since_ = 0.0f;
    }
}


void Motor::pwm_update_cb()
{
    float pwm_timings[3] = {NAN, NAN, NAN};
    std::optional<float> i_bus;

    error_e control_law_status = ERROR_CONTROLLER_FAILED;

    if(control_law_)
        control_law_->get_output(pwm_timings,&i_bus);

    if(is_armed_)
    {
        uint16_t next_pwm_timings[] = {
            (uint16_t)(pwm_timings[0] * (float)TIM_1_8_PERIOD_CLOCKS),
            (uint16_t)(pwm_timings[1] * (float)TIM_1_8_PERIOD_CLOCKS),
            (uint16_t)(pwm_timings[2] * (float)TIM_1_8_PERIOD_CLOCKS)
        };
        apply_pwm_timings(next_pwm_timings,false);
    }

    if(!is_armed_)
    {
        i_bus = 0.0f;
    }else if(is_armed_ && !i_bus.has_value())
    {
        i_bus = 0.0f;
    }

    I_bus_ = *i_bus;

    if(*i_bus < config_.I_bus_hard_min || *i_bus > config_.I_bus_hard_max)
    {}
}



void Motor::update()
{
    std::optional<float> maybe_torque = torque_setpoint_src_.present();
    if (!maybe_torque.has_value())
    {
        return;
    }
    float torque = direction_ * *maybe_torque;

    auto [id,iq] = Idq_setpoint_.previous().value_or((float2D){0.0f,0.0f});

    float i_lim = axis.motor_.effective_current_lim_;

    id = std::clamp(id, -i_lim*0.99f, i_lim*0.99f);

    iq = torque / axis_->motor_.config_.torque_constant;

    float iq_lim_sqr = SQ(i_lim) - SQ(id);
    float Iq_lim = (iq_lim_sqr <= 0.0f) ? 0.0f : sqrt(iq_lim_sqr);
    iq = std::clamp(iq, -Iq_lim, Iq_lim);

    float vd = 0.0f;
    float vq = 0.0f;

    std::optional<float> phase_vel = phase_vel_src_.present();

    vd -= *phase_vel * config_.phase_inductance * iq;
    vq += *phase_vel * config_.phase_inductance * id;
    vd += config_.phase_resistance * id;
    vq += config_.phase_resistance * iq;

    Vdq_setpoint_ = {vd, vq};
}















