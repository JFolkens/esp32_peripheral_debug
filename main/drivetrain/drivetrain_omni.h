/**
 * @file drivetrain_omni.h
 *
 * @brief Drivetrain control of omni-wheel platform.
 */
#pragma once

#include <memory>

#include "../hal/motor/motor_interface.h"

namespace rover
{

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
    double comp_fl = 1.0;   // Compensation factor on [0, 1] for front left motor.
    double comp_fr = 1.0;
    double comp_br = 1.0;
    double comp_bl = 1.0;
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
    DrivetrainOmni(std::unique_ptr<rover::hal::MotorInterface> front_left,
                   std::unique_ptr<rover::hal::MotorInterface> front_right,
                   std::unique_ptr<rover::hal::MotorInterface> back_right,
                   std::unique_ptr<rover::hal::MotorInterface> back_left,
                   const OmniPlatform &platform);
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
    std::unique_ptr<rover::hal::MotorInterface> _front_left;
    std::unique_ptr<rover::hal::MotorInterface> _front_right;
    std::unique_ptr<rover::hal::MotorInterface> _back_right;
    std::unique_ptr<rover::hal::MotorInterface> _back_left;
    OmniPlatform _platform;

    double _radius;
    OmniSpeed _state;
};

}  // namespace rover