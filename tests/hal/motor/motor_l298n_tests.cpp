#include <gtest/gtest.h>

#include "../gpio/gpio_mock.h"
#include "../pwm/pwm_mock.h"
#include "main/hal/motor/motor_l298n.h"

namespace rover::tests::hal
{

TEST(MotorL298NTest, PositiveSpeedDrivesForward)
{
    PwmMock pwm;
    GpioMock forward;
    GpioMock reverse;
    rover::hal::MotorL298N motor(pwm, forward, reverse);

    motor.set_speed(0.5f);

    EXPECT_TRUE(forward.get());
    EXPECT_FALSE(reverse.get());
    EXPECT_FLOAT_EQ(pwm.last_set_speed, 50.0f);
    EXPECT_FLOAT_EQ(motor.get_speed(), 0.5f);
    EXPECT_EQ(pwm.set_speed_call_count, 1);
}

TEST(MotorL298NTest, ZeroSpeedUsesZeroPwm)
{
    PwmMock pwm;
    GpioMock forward;
    GpioMock reverse;
    rover::hal::MotorL298N motor(pwm, forward, reverse);

    motor.set_speed(0.0f);

    EXPECT_FLOAT_EQ(pwm.last_set_speed, 0.0f);
    EXPECT_FLOAT_EQ(motor.get_speed(), 0.0f);
}

TEST(MotorL298NTest, NegativeSpeedDrivesReverseWithPositivePwm)
{
    PwmMock pwm;
    GpioMock forward;
    GpioMock reverse;
    rover::hal::MotorL298N motor(pwm, forward, reverse);

    motor.set_speed(-0.25f);

    EXPECT_FALSE(forward.get());
    EXPECT_TRUE(reverse.get());
    EXPECT_FLOAT_EQ(pwm.last_set_speed, 25.0f);
    EXPECT_FLOAT_EQ(motor.get_speed(), -0.25f);
    EXPECT_EQ(pwm.set_speed_call_count, 1);
}

TEST(MOTORL298NTest, StopSetsHardBraking)
{
    PwmMock pwm;
    GpioMock forward;
    GpioMock reverse;
    rover::hal::MotorL298N motor(pwm, forward, reverse);

    motor.stop();
    // Functionally, it does not matter if "stop" is implemented
    // as "both high" or "both low". Setting forward == backward
    // is hard braking for the L298N.
    EXPECT_EQ(forward.get(), reverse.get());
}

TEST(MotorL298NTest, RepeatedCommandsUpdateDirectionAndPwm)
{
    PwmMock pwm;
    GpioMock forward;
    GpioMock reverse;
    rover::hal::MotorL298N motor(pwm, forward, reverse);

    motor.set_speed(1.0f);
    EXPECT_TRUE(forward.get());
    EXPECT_FALSE(reverse.get());
    EXPECT_FLOAT_EQ(pwm.last_set_speed, 100.0f);

    motor.set_speed(-1.0f);
    EXPECT_FALSE(forward.get());
    EXPECT_TRUE(reverse.get());
    EXPECT_FLOAT_EQ(pwm.last_set_speed, 100.0f);

    motor.set_speed(0.25f);
    EXPECT_TRUE(forward.get());
    EXPECT_FALSE(reverse.get());
    EXPECT_FLOAT_EQ(pwm.last_set_speed, 25.0f);
    EXPECT_EQ(pwm.set_speed_call_count, 3);
    EXPECT_FLOAT_EQ(motor.get_speed(), 0.25f);
}

}  // namespace rover::tests::hal