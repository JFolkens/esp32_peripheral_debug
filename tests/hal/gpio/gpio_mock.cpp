#include "gpio_mock.h"

namespace rover::tests::hal
{

GpioMock::GpioMock(const rover::hal::GpioDirection &direction_)
    : rover::hal::GpioInterface(direction_)
{
    // Empty
}

void GpioMock::_set(bool val)
{
    state = val;
}

bool GpioMock::_get() const
{
    return state;
}

}  // namespace rover::tests::hal