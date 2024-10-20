#ifndef __AXIS_H__
#define __AXIS_H__

class Axis;

#include "encoder.hpp"
#include "open_loop_controller.hpp"
#include "utils.hpp"
#include <array>

class Axis
{
public:

    typedef enum {
        ERROR_NONE                       = 0x00000000,
        ERROR_INVALID_STATE              = 0x00000001,
        ERROR_MOTOR_FAILED               = 0x00000040,
        ERROR_SENSORLESS_ESTIMATOR_FAILED = 0x00000080,
        ERROR_ENCODER_FAILED             = 0x00000100,
        ERROR_CONTROLLER_FAILED          = 0x00000200,
        ERROR_WATCHDOG_TIMER_EXPIRED     = 0x00000800,
        ERROR_MIN_ENDSTOP_PRESSED        = 0x00001000,
        ERROR_MAX_ENDSTOP_PRESSED        = 0x00002000,
        ERROR_ESTOP_REQUESTED            = 0x00004000,
        ERROR_HOMING_WITHOUT_ENDSTOP     = 0x00020000,
        ERROR_OVER_TEMP                  = 0x00040000,
        ERROR_UNKNOWN_POSITION           = 0x00080000,
    }error_e;

    typedef enum {
        AXIS_STATE_UNDEFINED             = 0,
        AXIS_STATE_IDLE                  = 1,
        AXIS_STATE_STARTUP_SEQUENCE      = 2,
        AXIS_STATE_FULL_CALIBRATION_SEQUENCE = 3,
        AXIS_STATE_MOTOR_CALIBRATION     = 4,
        AXIS_STATE_ENCODER_INDEX_SEARCH  = 6,
        AXIS_STATE_ENCODER_OFFSET_CALIBRATION = 7,
        AXIS_STATE_CLOSED_LOOP_CONTROL   = 8,
        AXIS_STATE_LOCKIN_SPIN           = 9,
        AXIS_STATE_ENCODER_DIR_FIND      = 10,
        AXIS_STATE_HOMING                = 11,
        AXIS_STATE_ENCODER_HALL_POLARITY_CALIBRATION = 12,
        AXIS_STATE_ENCODER_HALL_PHASE_CALIBRATION = 13,
    }axis_state_e;


    typedef struct
    {
        bool startup_motor_calibration = false;
        bool startup_encoder_offset_calibration = false;
        bool startup_closed_loop_control = false;

        Axis *parent = nullptr;
    }Config_t;


    Axis(Encoder &encoder, Motor &motor);


    bool apply_config();
    void clear_config();

    void start_thread();
    bool run_closed_loop_control_loop();
    void run_state_machine_loop();

    Config_t config_;
    Encoder& encoder_;
    Motor& motor_;
    OpenLoopController open_loop_controller_;

    volatile bool thread_id_valid_ = false;
    error_e error_ = ERROR_NONE;

    axis_state_e requested_state_ = AXIS_STATE_STARTUP_SEQUENCE;
    std::array<axis_state_e, 10> task_chain_ = {AXIS_STATE_UNDEFINED};
    axis_state_e &current_state_ = task_chain_.front();


};



#endif

