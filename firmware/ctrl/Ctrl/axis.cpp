#include "main_help.h"

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

void Axis::run_state_machine_loop()
{


}




