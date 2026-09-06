#pragma once

#include "main/hal/motor/motor_interface.h"

namespace rover::tests::hal
{
class MotorMock : public rover::hal::MotorInterface
{
   public:
    void set_speed_(float speed) override;
    void stop() override;

    float last_set_speed = 0.0f;
    int set_speed_call_count = 0;
    int stop_call_count = 0;
};

}  // namespace
   // rover::tests::hal