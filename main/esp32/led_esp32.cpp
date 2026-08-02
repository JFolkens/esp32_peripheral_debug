#include "led_esp32.h"

namespace rover::hal
{
LedEsp32::LedEsp32(const gpio_num_t pin_) : pin(pin_)
{
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    set(false);  // Initialize to OFF
}

void LedEsp32::set(bool val)
{
    state = val;
    gpio_set_level(pin, val);
}

bool LedEsp32::get() const
{
    // Do not use `gpio_get_level`, that is only for input GPIO
    // `state` will track our current status
    return state;
}
}  // namespace rover::hal
