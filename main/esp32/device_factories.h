#pragma once

extern "C" {
#include "driver/gpio.h"
}

namespace rover::web
{
class PeripheralInterface;
}

namespace rover::devices
{

/**
 * @brief Create an LED device controlled via GPIO
 * @param pin The GPIO pin number
 * @param id Unique device identifier
 * @return PeripheralInterface* wrapping the LED (factory manages all memory)
 */
rover::web::PeripheralInterface* create_led(gpio_num_t pin, const std::string& id);

/**
 * @brief Create a PWM-based speed controller
 * @param gpio_pin The GPIO pin for PWM output
 * @param ledc_channel LEDC channel for this PWM
 * @param ledc_timer LEDC timer for this PWM
 * @param id Unique device identifier
 * @return PeripheralInterface* wrapping the PWM (factory manages all memory)
 */
rover::web::PeripheralInterface* create_pwm(gpio_num_t gpio_pin, int ledc_channel,
                                             int ledc_timer, const std::string& id);

/**
 * @brief Create an L298N motor controller with forward/reverse/speed
 * @param forward_pin GPIO pin for forward direction
 * @param reverse_pin GPIO pin for reverse direction
 * @param speed_gpio_pin GPIO pin for PWM speed control
 * @param speed_ledc_channel LEDC channel for speed PWM
 * @param speed_ledc_timer LEDC timer for speed PWM
 * @param id Unique device identifier
 * @return PeripheralInterface* wrapping the motor (factory manages all memory)
 */
rover::web::PeripheralInterface* create_motor_l298n(gpio_num_t forward_pin, gpio_num_t reverse_pin,
                                                      gpio_num_t speed_gpio_pin,
                                                      int speed_ledc_channel,
                                                      int speed_ledc_timer, const std::string& id);

}  // namespace rover::devices
