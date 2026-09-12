#pragma once

#include "../hal/pwm/pwm_interface.h"

extern "C" {
#include "driver/gpio.h"
#include "driver/ledc.h"
}

namespace rover::hal
{

class PwmEsp32 : public PwmInterface
{
   public:
    PwmEsp32(gpio_num_t pin_, ledc_channel_t ch_, ledc_timer_t timer_, uint32_t freq_hz = 5000);

    void set_speed_(float speed) override;
    void turn_off_() override;

   private:
    /*  Hardware */
    gpio_num_t pin;
    ledc_channel_t ch;
    ledc_timer_t timer;

    /* Current PWM state */
    float duty_cycle;
    uint32_t freq_hz = 5000;

    /* Remember our max speed settings so set_speed can set a percentage. */
    uint32_t max_duty;

    /* ESP32 supports high and low speed. This variable is
     * necessary as a parameter to most driver calls.
     */
    ledc_mode_t mode;
};

}  // namespace rover::hal