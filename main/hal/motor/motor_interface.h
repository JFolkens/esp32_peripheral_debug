/**
 * @file motor_interface.h
 *
 * @brief Interface for Motor controllers.
 */
#pragma once

namespace rover::hal
{

/**
 * @class MotorInterface
 * @brief Interface for controlling a motor.
 *
 * @details A motor has a speed that can be positive or
 * negative.
 */
class MotorInterface
{
   public:
    virtual ~MotorInterface() = default;

    /**
     * @brief Set motor speed, from -1 to 1.
     */
    void set_speed(float speed);

    /**
     * @brief Get current speed.
     */
    float get_speed() const;

   protected:
    virtual void set_speed_(float speed) = 0;
    float speed_;
};

}  // namespace rover::hal