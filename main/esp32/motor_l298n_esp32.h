#pragma once

#include <memory>

#include "../hal/motor/motor_interface.h"

extern "C" {
#include "driver/gpio.h"
#include "driver/ledc.h"
}

namespace rover::hal
{

std::unique_ptr<MotorInterface> MotorL298nEsp32(const gpio_num_t forward_,
                                                const gpio_num_t reverse_, const gpio_num_t pwm_,
                                                ledc_channel_t ch_, ledc_timer_t timer_);

}  // namespace rover::hal