#include "drivetrain_omni.h"

#include <algorithm>
#include <cmath>

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
    double _radius =
        std::sqrt(std::pow(_platform.width_m / 2.0, 2) + std::pow(_platform.length_m / 2.0, 2));

    // Initialize hardware to speed=0
    stop();
}

void DrivetrainOmni::drive(const OmniSpeed &speeds)
{
    // Speed input and motor output all on a [-1, 1] scale.
    // We have no encoders, so this function uses percentages, not meters/second

    // Compute raw values
    double fl = _platform.comp_fl * (speeds.x + speeds.y + speeds.a_cw);
    double fr = _platform.comp_fr * (speeds.x - speeds.y - speeds.a_cw);
    double br = _platform.comp_br * (speeds.x + speeds.y - speeds.a_cw);
    double bl = _platform.comp_bl * (speeds.x - speeds.y + speeds.a_cw);

    // Clip if any values exceed [-1, 1] bounds
    double max_val = std::max({std::abs(fl), std::abs(fr), std::abs(br), std::abs(bl)});

    if (max_val > 1.0) {
        fl = fl / max_val;
        fr = fr / max_val;
        br = br / max_val;
        bl = bl / max_val;
    }

    // Set motor speeds
    _front_left->set_speed(fl);
    _front_right->set_speed(fr);
    _back_right->set_speed(br);
    _back_left->set_speed(bl);

    // Compute our current speeds.
    // By computing instead of using input OmniSpeed, we show values
    // based on clipping and scaling operations.

    _state.x = ((fl / _platform.comp_fl) + (fr / _platform.comp_fr) + (br / _platform.comp_br) +
                (bl / _platform.comp_bl)) /
               4;
    _state.y = (-1 * (fl / _platform.comp_fl) + (fr / _platform.comp_fr) -
                (br / _platform.comp_br) + (bl / _platform.comp_bl)) /
               4;
    _state.a_cw = (-1 * (fl / _platform.comp_fl) + (fr / _platform.comp_fr) +
                   (br / _platform.comp_br) - (bl / _platform.comp_bl)) /
                  4;
};

OmniSpeed DrivetrainOmni::get_speeds()
{
    return _state;
}

void DrivetrainOmni::stop()
{
    drive({0, 0, 0});
}

}  // namespace rover