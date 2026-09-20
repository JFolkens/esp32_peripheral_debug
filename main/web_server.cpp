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

#include "esp32/http_server_esp32.h"
#include "esp32/motor_l298n_esp32.h"
#include "hal/motor/motor_interface.h"
#include "hal/motor/motor_l298n.h"
#include "web/device_web_app.h"
#include "web/peripherals/motor_web.h"

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

    /* --- Motors ---- */
    /* Problems:
     * Gpio 2 (front_right:reverse) is Blue on-board LED. Also must be low on boot
     * Gpio 5 (front_left:forward) must be high on boot
     * Gpio 15 (front_right:pwm) must be high during boot
     * Gpio 12 (back_right:pwm) must be low during boot
     *
     * Available GPIOs:
     * D13, D21, D22, D23, D26, TX2/RX2
     *
     * Solution: Move front left JST down to D23,22,21 (becomes front right)
     *           New "front left" gets D4 instead of D5
     *           Back left D12 gets D13
     */
    auto front_left = std::unique_ptr<MotorInterface>(
        MotorL298nEsp32(GPIO_NUM_5, GPIO_NUM_18, GPIO_NUM_19, LEDC_CHANNEL_0, LEDC_TIMER_0));
    auto front_right = std::unique_ptr<MotorInterface>(
        MotorL298nEsp32(GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_15, LEDC_CHANNEL_1, LEDC_TIMER_1));
    auto back_right = std::unique_ptr<MotorInterface>(
        MotorL298nEsp32(GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12, LEDC_CHANNEL_2, LEDC_TIMER_2));
    auto back_left = std::unique_ptr<MotorInterface>(
        MotorL298nEsp32(GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_32, LEDC_CHANNEL_3, LEDC_TIMER_3));

    /* --- Html rendering application ---- */
    auto debug_server = std::make_unique<HttpServerEsp32>(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    app = std::make_unique<DeviceWebApp>(std::move(debug_server));

    /* --- Add peripherals to webpage ---- */
    auto front_left_web = std::make_unique<MotorWeb>("front_left", std::move(front_left));
    auto front_right_web = std::make_unique<MotorWeb>("front_right", std::move(front_right));
    auto back_right_web = std::make_unique<MotorWeb>("back_right", std::move(back_right));
    auto back_left_web = std::make_unique<MotorWeb>("back_left", std::move(back_left));

    app->add_peripheral(std::move(front_left_web));
    app->add_peripheral(std::move(front_right_web));
    app->add_peripheral(std::move(back_right_web));
    app->add_peripheral(std::move(back_left_web));
}

}  // extern "C"
