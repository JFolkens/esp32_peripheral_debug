#include "http_server_esp32.h"

extern "C" {
#include "esp_log.h"
#include "esp_wifi.h"
}

#include <algorithm>

static const char *HTTP_SERVER_TAG = "HTTP_SERVER";

namespace rover::hal
{

namespace
{
/*
 * ESP32 specific wrapper around the high-level Response/Request
 * paradigm.
 *
 * Can not be a class method because the function pointer is a parameter
 * to underlying C library call.
 */
esp_err_t esp32_uri_handler(httpd_req_t *req)
{
    std::string body;
    if (req->content_len > 0) {
        std::vector<char> buf(req->content_len + 1);
        int ret = httpd_req_recv(req, buf.data(), req->content_len);
        if (ret > 0) {
            body.assign(buf.data(), ret);
        }
    }

    // Invoke the high-level callback function.
    // Pass in the request, get back a response.
    Endpoint *endpoint = static_cast<Endpoint *>(req->user_ctx);
    Request cpp_req = {};
    cpp_req.uri = req->uri;
    cpp_req.body = body;
    cpp_req.method =
        (req->method == HTTP_GET) ? HttpMethod::GET : HttpMethod::POST;

    Response cpp_resp = endpoint->handler(cpp_req);

    // Publish the returned webpage contents
    httpd_resp_send(req, cpp_resp.body.c_str(), HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

}  // namespace

HttpServerEsp32::HttpServerEsp32(std::string wifi_ssid,
                                 std::string wifi_password)
{
    esp_netif_init();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                        &wifi_event_handler, this,
                                        &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                        &wifi_event_handler, this,
                                        &instance_got_ip);

    wifi_config_t wifi_config = {};
    std::copy_n(wifi_ssid.begin(),
                std::min(wifi_ssid.size(), sizeof(wifi_config.sta.ssid)),
                wifi_config.sta.ssid);
    std::copy_n(
        wifi_password.begin(),
        std::min(wifi_password.size(), sizeof(wifi_config.sta.password)),
        wifi_config.sta.password);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

void HttpServerEsp32::add_endpoint(std::string uri, const Endpoint &e)
{
    HttpServerInterface::add_endpoint(uri, e);

    if (is_connected) {
        // This endpoint was not present when we started the web server.
        // A new webserver registers all endpoints, but this one is missing
        register_endpoint(uri);
    }
}

// ------ Private functions ------ //
void HttpServerEsp32::register_endpoint(const std::string &uri_path)
{
    auto &endpoint = endpoints.at(uri_path);
    const httpd_uri_t uri = {
        .uri = uri_path.c_str(),
        .method = (endpoint.method == HttpMethod::GET) ? HTTP_GET : HTTP_POST,
        .handler = esp32_uri_handler,
        .user_ctx = static_cast<void *>(&endpoint),
    };
    httpd_register_uri_handler(connection, &uri);
}

void HttpServerEsp32::start_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    ESP_LOGI(HTTP_SERVER_TAG, "Starting server on port: '%d'",
             config.server_port);
    if (!httpd_start(&connection, &config) == ESP_OK) {
        ESP_LOGI(HTTP_SERVER_TAG, "Error starting server!");
        return;
    }

    for (auto &[uri_path, endpoint] : endpoints) {
        register_endpoint(uri_path);
    }
}

void HttpServerEsp32::wifi_event_handler(void *arg, esp_event_base_t event_base,
                                         int32_t event_id, void *event_data)
{
    // Since wifi_event_handler is passed to the C library, it must be a static
    // function, i.e. can not access "this". Instead "arg" is a pointer to our
    // current object.
    HttpServerEsp32 *obj = static_cast<HttpServerEsp32 *>(arg);
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT &&
               event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGI(HTTP_SERVER_TAG, "Disconnected. Retrying connection...");
        obj->is_connected = false;
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(HTTP_SERVER_TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        obj->is_connected = true;
        obj->start_webserver(); /* Launch the server once IP is obtained */
    }
}

}  // namespace rover::hal
