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

#include <string>

#include "devices/device_registry.h"
#include "esp32/http_server_esp32.h"
#include "web/device_web_app.h"

/*
 The WIFI name is stored in KConfig.projbuild but true password
 is hidden in my .gitignore'd "wifi_password.h".
 Create the file "wifi_password.h" and add:
 #undef CONFIG_WIFI_PASSWORD
 #define CONFIG_WIFI_PASSWORD <your_password_here>
*/
#include "wifi_password.h"

static const char *THREAD_TAG = "WEB_SERVER";

using namespace rover::web;

// ---- Hardware Server
static HttpServerInterface *debug_server;

// ---- Web application
static rover::web::DeviceWebApp *app;

extern "C" {
void app_main()
{
    esp_log_level_set(THREAD_TAG, ESP_LOG_DEBUG);
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_event_loop_create_default();

    // ---- Initialize HTTP server ----
    debug_server = new HttpServerEsp32(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);

    // ---- Initialize web application ----
    app = new rover::web::DeviceWebApp(*debug_server);

    // ---- Initialize all hardware devices from registry and mount to web app ----
    rover::devices::HardwareRegistry &registry = rover::devices::HardwareRegistry::instance();

    for (const auto &descriptor : registry.get_all()) {
        // Factory creates hardware + web wrapper, returns PeripheralInterface*
        rover::web::PeripheralInterface *peripheral = descriptor.factory();
        app->add_peripheral(peripheral);
    }
}

}  // extern "C"
