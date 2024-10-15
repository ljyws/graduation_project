#include "motor.h"

struct ResistanceMeasurementControlLaw : AlphaBetaFrameController
{
    void reset() final
    {
        test_voltage_ = 0.0f;
        test_mod_ = std::nullopt;
    }

    bool on_measurement(
        std::optional<float> vbus_voltage,
        std::optional<float2D> Ialpha_beta,
        uint32_t input_timestamp) final
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
        uint32_t output_timestamp,
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
            std::optional<float2D> Ialpha_beta,
            uint32_t input_timestamp) final
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
            start_timestamp_ = input_timestamp;
            attached_ = true;
        }

        last_Ialpha_ = Ialpha;
        last_input_timestamp_ = input_timestamp;

        return false;
    }

    bool get_alpha_beta_output(
            uint32_t output_timestamp, std::optional<float2D>* mod_alpha_beta,
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

    tim->CCR1 = timings[0];
    tim->CCR2 = timings[1];
    tim->CCR3 = timings[2];


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


















