#include "led_mock.h"

namespace rover::hal
{

void LedMock::set(bool val)
{
    state_ = val;
}

bool LedMock::get() const
{
    return state_;
}

}  // namespace rover::hal