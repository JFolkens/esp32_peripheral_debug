#include "gpio_esp32.h"

namespace rover::hal
{

GpioEsp32::GpioEsp32(const GpioDirection &direction_, const gpio_num_t pin_)
    : GpioInterface(direction_), pin(pin_)
{
    gpio_reset_pin(pin);

    if (direction == GpioDirection::INPUT) {
        gpio_set_direction(pin, GPIO_MODE_INPUT);
    } else {
        gpio_set_direction(pin, GPIO_MODE_OUTPUT);
        set(false);  // Initialize to OFF
    }
}

void GpioEsp32::_set(bool val)
{
    gpio_set_level(pin, val);
}

bool GpioEsp32::_get() const
{
    return gpio_get_level(pin);
}

}  // namespace rover::hal
