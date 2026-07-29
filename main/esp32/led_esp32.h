#pragma once

#include "../hal/led/led_interface.h"

extern "C" {
#include "driver/gpio.h"
}

namespace rover::hal
{
class LedEsp32 : public LedInterface
{
   public:
    LedEsp32(const gpio_num_t pin_);

    void set(bool val) override;
    bool get() const override;

   private:
    const gpio_num_t pin;
};
}  // namespace rover::hal
