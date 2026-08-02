#pragma once

#include <string>
#include <vector>

#include "../../hal/led/led_interface.h"
#include "peripheral_interface.h"

namespace rover::web
{

class LedWeb : public PeripheralInterface
{
   public:
    LedWeb(const std::string &name_, rover::hal::LedInterface *led_);

    std::string render_html() const override;
    std::vector<EndpointDefinition> endpoints() const override;
    void handle_action(const std::string &action) override;

   private:
    rover::hal::LedInterface *led;
};

}  // namespace rover::web
