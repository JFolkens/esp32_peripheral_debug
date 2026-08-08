#pragma once

#include <gmock/gmock.h>

#include "main/hal/gpio/gpio_interface.h"

namespace rover::tests::hal
{

class GpioMock : public rover::hal::GpioInterface
{
   public:
    explicit GpioMock(const rover::hal::GpioDirection &direction_ =
                          rover::hal::GpioDirection::OUTPUT)
        : rover::hal::GpioInterface(direction_)
    {
    }

    void _set(bool val) override
    {
        state = val;
    }

    bool _get() const override
    {
        return state;
    }
};

}  // namespace rover::tests::hal
