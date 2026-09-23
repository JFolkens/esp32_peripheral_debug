#include "drivetrain_omni.h"

#include <algorithm>
#include <cmath>

#include "../hal/log/logging.h"

namespace rover
{

namespace
{
// Conversion factors from wheel speed to drivetrain speed
const double x_factor[4] = {1, 1, 1, 1};
const double y_factor[4] = {1, -1, 1, -1};
const double rot_factor[4] = {1, -1, -1, 1};

/** Scale array so largest value is at most `max`. */
void scale_to_max_abs(std::array<double, NUM_WHEELS> &arr, double max)
{
    double max_val = 0;
    for (double s : arr) {
        max_val = std::max(max_val, std::abs(s));
    }
    if (max_val > max) {
        for (auto &val : arr) {
            val /= (max_val / max);
        }
    }
}
}  // namespace

DrivetrainOmni::DrivetrainOmni(
    std::array<std::unique_ptr<rover::hal::MotorInterface>, NUM_WHEELS> motors,
    std::array<double, NUM_WHEELS> compensation_factors)
    : _motors(std::move(motors)), _compensation_factors(compensation_factors)
{
    // Initialize hardware to speed=0
    stop();
}

void DrivetrainOmni::drive(const OmniSpeed &speeds)
{
    // Speed input and motor output all on a [-1, 1] scale.
    // We have no encoders, so this function uses percentages, not meters/second

    rover::hal::log_info("DrivetrainOmni", "Setting speed to x=%f, y=%f, a_cw=%f", speeds.x,
                         speeds.y, speeds.a_cw);
    // Compute raw values
    std::array<double, NUM_WHEELS> wheel_speeds = {0, 0, 0, 0};
    for (std::size_t i : WHEELS) {
        wheel_speeds[i] =
            _compensation_factors[i] *
            (speeds.x * x_factor[i] + speeds.y * y_factor[i] + speeds.a_cw * rot_factor[i]);
    }

    rover::hal::log_info("DrivetrainOmni", "Computed wheel speeds as fl=%f, fr=%f, br=%f, bl=%f",
                         wheel_speeds[WheelIndex::FRONT_LEFT],
                         wheel_speeds[WheelIndex::FRONT_RIGHT],
                         wheel_speeds[WheelIndex::BACK_RIGHT], wheel_speeds[WheelIndex::BACK_LEFT]);

    // Clip if any values exceed [-1, 1] bounds
    scale_to_max_abs(wheel_speeds, 1.0);

    // Set motor speeds
    for (std::size_t i : WHEELS) {
        _motors[i]->set_speed(static_cast<float>(wheel_speeds[i]));
    }

    // Compute our current speeds.
    // By computing instead of using input OmniSpeed, we show values
    // based on clipping and scaling operations.
    _state = {0, 0, 0};
    for (int i = 0; i < NUM_WHEELS; ++i) {
        _state.x += _compensation_factors[i] * x_factor[i] * wheel_speeds[i];
        _state.y += _compensation_factors[i] * y_factor[i] * wheel_speeds[i];
        _state.a_cw += _compensation_factors[i] * rot_factor[i] * wheel_speeds[i];
    }

    // Each wheel contributes to average
    _state.x /= NUM_WHEELS;
    _state.y /= NUM_WHEELS;
    _state.a_cw /= NUM_WHEELS;
}

OmniSpeed DrivetrainOmni::get_speeds()
{
    return _state;
}

void DrivetrainOmni::stop()
{
    for (std::size_t w : WHEELS) {
        _motors[w]->stop();
    }
    _state = {0, 0, 0};
}

}  // namespace rover
