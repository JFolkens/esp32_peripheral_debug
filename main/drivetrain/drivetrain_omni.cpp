#include "drivetrain_omni.h"

#include "../hal/log/logging.h"

namespace rover
{

DrivetrainOmni::DrivetrainOmni(std::unique_ptr<rover::hal::MotorInterface> front_left,
                                std::unique_ptr<rover::hal::MotorInterface> front_right,
                                std::unique_ptr<rover::hal::MotorInterface> back_right,
                                std::unique_ptr<rover::hal::MotorInterface> back_left,
                                const OmniPlatform &platform)
    : _front_left(std::move(front_left)),
      _front_right(std::move(front_right)),
      _back_right(std::move(back_right)),
      _back_left(std::move(back_left)),
      _platform(platform)
{
    // Initialize hardware to speed=0
    stop();
}

void DrivetrainOmni::drive(const OmniSpeed &speeds)
{
    // Omega is angular velocity in rad/s, vx and vy are m/s and radius in m.
    // Need to adjust for
    // 1) no encoders
    // 2) clipping speeds
    // 3) different radius in x and y

    // front_left = vx + vy - (omega * radius)
    // front_right = vx - vy + (omega * radius)
    // back_right = vx + vy + (omega * radius)
    // back_left = vx - vy - (omega * radius)


}

void DrivetrainOmni::stop()
{
    drive({0,0,0});
}

}  // namespace rover