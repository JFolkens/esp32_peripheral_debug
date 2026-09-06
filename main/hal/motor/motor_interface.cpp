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

void MotorInterface::stop()
{
    // Default implementation is `set_speed(0)`.
    // Some motors have a more aggressive "stop" functionality.
    set_speed(0);
}

}  // namespace rover::hal