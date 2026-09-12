#include <gtest/gtest.h>

#include <memory>

#include "../../hal/motor/motor_mock.h"
#include "main/web/peripherals/motor_web.h"

namespace rover::tests::hal
{

class MotorWebTest : public ::testing::Test
{
   protected:
    MotorMock *motor_ptr;
    std::unique_ptr<rover::web::MotorWeb> motor_web;

    void SetUp() override
    {
        auto motor = std::make_unique<MotorMock>();
        motor_ptr = motor.get();
        motor_web = std::make_unique<rover::web::MotorWeb>("test", std::move(motor));
    }
};

TEST_F(MotorWebTest, SetsForwardSpeedFromPercentage)
{
    motor_web->handle_update({{"mode", "forward"}, {"speed", "50"}});

    EXPECT_EQ(motor_ptr->set_speed_call_count, 1);
    EXPECT_FLOAT_EQ(motor_ptr->last_set_speed, 0.5f);
    EXPECT_EQ(motor_ptr->stop_call_count, 0);
}

TEST_F(MotorWebTest, SetsReverseSpeedFromPercentage)
{
    motor_web->handle_update({{"mode", "reverse"}, {"speed", "75"}});

    EXPECT_EQ(motor_ptr->set_speed_call_count, 1);
    EXPECT_FLOAT_EQ(motor_ptr->last_set_speed, -0.75f);
    EXPECT_EQ(motor_ptr->stop_call_count, 0);
}

TEST_F(MotorWebTest, StopsMotorWhenModeIsOff)
{
    motor_web->handle_update({{"mode", "off"}, {"speed", "0"}});

    EXPECT_EQ(motor_ptr->stop_call_count, 1);
    EXPECT_EQ(motor_ptr->set_speed_call_count, 0);
}

TEST_F(MotorWebTest, IgnoresRequestWithoutRequiredParameters)
{
    motor_web->handle_update({{"mode", "forward"}});

    EXPECT_EQ(motor_ptr->set_speed_call_count, 0);
    EXPECT_EQ(motor_ptr->stop_call_count, 0);
}

}  // namespace rover::tests::hal