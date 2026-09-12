#include "motor_l298n.h"

#include <cmath>

namespace rover::hal
{

MotorL298n::MotorL298n(std::unique_ptr<GpioInterface> forward,
                       std::unique_ptr<GpioInterface> reverse,
                       std::unique_ptr<PwmInterface> speed_control)
    : _forward(std::move(forward)),
      _reverse(std::move(reverse)),
      _speed_control(std::move(speed_control))
{
    // Initialize hardware to speed=0
    set_speed(0);
}

void MotorL298n::stop()
{
    // Setting PWM to zero will coast.
    // Setting _forward and _reverse to the same value will
    // "hard brake" the motor.
    _forward->set(false);
    _reverse->set(false);
}

void MotorL298n::set_speed_(float speed)
{
    if (speed >= 0.0f) {
        _forward->set(true);
        _reverse->set(false);
    } else {
        _forward->set(false);
        _reverse->set(true);
    }

    // Motor is [-1, 1] and PWM is [0, 100]
    // Gpio pins control direction, hence `abs`
    _speed_control->set_speed(std::abs(speed * 100.0f));
}

}  // namespace rover::hal