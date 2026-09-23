#include <gtest/gtest.h>

#include <memory>

#include "../hal/motor/motor_mock.h"
#include "main/drivetrain/drivetrain_omni.h"

namespace rover::tests::drivetrain
{

class DrivetrainOmniTest : public ::testing::Test
{
   protected:
    std::array<rover::tests::hal::MotorMock *, rover::NUM_WHEELS> motor_ptr;

    std::unique_ptr<DrivetrainOmni> drivetrain;
    std::array<double, NUM_WHEELS> compensation_factors = {1, 1, 1, 1};

    void SetUp() override
    {
        std::array<std::unique_ptr<rover::hal::MotorInterface>, NUM_WHEELS> mock_motors;
        for (std::size_t w : rover::WHEELS) {
            auto mock_motor = std::make_unique<rover::tests::hal::MotorMock>();
            motor_ptr[w] = mock_motor.get();
            mock_motors[w] = std::move(mock_motor);
        }

        drivetrain = std::make_unique<DrivetrainOmni>(std::move(mock_motors), compensation_factors);
    }
};

TEST_F(DrivetrainOmniTest, StopInitializesMotorsToZero)
{
    // Constructor calls stop()
    for (std::size_t w : rover::WHEELS) {
        EXPECT_EQ(motor_ptr[w]->stop_call_count, 1);
    }
}

TEST_F(DrivetrainOmniTest, DriveForwardSetsCorrectSpeeds)
{
    drivetrain->drive({1.0, 0.0, 0.0});

    // fl = 1*(1+0+0) = 1
    // fr = 1*(1-0-0) = 1
    // br = 1*(1+0-0) = 1
    // bl = 1*(1-0+0) = 1
    std::array<float, NUM_WHEELS> expected = {1.0f, 1.0f, 1.0f, 1.0f};
    for (std::size_t w : rover::WHEELS) {
        EXPECT_FLOAT_EQ(motor_ptr[w]->last_set_speed, expected[w]);
    }
}

TEST_F(DrivetrainOmniTest, DriveStrafeSetsCorrectSpeeds)
{
    drivetrain->drive({0.0, 1.0, 0.0});

    // fl = 1*(0+1+0) = 1
    // fr = 1*(0-1-0) = -1
    // br = 1*(0+1-0) = 1
    // bl = 1*(0-1+0) = -1
    std::array<float, NUM_WHEELS> expected = {1.0f, -1.0f, 1.0f, -1.0f};
    for (std::size_t w : rover::WHEELS) {
        EXPECT_FLOAT_EQ(motor_ptr[w]->last_set_speed, expected[w]);
    }
}

TEST_F(DrivetrainOmniTest, DriveRotationSetsCorrectSpeeds)
{
    drivetrain->drive({0.0, 0.0, 1.0});

    // fl = 1*(0+0+1) = 1
    // fr = 1*(0-0-1) = -1
    // br = 1*(0+0-1) = -1
    // bl = 1*(0-0+1) = 1
    std::array<float, NUM_WHEELS> expected = {1.0f, -1.0f, -1.0f, 1.0f};
    for (std::size_t w : rover::WHEELS) {
        EXPECT_FLOAT_EQ(motor_ptr[w]->last_set_speed, expected[w]);
    }
}

TEST_F(DrivetrainOmniTest, SpeedClippingWorks)
{
    // This should result in values > 1.0 before clipping
    // fl = 1*(2+2+2) = 6
    drivetrain->drive({2.0, 2.0, 2.0});

    // Max val is 6, so all should be 6/6 = 1.0
    std::array<float, NUM_WHEELS> expected = {1.0f, -1 / 3.0f, 1 / 3.0f, 1 / 3.0f};
    for (std::size_t w : rover::WHEELS) {
        EXPECT_FLOAT_EQ(motor_ptr[w]->last_set_speed, expected[w]);
    }
}

TEST_F(DrivetrainOmniTest, GetSpeedsReturnsCorrectValues)
{
    // Set a known state
    drivetrain->drive({1.0, 0.0, 0.0});

    auto speeds = drivetrain->get_speeds();
    // fl=1, fr=1, br=1, bl=1
    // x = (1+1+1+1)/4 = 1
    // y = (-1+1-1+1)/4 = 0
    // a_cw = (-1+1+1-1)/4 = 0
    EXPECT_DOUBLE_EQ(speeds.x, 1.0);
    EXPECT_DOUBLE_EQ(speeds.y, 0.0);
    EXPECT_DOUBLE_EQ(speeds.a_cw, 0.0);
}

}  // namespace rover::tests::drivetrain
