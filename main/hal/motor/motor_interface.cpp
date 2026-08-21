#include "motor_interface.h"

namespace rover::hal
{

void MotorInterface::set_speed(float speed)
{
    // Store state and call child implementation
    speed_ = speed;
    set_speed_(speed);
}

float MotorInterface::get_speed() const
{
    return speed_;
}

}  // namespace rover::hal