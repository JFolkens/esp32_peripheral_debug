/* Example web server for controlling peripherals

*/

extern "C" {
#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
}

#include <algorithm>
#include <cstring>
#include <memory>
#include <string_view>

#include "esp32/http_server_esp32.h"
#include "esp32/led_esp32.h"
#include "hal/http_server/http_server_interface.h"
#include "hal/led/led_interface.h"

/* ----- Hardware objects -----*/
rover::hal::LedInterface *blink_led;
rover::hal::HttpServerInterface *debug_server;

/* ---- LED STUFF ----*/
static bool led_state;

/*
 The WIFI name is stored in KConfig.projbuild but true password
 is hidden in my .gitignore'd "wifi_password.h".
 Create the file "wifi_password.h" and add:
 #undef CONFIG_WIFI_PASSWORD
 #define CONFIG_WIFI_PASSWORD <your_password_here>
*/
#include "wifi_password.h"

/* ---- WIFI STUFF ----- */
static const char *THREAD_TAG = "WEB_SERVER";

static const char *html_page_header = R"raw(
<!DOCTYPE html><html>
<head>
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
    <style>
        html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}
        .button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;
        text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}
        .button-off {background-color: #555555;}
    </style>
</head>
)raw";

static const char *html_body_start = R"raw(
<body>
    <h1>ESP32 Web Server</h1>
)raw";

static const char *html_button_on = R"raw(
    <p>LED is currently ON</p>
    <p><a href=/led/off><button class=\"button\">Turn OFF</button></a></p>
)raw";

static const char *html_button_off = R"raw(
    <p>LED is currently OFF</p>
    <p><a href=/led/on><button class=\"button button-off\">Turn ON</button></a></p>
)raw";

static const char *html_body_end = R"raw(
</body>

</html>
)raw";

void update_led()
{
    blink_led->set(led_state);
}

std::string get_current_web_page()
{
    std::string html = std::string(html_page_header) + html_body_start;

    if (led_state == false) {
        html += html_button_off;
    } else {
        html += html_button_on;
    }
    html += html_body_end;

    return html;
}

/* HTTP GET Handler */
static rover::hal::Response root_get_handler(const rover::hal::Request &req)
{
    update_led();

    rover::hal::Response r = {};
    r.body = get_current_web_page();
    return r;
}

static rover::hal::Response led_on_handler(const rover::hal::Request &req)
{
    led_state = true;
    return root_get_handler(req);
}

static rover::hal::Response led_off_handler(const rover::hal::Request &req)
{
    led_state = false;
    return root_get_handler(req);
}

rover::hal::Endpoint root = {
    .handler = root_get_handler,
    .method = rover::hal::HttpMethod::GET,
};

rover::hal::Endpoint led_on = {
    .handler = led_on_handler,
    .method = rover::hal::HttpMethod::GET,
};

rover::hal::Endpoint led_off = {
    .handler = led_off_handler,
    .method = rover::hal::HttpMethod::GET,
};

extern "C" {
void app_main()
{
    esp_log_level_set(THREAD_TAG, ESP_LOG_DEBUG);
    ESP_ERROR_CHECK(nvs_flash_init());

    esp_event_loop_create_default();

    std::string ssid = CONFIG_WIFI_SSID;
    std::string password = CONFIG_WIFI_PASSWORD;
    debug_server = new rover::hal::HttpServerEsp32(ssid, password);

    debug_server->add_endpoint("/", root);

    debug_server->add_endpoint("/led/on", led_on);

    debug_server->add_endpoint("/led/off", led_off);

    blink_led = new rover::hal::LedEsp32(GPIO_NUM_26);
}
}