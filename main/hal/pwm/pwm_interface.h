#pragma once

namespace rover::hal
{

class PwmInterface
{
   public:
    virtual ~PwmInterface() = default;

    /**
     * @brief Set PWM speed as a percentage.
     *
     * @param speed Percentage from 0 to 100 of max speed.
     */
    void set_speed(float speed);

    /**
     * @brief Turn PWM completely off.
     */
    void turn_off();

    /**
     * @brief Get current PWM speed as a pecentage.
     */
    float get_speed();

    /**
     * @brief Get current on/off state.
     */
    bool is_on();

   protected:
    float speed;
    bool is_on_;

    /* Protected methods for implementation in concrete class */
    virtual void set_speed_(float speed) = 0;
    virtual void turn_off_() = 0;
};

}  // namespace rover::hal