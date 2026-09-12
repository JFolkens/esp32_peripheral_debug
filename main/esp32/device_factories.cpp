#include "device_factories.h"

#include "../hal/gpio/gpio_interface.h"
#include "../hal/motor/motor_interface.h"
#include "../hal/motor/motor_l298n.h"
#include "../hal/pwm/pwm_interface.h"
#include "../web/peripherals/led_web.h"
#include "../web/peripherals/motor_web.h"
#include "../web/peripherals/pwm_web.h"
#include "gpio_esp32.h"
#include "pwm_esp32.h"

namespace rover::devices
{

rover::web::PeripheralInterface* create_led(gpio_num_t pin, const std::string& id)
{
    auto gpio = new rover::hal::GpioEsp32(rover::hal::GpioDirection::OUTPUT, pin);
    return new rover::web::LedWeb(id, gpio);
}

rover::web::PeripheralInterface* create_pwm(gpio_num_t gpio_pin, int ledc_channel,
                                             int ledc_timer, const std::string& id)
{
    auto pwm = new rover::hal::PwmEsp32(gpio_pin, static_cast<ledc_channel_t>(ledc_channel),
                                        static_cast<ledc_timer_t>(ledc_timer));
    return new rover::web::PwmWeb(id, pwm);
}

rover::web::PeripheralInterface* create_motor_l298n(gpio_num_t forward_pin, gpio_num_t reverse_pin,
                                                      gpio_num_t speed_gpio_pin,
                                                      int speed_ledc_channel,
                                                      int speed_ledc_timer, const std::string& id)
{
    // Create GPIO pins for direction control
    auto forward = new rover::hal::GpioEsp32(rover::hal::GpioDirection::OUTPUT, forward_pin);
    auto reverse = new rover::hal::GpioEsp32(rover::hal::GpioDirection::OUTPUT, reverse_pin);

    // Create PWM for speed control
    auto speed =
        new rover::hal::PwmEsp32(speed_gpio_pin, static_cast<ledc_channel_t>(speed_ledc_channel),
                                 static_cast<ledc_timer_t>(speed_ledc_timer));

    // Create motor controller
    auto motor = new rover::hal::MotorL298N(*speed, *forward, *reverse);

    // Wrap in web interface
    return new rover::web::MotorWeb(id, motor);
}

}  // namespace rover::devices
