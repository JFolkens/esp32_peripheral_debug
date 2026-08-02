/* Example web server for controlling peripherals

*/

extern "C" {
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
#include "esp32/led_esp32.h"
#include "web/device_web_app.h"
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

// ---- Hardware
static rover::hal::LedInterface *green_led;
static rover::hal::LedInterface *red_led;
static rover::hal::HttpServerInterface *debug_server;

// ---- Web Peripheral Integrations
static rover::web::PeripheralInterface *green_led_web;
static rover::web::PeripheralInterface *red_led_web;

extern "C" {
void app_main()
{
    esp_log_level_set(THREAD_TAG, ESP_LOG_DEBUG);
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_event_loop_create_default();

    // ---- Hardware initializations ----
    debug_server =
        new rover::hal::HttpServerEsp32(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    green_led = new rover::hal::LedEsp32(GPIO_NUM_26);
    red_led = new rover::hal::LedEsp32(GPIO_NUM_27);

    // ---- Html rendering application ----
    rover::web::DeviceWebApp app(*debug_server);

    // --- Add peripherals to webpage ----
    green_led_web = new rover::web::LedWeb("green_led", green_led);
    red_led_web = new rover::web::LedWeb("red_led", red_led);

    app.add_peripheral(green_led_web);
    app.add_peripheral(red_led_web);
}
}
