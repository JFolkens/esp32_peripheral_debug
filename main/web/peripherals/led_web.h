#pragma once

#include <string>
#include <vector>

#include "../../hal/gpio/gpio_interface.h"
#include "peripheral_interface.h"

namespace rover::web
{

class LedWeb : public PeripheralInterface
{
   public:
    LedWeb(const std::string &name_, rover::hal::GpioInterface *led_);

    std::string html_state() const override;
    std::string html_control() const override;
    std::vector<EndpointDefinition> endpoints() const override;
    void handle_action(const rover::hal::Request &action) override;

   private:
    rover::hal::GpioInterface *led;
};

}  // namespace rover::web
