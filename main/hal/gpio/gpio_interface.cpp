#include "gpio_interface.h"

namespace rover::hal
{

GpioInterface::GpioInterface(const GpioDirection &direction_) : direction(direction_)
{
    // Empty
}

void GpioInterface::set(bool val)
{
    if (direction == GpioDirection::INPUT) {
        // TODO: Throw error
        // throw std::runtime_error("Can not call gpio.set on input gpio.");
    } else {
        _set(val);
        state = val;
    }
}

bool GpioInterface::get() const
{
    if (direction == GpioDirection::INPUT) {
        // GPIO In; query value
        return _get();
    } else {
        // GPIO Out; return whatever value we last SET to
        return state;
    }
}

}  // namespace rover::hal