/**
 * @file motor_l298n.h
 *
 * @brief Define the MotorL298N class.
 */
#pragma once

#include "../gpio/gpio_interface.h"
#include "../pwm/pwm_interface.h"
#include "motor_interface.h"

namespace rover::hal
{

/**
 * @class MotorL298N
 * @brief Control a motor using the L298N motor control module.
 *
 * @details The L298N has a PWM for speed, and two GPIO output
 * pins for forward and reverse.
 *
 * In reality, each physical L298N has 6 ports and can support
 * two MotorL298N objects, because it is a dual controller. The
 * motors are independent and have their own unique software object.
 */
class MotorL298N : public MotorInterface
{
   public:
    MotorL298N(PwmInterface &speed_control, GpioInterface &forward,
               GpioInterface &reverse);
    ~MotorL298N() = default;

    /**
     * @brief Stop the motor.
     *
     * `set_speed(0)` for L298N is a coasting stop. `stop()` is a "braking"
     * function.
     */
    void stop() override;

   protected:
    void set_speed_(float speed) override;

   private:
    PwmInterface &_speed_control;
    GpioInterface &_forward;
    GpioInterface &_reverse;
};

}  // namespace rover::hal