#pragma once

#include "main/hal/pwm/pwm_interface.h"

namespace rover::tests::hal
{

class PwmMock : public rover::hal::PwmInterface
{
   public:
    void set_speed_(float speed) override;
    void turn_off_() override;

    float last_set_speed = 0.0f;
    int set_speed_call_count = 0;
    int turn_off_call_count = 0;
};

}  // namespace rover::tests::hal