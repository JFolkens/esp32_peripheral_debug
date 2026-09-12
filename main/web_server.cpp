/* Example web server for controlling peripherals

*/

extern "C" {
#include "driver/ledc.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
}

#include <memory>
#include <string>

#include "esp32/gpio_esp32.h"
#include "esp32/http_server_esp32.h"
#include "esp32/motor_l298n_esp32.h"
#include "esp32/pwm_esp32.h"
#include "hal/gpio/gpio_interface.h"
#include "hal/motor/motor_interface.h"
#include "hal/motor/motor_l298n.h"
#include "hal/pwm/pwm_interface.h"
#include "web/device_web_app.h"
#include "web/peripherals/led_web.h"
#include "web/peripherals/motor_web.h"
#include "web/peripherals/pwm_web.h"

/*
 The WIFI name is stored in KConfig.projbuild but true password
 is hidden in my .gitignore'd "wifi_password.h".
 Create the file "wifi_password.h" and add:
 #undef CONFIG_WIFI_PASSWORD
 #define CONFIG_WIFI_PASSWORD <your_password_here>
*/
#include "wifi_password.h"

static const char *THREAD_TAG = "WEB_SERVER";

using namespace rover::hal;
using namespace rover::web;

static std::unique_ptr<DeviceWebApp> app;

extern "C" {
void app_main()
{
    esp_log_level_set(THREAD_TAG, ESP_LOG_DEBUG);
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_event_loop_create_default();

    // ---- Hardware initializations ----
    auto debug_server = std::make_unique<HttpServerEsp32>(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    auto green_led = std::make_unique<GpioEsp32>(GpioDirection::OUTPUT, GPIO_NUM_26);
    auto red_led = std::make_unique<GpioEsp32>(GpioDirection::OUTPUT, GPIO_NUM_22);

    /* --- Front left ----*/
    auto motor2_forward = std::make_unique<GpioEsp32>(GpioDirection::OUTPUT, GPIO_NUM_5);
    auto motor2_reverse = std::make_unique<GpioEsp32>(GpioDirection::OUTPUT, GPIO_NUM_18);
    auto motor2_speed = std::make_unique<PwmEsp32>(GPIO_NUM_19, LEDC_CHANNEL_1, LEDC_TIMER_1);
    auto motor2 = std::make_unique<MotorL298n>(std::move(motor2_forward), std::move(motor2_reverse),
                                               std::move(motor2_speed));

    /* --- Back right ----*/
    auto motor3 = std::unique_ptr<MotorInterface>(
        MotorL298nEsp32(GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12, LEDC_CHANNEL_0, LEDC_TIMER_0));
    // motor_forward = new GpioEsp32(GpioDirection::OUTPUT, GPIO_NUM_27);
    // motor_reverse = new GpioEsp32(GpioDirection::OUTPUT, GPIO_NUM_14);
    // motor_speed = new PwmEsp32(GPIO_NUM_12, LEDC_CHANNEL_0, LEDC_TIMER_0);

    // /* --- Front right ----*/
    // motor_forward = new GpioEsp32(GpioDirection::OUTPUT, GPIO_NUM_4);
    // // GPIO 2 is also Blue on-board LED
    // motor_reverse = new GpioEsp32(GpioDirection::OUTPUT, GPIO_NUM_2);
    // motor_speed = new PwmEsp32(GPIO_NUM_15, LEDC_CHANNEL_0, LEDC_TIMER_0);

    /* --- Back left ----*/
    auto motor_forward = std::make_unique<GpioEsp32>(GpioDirection::OUTPUT, GPIO_NUM_33);
    auto motor_reverse = std::make_unique<GpioEsp32>(GpioDirection::OUTPUT, GPIO_NUM_25);
    auto motor_speed = std::make_unique<PwmEsp32>(GPIO_NUM_32, LEDC_CHANNEL_2, LEDC_TIMER_2);

    // ---- Html rendering application ----
    app = std::make_unique<DeviceWebApp>(std::move(debug_server));

    // --- Add peripherals to webpage ----
    auto green_led_web = std::make_unique<LedWeb>("green_led", std::move(green_led));
    auto red_led_web = std::make_unique<LedWeb>("red_led", std::move(red_led));

    auto motor_forward_web = std::make_unique<LedWeb>("m_forward", std::move(motor_forward));
    auto motor_reverse_web = std::make_unique<LedWeb>("m_reverse", std::move(motor_reverse));
    auto motor_speed_web = std::make_unique<PwmWeb>("m_speed", std::move(motor_speed));

    auto motor2_web = std::make_unique<MotorWeb>("front_left", std::move(motor2));
    auto motor3_web = std::make_unique<MotorWeb>("back_right", std::move(motor3));

    app->add_peripheral(std::move(green_led_web));
    app->add_peripheral(std::move(red_led_web));

    app->add_peripheral(std::move(motor_forward_web));
    app->add_peripheral(std::move(motor_reverse_web));
    app->add_peripheral(std::move(motor_speed_web));

    app->add_peripheral(std::move(motor2_web));
    app->add_peripheral(std::move(motor3_web));
}

}  // extern "C"
