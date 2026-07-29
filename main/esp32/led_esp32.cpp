#include "led_esp32.h"

namespace rover::hal
{
LedEsp32::LedEsp32(const gpio_num_t pin_) : pin(pin_)
{
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);  // Initialize to OFF
}

void LedEsp32::set(bool val)
{
    gpio_set_level(pin, val);
}

bool LedEsp32::get() const
{
    return static_cast<bool>(gpio_get_level(pin));
}
}  // namespace rover::hal
