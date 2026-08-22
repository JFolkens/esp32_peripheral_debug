#include "pwm_interface.h"

namespace rover::hal
{

void PwmInterface::set_speed(float speed)
{
    is_on_ = true;

    if (speed <= 0.01f) {
        // Coerce small values to zero
        turn_off();
    } else {
        // Store state and call child class implementation
        speed_ = speed;
        set_speed_(speed);
    }
}

void PwmInterface::turn_off()
{
    speed_ = 0;
    is_on_ = false;
    turn_off_();
}

/* Interface means subclasses don't need to each track state */
float PwmInterface::get_speed()
{
    return speed_;
}

bool PwmInterface::is_on()
{
    return is_on_;
}
}  // namespace rover::hal