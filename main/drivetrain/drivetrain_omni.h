/**
 * @file drivetrain_omni.h
 *
 * @brief Drivetrain control of omni-wheel platform.
 */
#pragma once

#include <array>
#include <cstddef>
#include <memory>

#include "../hal/motor/motor_interface.h"

namespace rover
{

/** @brief Index of wheel position for Drivetrain commands and state. */
enum WheelIndex : std::size_t {
    FRONT_LEFT = 0,
    FRONT_RIGHT = 1,
    BACK_RIGHT = 2,
    BACK_LEFT = 3,
};

constexpr std::size_t NUM_WHEELS = 4;
const std::array<std::size_t, NUM_WHEELS> WHEELS = {0, 1, 2, 3};

/**
 * @brief OmniSpeed represents movement of omni-directional drivetrain.
 *
 * OmniSpeed is for a drivetrain with omni-direction motion but no encoders.
 * Speeds are relative to max power, not units of velocity.
 */
struct OmniSpeed
{
    double x = 0.0;     // Forward/backward speed on interval [-1, 1]
    double y = 0.0;     // Strafe left/right speed on interval [-1, 1]
    double a_cw = 0.0;  // Angular/rotational speed on interval [-1, 1]. Positive is clockwise.
};

/**
 * @brief Platform constants for Drivetrain Omni.
 */
struct OmniPlatform
{
    double width_m = 0.0;   // Distance between left and right wheel centers (meters).
    double length_m = 0.0;  // Distance between front and back wheels (meters).

    // Compensation factor on [0, 1] for each motor.
    std::array<double, NUM_WHEELS> compensation_factors = {1, 1, 1, 1};
};

/**
 * @class DrivetrainOmni
 * @brief Drivetrain control of platform with omni-wheels.
 *
 * DrivetrainOmni is a platform with omni-directional motion but no encoders.
 */
class DrivetrainOmni
{
   public:
    DrivetrainOmni(std::array<std::unique_ptr<rover::hal::MotorInterface>, NUM_WHEELS> motors,
                   std::array<double, NUM_WHEELS> compensation_factors = {1, 1, 1, 1});
    virtual ~DrivetrainOmni() = default;

    /**
     * @brief Set drivetrain speeds.
     * @param speeds Target linear and angular speeds.
     */
    void drive(const OmniSpeed &speeds);

    /**
     * @brief Get drivetrain speeds.
     *
     * Get current (expected) drivetrain speeds. This is based on
     * the latest update to motors. There are no encoders, so this
     * is a rough estimate using OmniPlatform wheel scaling constants.
     */
    OmniSpeed get_speeds();

    /**
     * @brief Stop all motors immediately.
     */
    void stop();

   private:
    std::array<std::unique_ptr<rover::hal::MotorInterface>, NUM_WHEELS> _motors;
    std::array<double, NUM_WHEELS> _compensation_factors;

    OmniSpeed _state;
};

}  // namespace rover
