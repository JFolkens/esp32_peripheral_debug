#include "pwm_esp32.h"

#include "../hal/log/logging.h"

static const char *PWM_TAG = "PWM";

namespace rover::hal
{

PwmEsp32::PwmEsp32(gpio_num_t pin_, ledc_channel_t ch_, ledc_timer_t timer_,
                   uint32_t freq_hz)
    : pin(pin_), ch(ch_), timer(timer_)
{
    // Initialize hardware
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,  // ESP32 can handle high-speed mode
        .duty_resolution = LEDC_TIMER_13_BIT,  // Could potentially increase
        .timer_num = timer_,
        .freq_hz = freq_hz,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false,
    };
    esp_err_t err = ledc_timer_config(&ledc_timer);
    if (err != ESP_OK) {
        log_error(PWM_TAG, "Error acquiring timer for pwm");
        return;
    }

    // Calculate max duty cycle value based on resolution (2^res - 1)
    max_duty = (1 << static_cast<int>(ledc_timer.duty_resolution)) - 1;

    // Configure PWM channel
    ledc_channel_config_t ledc_channel = {
        .gpio_num = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = ch,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = timer,
        .duty = 0,
        .hpoint = 0,
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
        .flags = {},
        .deconfigure = false,
    };

    err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
        log_error(PWM_TAG, "Error configuring pwm controller for gpio pin");
        return;
    }
}

void PwmEsp32::set_speed_(float speed)
{
    log_info("PwmEsp32", "Setting speed: %f", speed);

    uint32_t duty = static_cast<uint32_t>((speed / 100.0f) * max_duty);

    esp_err_t err = ledc_set_duty(LEDC_LOW_SPEED_MODE, ch, duty);

    if (err != ESP_OK) {
        log_error(PWM_TAG, "Failed to set PWM duty cycle");
        return;
    }

    ledc_update_duty(LEDC_LOW_SPEED_MODE, ch);

    if (err != ESP_OK) {
        log_error(PWM_TAG, "Failed to set PWM duty cycle");
        return;
    }
}

void PwmEsp32::turn_off_()
{
    log_info("PwmEsp32", "Turning off");

    ledc_stop(LEDC_LOW_SPEED_MODE, ch, 0);
}

}  // namespace rover::hal