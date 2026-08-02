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

#include "esp32/http_server_esp32.h"
#include "esp32/led_esp32.h"
#include "hal/http_server/http_server_interface.h"
#include "hal/led/led_interface.h"

/* ----- Hardware objects -----*/
rover::hal::LedInterface *green_led;
rover::hal::LedInterface *red_led;
rover::hal::HttpServerInterface *debug_server;

/* ---- LED STUFF ----*/
static bool green_led_state;
static bool red_led_state;

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

static const char *html_green_button_on = R"raw(
    <p>Green LED is currently ON</p>
    <p><a href=/green_led/off><button class=\"button\">Turn OFF</button></a></p>
)raw";

static const char *html_green_button_off = R"raw(
    <p>Green LED is currently OFF</p>
    <p><a href=/green_led/on><button class=\"button button-off\">Turn ON</button></a></p>
)raw";

static const char *html_red_button_on = R"raw(
    <p>Red LED is currently ON</p>
    <p><a href=/red_led/off><button class=\"button\">Turn OFF</button></a></p>
)raw";

static const char *html_red_button_off = R"raw(
    <p>Red LED is currently OFF</p>
    <p><a href=/red_led/on><button class=\"button button-off\">Turn ON</button></a></p>
)raw";

static const char *html_body_end = R"raw(
</body>

</html>
)raw";

std::string get_current_web_page()
{
    std::string html = std::string(html_page_header) + html_body_start;

    if (green_led_state == false) {
        html += html_green_button_off;
    } else {
        html += html_green_button_on;
    }

    if (red_led_state == false) {
        html += html_red_button_off;
    } else {
        html += html_red_button_on;
    }
    html += html_body_end;

    return html;
}

/* HTTP GET Handler */
static rover::hal::Response root_get_handler(const rover::hal::Request &req)
{
    green_led->set(green_led_state);
    red_led->set(red_led_state);

    rover::hal::Response r = {};
    r.body = get_current_web_page();
    return r;
}

static rover::hal::Response green_led_on_handler(const rover::hal::Request &req)
{
    green_led_state = true;
    return root_get_handler(req);
}

static rover::hal::Response green_led_off_handler(
    const rover::hal::Request &req)
{
    green_led_state = false;
    return root_get_handler(req);
}

static rover::hal::Response red_led_on_handler(const rover::hal::Request &req)
{
    red_led_state = true;
    return root_get_handler(req);
}

static rover::hal::Response red_led_off_handler(const rover::hal::Request &req)
{
    red_led_state = false;
    return root_get_handler(req);
}

extern "C" {
void app_main()
{
    esp_log_level_set(THREAD_TAG, ESP_LOG_DEBUG);
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_event_loop_create_default();

    debug_server =
        new rover::hal::HttpServerEsp32(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);

    debug_server->add_endpoint("/", rover::hal::HttpMethod::GET,
                               root_get_handler);
    debug_server->add_endpoint("/green_led/on", rover::hal::HttpMethod::GET,
                               green_led_on_handler);
    debug_server->add_endpoint("/green_led/off", rover::hal::HttpMethod::GET,
                               green_led_off_handler);
    debug_server->add_endpoint("/red_led/on", rover::hal::HttpMethod::GET,
                               red_led_on_handler);
    debug_server->add_endpoint("/red_led/off", rover::hal::HttpMethod::GET,
                               red_led_off_handler);

    green_led = new rover::hal::LedEsp32(GPIO_NUM_26);
    red_led = new rover::hal::LedEsp32(GPIO_NUM_27);
}
}
