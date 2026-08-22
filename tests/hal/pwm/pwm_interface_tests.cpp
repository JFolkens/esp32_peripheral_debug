#include <gtest/gtest.h>

#include "pwm_mock.h"

namespace rover::tests::hal
{

TEST(PwmInterfaceTest, DefaultsToOffWithZeroSpeed)
{
    PwmMock pwm;

    EXPECT_FLOAT_EQ(pwm.get_speed(), 0.0f);
    EXPECT_FALSE(pwm.is_on());
}

TEST(PwmInterfaceTest, SetSpeedStoresAndDelegatesValue)
{
    PwmMock pwm;

    pwm.set_speed(50.0f);

    EXPECT_FLOAT_EQ(pwm.get_speed(), 50.0f);
    EXPECT_FLOAT_EQ(pwm.last_set_speed, 50.0f);
    EXPECT_EQ(pwm.set_speed_call_count, 1);
    EXPECT_TRUE(pwm.is_on());
}

TEST(PwmInterfaceTest, ValuesAtOrBelowThresholdTurnOff)
{
    PwmMock pwm;

    pwm.set_speed(0.01f);
    EXPECT_FALSE(pwm.is_on());
    EXPECT_FLOAT_EQ(pwm.get_speed(), 0.0f);
    EXPECT_EQ(pwm.set_speed_call_count, 0);
    EXPECT_EQ(pwm.turn_off_call_count, 1);

    pwm.set_speed(0.0f);
    EXPECT_FALSE(pwm.is_on());
    EXPECT_FLOAT_EQ(pwm.get_speed(), 0.0f);

    pwm.set_speed(-1.0f);
    EXPECT_FALSE(pwm.is_on());
    EXPECT_FLOAT_EQ(pwm.get_speed(), 0.0f);
    EXPECT_EQ(pwm.set_speed_call_count, 0);
    EXPECT_EQ(pwm.turn_off_call_count, 3);
}

TEST(PwmInterfaceTest, RepeatedSetSpeedCallsUpdateState)
{
    PwmMock pwm;

    pwm.set_speed(25.0f);
    pwm.set_speed(0.0f);

    EXPECT_FLOAT_EQ(pwm.get_speed(), 0.0f);
    EXPECT_FALSE(pwm.is_on());
    EXPECT_EQ(pwm.set_speed_call_count, 1);
    EXPECT_EQ(pwm.turn_off_call_count, 1);
}

TEST(PwmInterfaceTest, TurnOffClearsStateAndDelegates)
{
    PwmMock pwm;
    pwm.set_speed(75.0f);

    pwm.turn_off();

    EXPECT_FALSE(pwm.is_on());
    EXPECT_FLOAT_EQ(pwm.get_speed(), 0.0f);
    EXPECT_EQ(pwm.turn_off_call_count, 1);
}

}  // namespace rover::tests::hal