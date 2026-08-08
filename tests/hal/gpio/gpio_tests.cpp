#include <gtest/gtest.h>

#include <stdexcept>

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

TEST(GpioMockTest, InputGpioRejectsSet)
{
    GpioMock gpio(rover::hal::GpioDirection::INPUT);

    EXPECT_THROW(gpio.set(true), std::runtime_error);
    EXPECT_THROW(gpio.set(false), std::runtime_error);
}

TEST(GpioMockTest, InputGpioCanReadCurrentValue)
{
    GpioMock gpio(rover::hal::GpioDirection::INPUT);

    EXPECT_EQ(gpio.get(), false);
}

}  // namespace rover::tests::hal
