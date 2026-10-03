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

#include "drivetrain/drivetrain_omni.h"
#include "esp32/gpio_esp32.h"
#include "esp32/http_server_esp32.h"
#include "esp32/motor_l298n_esp32.h"
#include "hal/gpio/gpio_interface.h"
#include "hal/motor/motor_interface.h"
#include "hal/motor/motor_l298n.h"
#include "web/device_web_app.h"
#include "web/peripherals/drivetrain_omni_web.h"
#include "web/peripherals/led_web.h"

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

    /* --- Onboard LED ---- */
    auto blue_led = std::make_unique<GpioEsp32>(rover::hal::GpioDirection::OUTPUT, GPIO_NUM_2);

    /* --- Motors ---- */
    std::array<std::unique_ptr<MotorInterface>, rover::NUM_WHEELS> motors;
    motors[rover::WheelIndex::FRONT_LEFT] = std::unique_ptr<MotorInterface>(
        MotorL298nEsp32(GPIO_NUM_21, GPIO_NUM_22, GPIO_NUM_23, LEDC_CHANNEL_0, LEDC_TIMER_0));
    motors[rover::WheelIndex::FRONT_RIGHT] = std::unique_ptr<MotorInterface>(
        MotorL298nEsp32(GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_4, LEDC_CHANNEL_1, LEDC_TIMER_1));
    motors[rover::WheelIndex::BACK_RIGHT] = std::unique_ptr<MotorInterface>(
        MotorL298nEsp32(GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_13, LEDC_CHANNEL_2, LEDC_TIMER_2));
    motors[rover::WheelIndex::BACK_LEFT] = std::unique_ptr<MotorInterface>(
        MotorL298nEsp32(GPIO_NUM_33, GPIO_NUM_25, GPIO_NUM_32, LEDC_CHANNEL_3, LEDC_TIMER_3));

    /* --- Drivetrain --- */
    auto drivetrain = std::make_unique<rover::DrivetrainOmni>(std::move(motors));

    /* --- Html rendering application ---- */
    auto debug_server = std::make_unique<HttpServerEsp32>(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    app = std::make_unique<DeviceWebApp>(std::move(debug_server));

    /* --- Add peripherals to webpage ---- */
    auto blue_led_web = std::make_unique<LedWeb>("blue_led", std::move(blue_led));
    auto drivetrain_web = std::make_unique<DrivetrainOmniWeb>("drivetrain", std::move(drivetrain));

    app->add_peripheral(std::move(blue_led_web));
    app->add_peripheral(std::move(drivetrain_web));
}

}  // extern "C"
