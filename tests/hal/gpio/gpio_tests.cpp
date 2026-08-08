#include <gtest/gtest.h>

#include "gpio_mock.h"

namespace rover::tests::hal
{

TEST(GpioMockTest, DefaultsToOffWhenOutput)
{
    GpioMock gpio;

    EXPECT_EQ(gpio.get(), false);
}

TEST(GpioMockTest, SetOutputHigh)
{
    GpioMock gpio;

    gpio.set(true);

    EXPECT_EQ(gpio.get(), true);
}

TEST(GpioMockTest, SetOutputLow)
{
    GpioMock gpio;

    gpio.set(false);

    EXPECT_EQ(gpio.get(), false);
}

}  // namespace rover::tests::hal
