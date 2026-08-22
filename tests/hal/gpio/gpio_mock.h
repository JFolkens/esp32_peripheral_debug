#pragma once

#include <gmock/gmock.h>

#include "main/hal/gpio/gpio_interface.h"

namespace rover::tests::hal
{

class GpioMock : public rover::hal::GpioInterface
{
   public:
    explicit GpioMock(const rover::hal::GpioDirection &direction_ =
                          rover::hal::GpioDirection::OUTPUT);

    void _set(bool val) override;
    bool _get() const override;
};

}  // namespace rover::tests::hal
