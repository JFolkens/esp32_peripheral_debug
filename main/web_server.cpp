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

#include "esp32/led_esp32.h"
#include "hal/led/led_interface.h"

/* ---- LED STUFF ----*/
static bool led_state;
static std::unique_ptr<rover::hal::LedInterface> blink_led =
    std::make_unique<rover::hal::LedEsp32>(GPIO_NUM_26);
// static LedInterface *blink_led = new LedEsp32(GPIO_NUM_26);

/*
 The WIFI name is stored in KConfig.projbuild but true password
 is hidden in my .gitignore'd "wifi_password.h".
 Create the file "wifi_password.h" and add:
 #undef CONFIG_WIFI_PASSWORD
 #define CONFIG_WIFI_PASSWORD <your_password_here>
*/
#include "wifi_password.h"

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

void display_current_web_page(httpd_req_t *req)
{
    httpd_resp_send_chunk(req, html_page_header, HTTPD_RESP_USE_STRLEN);
    httpd_resp_send_chunk(req, html_body_start, HTTPD_RESP_USE_STRLEN);

    if (led_state == false) {
        httpd_resp_send_chunk(req, html_button_off, HTTPD_RESP_USE_STRLEN);
    } else {
        httpd_resp_send_chunk(req, html_button_on, HTTPD_RESP_USE_STRLEN);
    }
    httpd_resp_send_chunk(req, html_body_end, HTTPD_RESP_USE_STRLEN);
    // End the response with a final chunk of length 0 to signal completion
    httpd_resp_send_chunk(req, NULL, 0);
}

/* HTTP GET Handler */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    update_led();
    display_current_web_page(req);

    return ESP_OK;
}

static esp_err_t led_on_handler(httpd_req_t *req)
{
    led_state = true;

    update_led();
    display_current_web_page(req);

    return ESP_OK;
}

static esp_err_t led_off_handler(httpd_req_t *req)
{
    led_state = false;

    update_led();
    display_current_web_page(req);

    return ESP_OK;
}

/* URI Structure mapping the handler to the root path "/" */
static const httpd_uri_t uri_root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
    .user_ctx = NULL,
};

static const httpd_uri_t uri_led_on = {
    .uri = "/led/on",
    .method = HTTP_GET,
    .handler = led_on_handler,
    .user_ctx = NULL,
};

static const httpd_uri_t uri_led_off = {
    .uri = "/led/off",
    .method = HTTP_GET,
    .handler = led_off_handler,
    .user_ctx = NULL,
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    ESP_LOGI(THREAD_TAG, "Starting server on port: '%d'", config.server_port);
    if (!httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(THREAD_TAG, "Error starting server!");
        return NULL;
    }
    httpd_register_uri_handler(server, &uri_root);
    httpd_register_uri_handler(server, &uri_led_on);
    httpd_register_uri_handler(server, &uri_led_off);
    return server;
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(THREAD_TAG, "Disconnected. Retrying connection...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(THREAD_TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        start_webserver(); /* Launch the server once IP is obtained */
    }
}

void wifi_init_sta()
{
    esp_event_loop_create_default();
    esp_netif_init();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                        &wifi_event_handler, NULL,
                                        &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler, NULL,
                                        &instance_got_ip);

    wifi_config_t wifi_config = {};
    std::string_view ssid_view(CONFIG_WIFI_SSID);
    std::copy_n(ssid_view.begin(),
                std::min(ssid_view.size(), sizeof(wifi_config.sta.ssid)),
                wifi_config.sta.ssid);
    std::string_view pass_view(CONFIG_WIFI_PASSWORD);
    std::copy_n(pass_view.begin(),
                std::min(pass_view.size(), sizeof(wifi_config.sta.password)),
                wifi_config.sta.password);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

extern "C" {
void app_main()
{
    esp_log_level_set(THREAD_TAG, ESP_LOG_DEBUG);
    ESP_ERROR_CHECK(nvs_flash_init());

    wifi_init_sta();
}
}