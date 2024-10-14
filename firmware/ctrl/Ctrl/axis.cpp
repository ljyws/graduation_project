#include "axis.h"

Axis::Axis(Encoder &encoder)
    : encoder_(encoder)
{
    encoder_.axis_ = this;
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




