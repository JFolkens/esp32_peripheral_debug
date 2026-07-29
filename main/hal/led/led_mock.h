#pragma once

#include "led_interface.h"

namespace rover::hal
{

class LedMock : public LedInterface
{
   public:
    void set(bool val) override;
    bool get() const override;

   private:
    bool state_{0};
};

}  // namespace rover::hal