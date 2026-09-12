#include "motor_l298n_esp32.h"

#include <memory>

#include "../hal/motor/motor_l298n.h"
#include "gpio_esp32.h"
#include "pwm_esp32.h"

namespace rover::hal
{

std::unique_ptr<MotorInterface> MotorL298nEsp32(const gpio_num_t forward_,
                                                const gpio_num_t reverse_, const gpio_num_t pwm_,
                                                ledc_channel_t ch_, ledc_timer_t timer_)
{
    auto forward = std::make_unique<GpioEsp32>(GpioDirection::OUTPUT, forward_);
    auto reverse = std::make_unique<GpioEsp32>(GpioDirection::OUTPUT, reverse_);
    auto pwm = std::make_unique<PwmEsp32>(pwm_, ch_, timer_);

    return std::make_unique<MotorL298n>(std::move(forward), std::move(reverse), std::move(pwm));
}
}  // namespace rover::hal