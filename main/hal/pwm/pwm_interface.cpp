#include "pwm_interface.h"

namespace rover::hal
{

void PwmInterface::set_speed(float speed)
{
    // Store state and call child class implementation
    is_on_ = true;
    speed = speed;
    set_speed_(speed);
}

void PwmInterface::turn_off()
{
    is_on_ = false;
    turn_off_();
}

/* Interface means subclasses don't need to each track state */
float PwmInterface::get_speed()
{
    return speed;
}

bool PwmInterface::is_on()
{
    return is_on_;
}
}  // namespace rover::hal