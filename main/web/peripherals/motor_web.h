#pragma once

#include <string>
#include <vector>

#include "../../hal/motor/motor_interface.h"
#include "peripheral_interface.h"

namespace rover::web
{

class MotorWeb : public PeripheralInterface
{
   public:
    MotorWeb(const std::string &name_, rover::hal::MotorInterface *motor_);

    std::string html_state() const override;
    std::string html_control() const override;
    std::vector<EndpointDefinition> endpoints() const override;
    void handle_action(const rover::hal::Request &action) override;

   private:
    rover::hal::MotorInterface *motor;
};
}  // namespace
   // rover::web