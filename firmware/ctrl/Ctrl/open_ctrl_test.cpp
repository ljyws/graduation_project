#include "open_ctrl_test.h"
#include "board.h"
#include "main_help.h"

float acc_ = 0.005f;
float p_e = 0.0f;
//		mop.p_e += moc.pos_acc;
//		wrap_0_2pi(mop.p_e);
//		foc_volt(2,0,mop.p_e);
void test_foc_volt()
{
    encoder.shadow_count_ = encoder.count_in_cpr_;
    const float start_lock_duration = 1.0f;

    axis.open_loop_controller_.Idq_setpoint_ = {0.0f, 0.0f};
    axis.open_loop_controller_.Vdq_setpoint_ = {0.0f, 0.0f};
    axis.open_loop_controller_.phase_ = 0.0f;
    axis.open_loop_controller_.phase_vel_ = 0.0f;

    float max_current_ramp = axis.motor_.config_.calibration_current / start_lock_duration * 2.0f;
    axis.open_loop_controller_.max_current_ramp_ = max_current_ramp;
    axis.open_loop_controller_.max_voltage_ramp_ = max_current_ramp;
    axis.open_loop_controller_.max_phase_vel_ramp_ = INFINITY;
    axis.open_loop_controller_.target_current_ =  0.0f;
    axis.open_loop_controller_.target_voltage_ =  0.0f;
    axis.open_loop_controller_.target_vel_ = 0.0f;
    axis.open_loop_controller_.total_distance_ = 0.0f;
    axis.open_loop_controller_.phase_ = axis.open_loop_controller_.initial_phase_ = wrap_pm_pi(0 - (16.0f * M_PI) / 2.0f);

    axis.motor_.current_control_.enable_current_control_src_ = true;
    axis.motor_.current_control_.Idq_setpoint_src_.connect_to(&axis.open_loop_controller_.Idq_setpoint_);
    axis.motor_.current_control_.Vdq_setpoint_src_.connect_to(&axis.open_loop_controller_.Vdq_setpoint_);
    axis.motor_.current_control_.phase_src_.connect_to(&axis.open_loop_controller_.phase_);


    axis.motor_.phase_vel_src_.connect_to(&axis.open_loop_controller_.phase_vel_);
    axis.motor_.current_control_.phase_vel_src_.connect_to(&axis.open_loop_controller_.phase_vel_);

     axis.motor_.arm(& axis.motor_.current_control_);
    int32_t init_enc_val = encoder.shadow_count_;
    uint32_t num_steps = 0;
    int64_t encvaluesum = 0;
    //
    axis.open_loop_controller_.target_vel_ = 4.0f * M_PI;;
    axis.open_loop_controller_.total_distance_ = 0.0f;

    // while ((axis.requested_state_ == Axis::AXIS_STATE_UNDEFINED) && axis.motor_.is_armed_)
    while (axis.motor_.is_armed_)
    {
        bool reached_target_dist = axis.open_loop_controller_.total_distance_.any().value_or(-INFINITY) >= (16.0f * M_PI);
        if (reached_target_dist)
        {
            break;
        }
        encvaluesum += encoder.shadow_count_;
        num_steps++;
        osDelay(1);
    }
}

