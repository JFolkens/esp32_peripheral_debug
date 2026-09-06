#include "motor_mock.h"

namespace rover::tests::hal
{

void MotorMock::set_speed_(float speed)
{
    last_set_speed = speed;
    set_speed_call_count++;
}

void MotorMock::stop()
{
    stop_call_count++;
}

}  // namespace
   // rover::tests::hal