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

    /* --- Hardware objects ---- */
    auto blue_led = std::make_unique<GpioEsp32>(rover::hal::GpioDirection::OUTPUT, GPIO_NUM_2);
    auto front_left = std::unique_ptr<MotorInterface>(
        MotorL298nEsp32(GPIO_NUM_5, GPIO_NUM_18, GPIO_NUM_19, LEDC_CHANNEL_0, LEDC_TIMER_0));
    auto front_right_pwm = std::make_unique<PwmEsp32>(GPIO_NUM_15, LEDC_CHANNEL_1, LEDC_TIMER_1);

    /* --- Html webpage class ---- */
    auto debug_server = std::make_unique<HttpServerEsp32>(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    app = std::make_unique<DeviceWebApp>(std::move(debug_server));

    /* --- Wrap hardware in PeripheralInterface classes ---- */
    auto blue_led_web = std::make_unique<LedWeb>("Blue_LED", std::move(blue_led));
    auto front_left_web = std::make_unique<MotorWeb>("Front_Left_Motor", std::move(front_left));
    auto front_right_pwm_web =
        std::make_unique<PwmWeb>("Front_Right_PWM", std::move(front_right_pwm));

    /* --- Add to webpage ---- */
    app->add_peripheral(std::move(blue_led_web));
    app->add_peripheral(std::move(front_left_web));
    app->add_peripheral(std::move(front_right_pwm_web));
}

}  // extern "C"