#ifndef __MOTOR_H__
#define __MOTOR_H__

#include "board.h"
#include "foc.hpp"


class Axis;
class Motor;

class Motor
{
public:
    typedef enum
    {
        ERROR_NONE                              = 0x00000000,
        ERROR_PHASE_RESISTANCE_OUT_OF_RANGE     = 0x00000001,
        ERROR_PHASE_INDUCTANCE_OUT_OF_RANGE     = 0x00000002,
        ERROR_DRV_FAULT                         = 0x00000008,
        ERROR_CONTROL_DEADLINE_MISSED           = 0x00000010,
        ERROR_MODULATION_MAGNITUDE              = 0x00000080,
        ERROR_CURRENT_SENSE_SATURATION          = 0x00000400,
        ERROR_CURRENT_LIMIT_VIOLATION           = 0x00001000,
        ERROR_MODULATION_IS_NAN                 = 0x00010000,
        ERROR_MOTOR_THERMISTOR_OVER_TEMP        = 0x00020000,
        ERROR_FET_THERMISTOR_OVER_TEMP          = 0x00040000,
        ERROR_TIMER_UPDATE_MISSED               = 0x00080000,
        ERROR_CURRENT_MEASUREMENT_UNAVAILABLE   = 0x00100000,
        ERROR_CONTROLLER_FAILED                 = 0x00200000,
        ERROR_I_BUS_OUT_OF_RANGE                = 0x00400000,
        ERROR_BRAKE_RESISTOR_DISARMED           = 0x00800000,
        ERROR_SYSTEM_LEVEL                      = 0x01000000,
        ERROR_BAD_TIMING                        = 0x02000000,
        ERROR_UNKNOWN_PHASE_ESTIMATE            = 0x04000000,
        ERROR_UNKNOWN_PHASE_VEL                 = 0x08000000,
        ERROR_UNKNOWN_TORQUE                    = 0x10000000,
        ERROR_UNKNOWN_CURRENT_COMMAND           = 0x20000000,
        ERROR_UNKNOWN_CURRENT_MEASUREMENT       = 0x40000000,
        ERROR_UNKNOWN_VBUS_VOLTAGE              = 0x80000000,
        ERROR_UNKNOWN_VOLTAGE_COMMAND           = 0x100000000,
        ERROR_UNKNOWN_GAINS                     = 0x200000000,
        ERROR_CONTROLLER_INITIALIZING           = 0x400000000,
        ERROR_UNBALANCED_PHASES                 = 0x800000000,
    }error_e;

    typedef struct
    {
        bool pre_calibrated = false;
        int32_t pole_pairs = 7;
        float resistance_calib_max_voltage = 2.0f; // [V] - You may need to increase this if this voltage isn't sufficient to drive calibration_current through the motor.
        float phase_inductance = 0.0f;        // to be set by measure_phase_inductance
        float phase_resistance = 0.0f;        // to be set by measure_phase_resistance
        float torque_constant = 0.04f;         // [Nm/A] for PM motors, [Nm/A^2] for induction motors. Equal to 8.27/Kv of the motor

        float requested_current_range = 60.0f; // [A]
        float current_control_bandwidth = 1000.0f;  // [rad/s]
        float inverter_temp_limit_lower = 100;
        float inverter_temp_limit_upper = 120;

        Motor *parent = nullptr;

        void set_pre_calibrated(bool val)
        {
            pre_calibrated = val;

        }

    }Config_t;

    Motor(TIM_HandleTypeDef *timer,
    uint8_t current_sensor_mask,
    float shunt_conductance,
        DRV8301 driver);

    bool arm(PhaseControlLaw<3> *control_law);
    void apply_pwm_timings(uint16_t timings[3], bool tentative);
    bool disarm(bool *p_was_armed = nullptr);
    bool apply_config();
    bool init();

    void update_current_controller_gains();
    void disarm_with_error(error_e error);

    bool measure_phase_resistance(float test_current, float max_voltage);
    bool measure_phase_inductance(float test_voltage);

    bool run_calibration();
    void update(uint32_t timestamp);

    void current_meas_cb(uint32_t timestamp, std::optional<Iph_ABC_t> current);
    void dc_calib_cb(uint32_t timestamp, std::optional<Iph_ABC_t> current);
    void pwm_update_cb(uint32_t output_timestamp);

    TIM_HandleTypeDef *const timer_;
    const uint8_t current_sensor_mask_;
    const float shunt_conductance_;
    DRV8301 &driver_;

    Config_t config_;
    Axis *axis_ = nullptr;

    error_e error_ = ERROR_NONE;

    bool is_armed_ = false;
    bool is_calibrated = false;
    std::optional<Iph_ABC_t> current_meas_;
    Iph_ABC_t dc_calib_ = {0.0f, 0.0f, 0.0f};
    float I_bus_ = 0.0f;
    float phase_current_rev_gain_ = 0.0f;
    FOC current_control_;
    float effective_current_lim_ = 10.0f; // [A]
    float max_allowed_current_ = 0.0f; // [A] set in setup()
    float max_dc_calib_ = 0.0f; // [A] set in setup()

    InputPort<float> torque_setpoint_src_; // Usually points to the Controller object's output
    InputPort<float> phase_vel_src_; // Usually points to the Encoder object's output

    float direction_ = 0.0f; // if -1 then positive torque is converted to negative Iq
    OutputPort<float2D> Vdq_setpoint_ = {{0.0f, 0.0f}}; // fed to the FOC
    OutputPort<float2D> Idq_setpoint_ = {{0.0f, 0.0f}}; // fed to the FOC

    PhaseControlLaw<3>* control_law_;




};



#endif
