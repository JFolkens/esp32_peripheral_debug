#include <gtest/gtest.h>

#include "../gpio/gpio_mock.h"
#include "../pwm/pwm_mock.h"
#include "main/hal/motor/motor_l298n.h"

namespace rover::tests::hal
{

class MotorL298NTest : public ::testing::Test
{
   protected:
    std::unique_ptr<PwmMock> pwm;
    std::unique_ptr<GpioMock> forward;
    std::unique_ptr<GpioMock> reverse;
    std::unique_ptr<rover::hal::MotorInterface> motor;

    void SetUp() override
    {
        pwm = std::make_unique<PwmMock>();
        forward = std::make_unique<GpioMock>();
        reverse = std::make_unique<GpioMock>();
        motor = std::make_unique<rover::hal::MotorL298N>(*pwm, *forward, *reverse);
    }
};

TEST_F(MotorL298NTest, PositiveSpeedDrivesForward)
{
    motor->set_speed(0.5f);

    EXPECT_TRUE(forward->get());
    EXPECT_FALSE(reverse->get());
    EXPECT_FLOAT_EQ(pwm->last_set_speed, 50.0f);
    EXPECT_FLOAT_EQ(motor->get_speed(), 0.5f);
    EXPECT_EQ(pwm->set_speed_call_count, 1);
}

TEST_F(MotorL298NTest, ZeroSpeedUsesZeroPwm)
{
    motor->set_speed(0.0f);

    EXPECT_FLOAT_EQ(pwm->last_set_speed, 0.0f);
    EXPECT_FLOAT_EQ(motor->get_speed(), 0.0f);
}

TEST_F(MotorL298NTest, NegativeSpeedDrivesReverseWithPositivePwm)
{
    motor->set_speed(-0.25f);

    EXPECT_FALSE(forward->get());
    EXPECT_TRUE(reverse->get());
    EXPECT_FLOAT_EQ(pwm->last_set_speed, 25.0f);
    EXPECT_FLOAT_EQ(motor->get_speed(), -0.25f);
    EXPECT_EQ(pwm->set_speed_call_count, 1);
}

TEST_F(MotorL298NTest, StopSetsHardBraking)
{
    motor->stop();
    // Functionally, it does not matter if "stop" is implemented
    // as "both high" or "both low". Setting forward == backward
    // is hard braking for the L298N.
    EXPECT_EQ(forward->get(), reverse->get());
}

TEST_F(MotorL298NTest, RepeatedCommandsUpdateDirectionAndPwm)
{
    motor->set_speed(1.0f);
    EXPECT_TRUE(forward->get());
    EXPECT_FALSE(reverse->get());
    EXPECT_FLOAT_EQ(pwm->last_set_speed, 100.0f);

    motor->set_speed(-1.0f);
    EXPECT_FALSE(forward->get());
    EXPECT_TRUE(reverse->get());
    EXPECT_FLOAT_EQ(pwm->last_set_speed, 100.0f);

    motor->set_speed(0.25f);
    EXPECT_TRUE(forward->get());
    EXPECT_FALSE(reverse->get());
    EXPECT_FLOAT_EQ(pwm->last_set_speed, 25.0f);
    EXPECT_EQ(pwm->set_speed_call_count, 3);
    EXPECT_FLOAT_EQ(motor->get_speed(), 0.25f);
}

}  // namespace rover::tests::hal