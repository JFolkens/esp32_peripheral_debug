#include <gtest/gtest.h>

#include "../gpio/gpio_mock.h"
#include "../pwm/pwm_mock.h"
#include "main/hal/motor/motor_l298n.h"

namespace rover::tests::hal
{

class MotorL298nTest : public ::testing::Test
{
   protected:
    PwmMock *pwm_ptr;
    GpioMock *forward_ptr;
    GpioMock *reverse_ptr;
    std::unique_ptr<rover::hal::MotorInterface> motor;

    void SetUp() override
    {
        // Create mocks and save raw pointers for inspection
        auto pwm = std::make_unique<PwmMock>();
        auto forward = std::make_unique<GpioMock>();
        auto reverse = std::make_unique<GpioMock>();

        pwm_ptr = pwm.get();
        forward_ptr = forward.get();
        reverse_ptr = reverse.get();

        // Transfer ownership to motor
        motor = std::make_unique<rover::hal::MotorL298n>(std::move(forward), std::move(reverse),
                                                         std::move(pwm));
    }
};

TEST_F(MotorL298nTest, PositiveSpeedDrivesForward)
{
    motor->set_speed(0.5f);

    EXPECT_TRUE(forward_ptr->get());
    EXPECT_FALSE(reverse_ptr->get());
    EXPECT_FLOAT_EQ(pwm_ptr->last_set_speed, 50.0f);
    EXPECT_FLOAT_EQ(motor->get_speed(), 0.5f);
    EXPECT_EQ(pwm_ptr->set_speed_call_count, 1);
}

TEST_F(MotorL298nTest, ZeroSpeedUsesZeroPwm)
{
    motor->set_speed(0.0f);

    EXPECT_FLOAT_EQ(pwm_ptr->last_set_speed, 0.0f);
    EXPECT_FLOAT_EQ(motor->get_speed(), 0.0f);
}

TEST_F(MotorL298nTest, NegativeSpeedDrivesReverseWithPositivePwm)
{
    motor->set_speed(-0.25f);

    EXPECT_FALSE(forward_ptr->get());
    EXPECT_TRUE(reverse_ptr->get());
    EXPECT_FLOAT_EQ(pwm_ptr->last_set_speed, 25.0f);
    EXPECT_FLOAT_EQ(motor->get_speed(), -0.25f);
    EXPECT_EQ(pwm_ptr->set_speed_call_count, 1);
}

TEST_F(MotorL298nTest, StopSetsHardBraking)
{
    motor->stop();
    // Functionally, it does not matter if "stop" is implemented
    // as "both high" or "both low". Setting forward == backward
    // is hard braking for the L298N.
    EXPECT_EQ(forward_ptr->get(), reverse_ptr->get());
}

TEST_F(MotorL298nTest, RepeatedCommandsUpdateDirectionAndPwm)
{
    motor->set_speed(1.0f);
    EXPECT_TRUE(forward_ptr->get());
    EXPECT_FALSE(reverse_ptr->get());
    EXPECT_FLOAT_EQ(pwm_ptr->last_set_speed, 100.0f);

    motor->set_speed(-1.0f);
    EXPECT_FALSE(forward_ptr->get());
    EXPECT_TRUE(reverse_ptr->get());
    EXPECT_FLOAT_EQ(pwm_ptr->last_set_speed, 100.0f);

    motor->set_speed(0.25f);
    EXPECT_TRUE(forward_ptr->get());
    EXPECT_FALSE(reverse_ptr->get());
    EXPECT_FLOAT_EQ(pwm_ptr->last_set_speed, 25.0f);
    EXPECT_EQ(pwm_ptr->set_speed_call_count, 3);
    EXPECT_FLOAT_EQ(motor->get_speed(), 0.25f);
}

}  // namespace rover::tests::hal