#include <gtest/gtest.h>

#include "main/hal/led/led_mock.h"

namespace rover::tests::hal
{

TEST(LedMockTest, DefaultsToLedOff)
{
    rover::hal::LedMock led;

    EXPECT_EQ(led.get(), false);
}

TEST(LedMockTest, TurnLedOn)
{
    rover::hal::LedMock led;

    led.set(true);

    EXPECT_EQ(led.get(), true);
}

TEST(LedMockTest, TurnLedOff)
{
    rover::hal::LedMock led;

    led.set(false);

    EXPECT_EQ(led.get(), false);
}
}  // namespace rover::tests::hal