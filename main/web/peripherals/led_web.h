#pragma once

#include <memory>
#include <string>

#include "../../hal/gpio/gpio_interface.h"
#include "peripheral_interface.h"

namespace rover::web
{

class LedWeb : public PeripheralInterface
{
   public:
    LedWeb(const std::string &name_, std::unique_ptr<rover::hal::GpioInterface> led_);

    std::string html_state() const override;
    std::string html_control() const override;
    void handle_update(const std::map<std::string, std::string> &parameters) override;

   private:
    std::unique_ptr<rover::hal::GpioInterface> led;
};

}  // namespace rover::web
