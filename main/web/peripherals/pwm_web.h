#pragma once

#include <string>
#include <vector>

#include "../../hal/pwm/pwm_interface.h"
#include "peripheral_interface.h"

namespace rover::web
{

class PwmWeb : public PeripheralInterface
{
   public:
    PwmWeb(const std::string &name_, rover::hal::PwmInterface *pwm_);

    std::string html_state() const override;
    std::string html_control() const override;
    std::vector<EndpointDefinition> endpoints() const override;
    void handle_action(const rover::hal::Request &action) override;

   private:
    rover::hal::PwmInterface *pwm;
};

}  // namespace rover::web