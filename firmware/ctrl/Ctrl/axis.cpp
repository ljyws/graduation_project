#include "main_help.h"
#include<algorithm>
Axis::Axis(Encoder &encoder,Motor &motor)
    : encoder_(encoder),
      motor_(motor)
{
    encoder_.axis_ = this;
    motor_.axis_ = this;
}


bool Axis::apply_config()
{
    config_.parent = this;
    return true;
}

void Axis::clear_config()
{
    config_ = {};
}


bool Axis::run_closed_loop_control_loop()
{

    return true;
}

void Axis::run_state_machine_loop()
{
    for(;;)
    {
        // Load the task chain if a specific request is pending
        if (requested_state_ != AXIS_STATE_UNDEFINED)
        {
            size_t pos = 0;
            if (requested_state_ == AXIS_STATE_STARTUP_SEQUENCE)
            {
                if (config_.startup_motor_calibration)
                    task_chain_[pos++] = AXIS_STATE_MOTOR_CALIBRATION;
                if (config_.startup_encoder_offset_calibration)
                    task_chain_[pos++] = AXIS_STATE_ENCODER_OFFSET_CALIBRATION;
                if (config_.startup_closed_loop_control)
                    task_chain_[pos++] = AXIS_STATE_CLOSED_LOOP_CONTROL;
                task_chain_[pos++] = AXIS_STATE_IDLE;
            }
            else if (requested_state_ == AXIS_STATE_FULL_CALIBRATION_SEQUENCE)
            {
                task_chain_[pos++] = AXIS_STATE_MOTOR_CALIBRATION;
                task_chain_[pos++] = AXIS_STATE_ENCODER_OFFSET_CALIBRATION;
                task_chain_[pos++] = AXIS_STATE_IDLE;
            }
            else if (requested_state_ != AXIS_STATE_UNDEFINED)
            {
                task_chain_[pos++] = requested_state_;
                task_chain_[pos++] = AXIS_STATE_IDLE;
            }
            task_chain_[pos++] = AXIS_STATE_UNDEFINED;  // TODO: bounds checking
            requested_state_ = AXIS_STATE_UNDEFINED;
        }
        bool status;
        switch (current_state_)
        {
            case AXIS_STATE_MOTOR_CALIBRATION:
            {
                status = motor_.run_calibration();
            } break;

            case AXIS_STATE_ENCODER_OFFSET_CALIBRATION: {
                //if (odrv.any_error())
                //    goto invalid_state_label;
                if (!motor_.is_calibrated_)
                    goto invalid_state_label;
                status = encoder_.run_offset_calibration();
            } break;


            default:
            invalid_state_label:
                status = false;  // this will set the state to idle
                break;
        }

        if (!status)
        {
            std::fill(task_chain_.begin(), task_chain_.end(), AXIS_STATE_UNDEFINED);
            current_state_ = AXIS_STATE_IDLE;
        } else {
            //如果status == true ，这里就把队列向前移动一个，也就是current_state = task_chain_[0]
            std::rotate(task_chain_.begin(), task_chain_.begin() + 1, task_chain_.end());
            task_chain_.back() = AXIS_STATE_UNDEFINED;
        }


    }

}







