#include "pwm_mock.h"

namespace rover::tests::hal
{

void PwmMock::set_speed_(float speed)
{
    last_set_speed = speed;
    set_speed_call_count++;
}

void PwmMock::turn_off_()
{
    turn_off_call_count++;
}

}  // namespace rover::tests::hal