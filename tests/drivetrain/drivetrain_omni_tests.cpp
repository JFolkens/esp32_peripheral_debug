#include <gtest/gtest.h>

#include <memory>

#include "../../hal/motor/motor_mock.h"
#include "main/drivetrain/drivetrain_omni.h"

namespace rover::tests::drivetrain
{

class DrivetrainOmniTest : public ::testing::Test
{
   protected:
    std::unique_ptr<rover::hal::MotorInterface> fl_mock;
    std::unique_ptr<rover::hal::MotorInterface> fr_mock;
    std::unique_ptr<rover::hal::MotorInterface> br_mock;
    std::unique_ptr<rover::hal::MotorInterface> bl_mock;

    rover::hal::MotorMock *fl_ptr;
    rover::hal::MotorMock *fr_ptr;
    rover::hal::MotorMock *br_ptr;
    rover::hal::MotorMock *bl_ptr;

    std::unique_ptr<DrivetrainOmni> drivetrain;
    rover::OmniPlatform platform;

    void SetUp() override
    {
        fl_mock = std::make_unique<rover::hal::MotorMock>();
        fr_mock = std::make_unique<rover::hal::MotorMock>();
        br_mock = std::make_unique<rover::hal::MotorMock>();
        bl_mock = std::make_unique<rover::hal::MotorMock>();

        fl_ptr = fl_mock.get();
        fr_ptr = fr_mock.get();
        br_ptr = br_mock.get();
        bl_ptr = bl_mock.get();

        platform.width_m = 1.0;
        platform.length_m = 1.0;
        platform.comp_fl = 1.0;
        platform.comp_fr = 1.0;
        platform.comp_br = 1.0;
        platform.comp_bl = 1.0;

        drivetrain =
            std::make_unique<DrivetrainOmni>(std::move(fl_mock), std::move(fr_mock),
                                             std::move(br_mock), std::move(bl_mock), platform);
    }
};

TEST_F(DrivetrainOmniTest, StopInitializesMotorsToZero)
{
    // Constructor calls stop()
    EXPECT_EQ(fl_ptr->stop_call_count, 1);
    EXPECT_EQ(fr_ptr->stop_call_count, 1);
    EXPECT_EQ(br_ptr->stop_call_count, 1);
    EXPECT_EQ(bl_ptr->stop_call_count, 1);
}

TEST_F(DrivetrainOmniTest, DriveForwardSetsCorrectSpeeds)
{
    drivetrain->drive({1.0, 0.0, 0.0});

    // fl = 1*(1+0+0) = 1
    // fr = 1*(1-0-0) = 1
    // br = 1*(1+0-0) = 1
    // bl = 1*(1-0+0) = 1
    EXPECT_FLOAT_EQ(fl_ptr->last_set_speed, 1.0f);
    EXPECT_FLOAT_EQ(fr_ptr->last_set_speed, 1.0f);
    EXPECT_FLOAT_EQ(br_ptr->last_set_speed, 1.0f);
    EXPECT_FLOAT_EQ(bl_ptr->last_set_speed, 1.0f);
}

TEST_F(DrivetrainOmniTest, DriveStrafeSetsCorrectSpeeds)
{
    drivetrain->drive({0.0, 1.0, 0.0});

    // fl = 1*(0+1+0) = 1
    // fr = 1*(0-1-0) = -1
    // br = 1*(0+1-0) = 1
    // bl = 1*(0-1+0) = -1
    EXPECT_FLOAT_EQ(fl_ptr->last_set_speed, 1.0f);
    EXPECT_FLOAT_EQ(fr_ptr->last_set_speed, -1.0f);
    EXPECT_FLOAT_EQ(br_ptr->last_set_speed, 1.0f);
    EXPECT_FLOAT_EQ(bl_ptr->last_set_speed, -1.0f);
}

TEST_F(DrivetrainOmniTest, DriveRotationSetsCorrectSpeeds)
{
    drivetrain->drive({0.0, 0.0, 1.0});

    // fl = 1*(0+0+1) = 1
    // fr = 1*(0-0-1) = -1
    // br = 1*(0+0-1) = -1
    // bl = 1*(0-0+1) = 1
    EXPECT_FLOAT_EQ(fl_ptr->last_set_speed, 1.0f);
    EXPECT_FLOAT_EQ(fr_ptr->last_set_speed, -1.0f);
    EXPECT_FLOAT_EQ(br_ptr->last_set_speed, -1.0f);
    EXPECT_FLOAT_EQ(bl_ptr->last_set_speed, 1.0f);
}

TEST_F(DrivetrainOmniTest, SpeedClippingWorks)
{
    // This should result in values > 1.0 before clipping
    // fl = 1*(2+2+2) = 6
    drivetrain->drive({2.0, 2.0, 2.0});

    // Max val is 6, so all should be 6/6 = 1.0
    EXPECT_FLOAT_EQ(fl_ptr->last_set_speed, 1.0f);
    EXPECT_FLOAT_EQ(fr_ptr->last_set_speed, 1.0f);
    EXPECT_FLOAT_EQ(br_ptr->last_set_speed, 1.0f);
    EXPECT_FLOAT_EQ(bl_ptr->last_set_speed, 1.0f);
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
    EXPECT_FLOAT_EQ(speeds.x, 1.0);
    EXPECT_FLOAT_EQ(speeds.y, 0.0);
    EXPECT_FLOAT_EQ(speeds.a_cw, 0.0);
}

}  // namespace rover::tests::drivetrain
