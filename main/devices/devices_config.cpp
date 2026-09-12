#include "device_registry.h"

#include "esp32/device_factories.h"

extern "C" {
#include "driver/gpio.h"
#include "driver/ledc.h"
}

/**
 * @brief Load all hardware device descriptors into the registry
 *
 * This function defines the complete hardware configuration for the rover.
 * Each device is created via a factory function that instantiates:
 *  - The underlying HAL hardware object(s) (GPIO, PWM, Motor)
 *  - The web wrapper (PeripheralInterface subclass)
 *
 * Factory functions manage all memory and return only the PeripheralInterface*.
 */
void load_devices(rover::devices::HardwareRegistry& registry)
{
    // ---- Status LEDs ----
    registry.add_device(rover::devices::DeviceDescriptor(
        "green_led", "Green LED", rover::devices::DeviceType::LED,
        []() { return rover::devices::create_led(GPIO_NUM_26, "green_led"); }));

    registry.add_device(rover::devices::DeviceDescriptor(
        "red_led", "Red LED", rover::devices::DeviceType::LED,
        []() { return rover::devices::create_led(GPIO_NUM_22, "red_led"); }));

    // ---- Back left motor (raw GPIO + PWM interface) ----
    registry.add_device(rover::devices::DeviceDescriptor(
        "m_forward", "Back Left Forward", rover::devices::DeviceType::LED,
        []() { return rover::devices::create_led(GPIO_NUM_33, "m_forward"); }));

    registry.add_device(rover::devices::DeviceDescriptor(
        "m_reverse", "Back Left Reverse", rover::devices::DeviceType::LED,
        []() { return rover::devices::create_led(GPIO_NUM_25, "m_reverse"); }));

    registry.add_device(rover::devices::DeviceDescriptor(
        "m_speed", "Back Left Speed", rover::devices::DeviceType::PWM,
        []() {
            return rover::devices::create_pwm(GPIO_NUM_32, LEDC_CHANNEL_0, LEDC_TIMER_0, "m_speed");
        }));

    // ---- Front left motor (L298N) ----
    registry.add_device(rover::devices::DeviceDescriptor(
        "front_left", "Front Left Motor", rover::devices::DeviceType::MOTOR,
        []() {
            return rover::devices::create_motor_l298n(GPIO_NUM_5,   // forward
                                                       GPIO_NUM_18,  // reverse
                                                       GPIO_NUM_19,  // speed GPIO
                                                       LEDC_CHANNEL_1, LEDC_TIMER_1, "front_left");
        }));

    // Note: Additional motors (front_right, back_right) can be added here by following
    // the same pattern with different GPIO pins and LEDC channels.
    // The commented-out configurations from web_server.cpp can be restored:
    //
    // // Front right:
    // registry.add_device(rover::devices::DeviceDescriptor(
    //     "front_right", "Front Right Motor", rover::devices::DeviceType::MOTOR,
    //     []() {
    //         return rover::devices::create_motor_l298n(GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_15,
    //                                                    LEDC_CHANNEL_0, LEDC_TIMER_0,
    //                                                    "front_right");
    //     }));
    //
    // // Back right:
    // registry.add_device(rover::devices::DeviceDescriptor(
    //     "back_right", "Back Right Motor", rover::devices::DeviceType::MOTOR,
    //     []() {
    //         return rover::devices::create_motor_l298n(GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12,
    //                                                    LEDC_CHANNEL_0, LEDC_TIMER_0,
    //                                                    "back_right");
    //     }));
}
