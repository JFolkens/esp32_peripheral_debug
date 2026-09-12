#pragma once

#include <memory>
#include <string>

#include "../../hal/pwm/pwm_interface.h"
#include "peripheral_interface.h"

namespace rover::web
{

class PwmWeb : public PeripheralInterface
{
   public:
    PwmWeb(const std::string &name_, std::unique_ptr<rover::hal::PwmInterface> pwm_);

    std::string html_state() const override;
    std::string html_control() const override;
    void handle_update(const std::map<std::string, std::string> &parameters) override;

   private:
    std::unique_ptr<rover::hal::PwmInterface> pwm;
};

}  // namespace rover::web