#pragma once

#include "main/hal/led/led_interface.h"

namespace rover::hal
{
class LedMock : public LedInterface
{
   public:
    void set(bool val) override;
    bool get() const override;
}
}  // namespace rover::hal