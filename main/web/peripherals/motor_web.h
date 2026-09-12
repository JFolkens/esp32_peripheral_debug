#pragma once

#include <string>

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
    void handle_update(const std::map<std::string, std::string> &parameters) override;

   private:
    rover::hal::MotorInterface *motor;
};
}  // namespace  rover::web