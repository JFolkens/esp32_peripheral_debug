#pragma once

#include "../hal/gpio/gpio_interface.h"

extern "C" {
#include "driver/gpio.h"
}

namespace rover::hal
{
class GpioEsp32 : public virtual GpioInterface
{
   public:
    explicit GpioEsp32(const GpioDirection &direction_, const gpio_num_t pin_);

   private:
    void _set(bool val) override;
    bool _get() const override;
    const gpio_num_t pin;
};

}  // namespace rover::hal