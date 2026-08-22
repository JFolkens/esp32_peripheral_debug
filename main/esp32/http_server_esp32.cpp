#include "http_server_esp32.h"

#include "../hal/log/logging.h"

extern "C" {
#include "esp_wifi.h"
}

#include <algorithm>

static const char *HTTP_SERVER_TAG = "HTTP_SERVER";

namespace rover::hal
{

namespace
{

constexpr httpd_method_t to_httpd_method(HttpMethod method)
{
    switch (method) {
        case HttpMethod::GET:
            return HTTP_GET;
        case HttpMethod::POST:
            return HTTP_POST;
        case HttpMethod::PUT:
            return HTTP_PUT;
        case HttpMethod::DELETE:
            return HTTP_DELETE;
    }
    return HTTP_GET;
}

constexpr HttpMethod from_httpd_method(httpd_method_t method)
{
    switch (method) {
        case HTTP_GET:
            return HttpMethod::GET;
        case HTTP_POST:
            return HttpMethod::POST;
        case HTTP_PUT:
            return HttpMethod::PUT;
        case HTTP_DELETE:
            return HttpMethod::DELETE;
        default:
            return HttpMethod::GET;
    }
}

/**
 * @brief Convert ESP32 callback to high-level Response/Request paradigm.
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

    Request cpp_req = {};
    cpp_req.uri = req->uri;
    cpp_req.body = body;
    cpp_req.method = from_httpd_method((httpd_method_t)req->method);

    HttpServerInterface *server =
        static_cast<HttpServerInterface *>(req->user_ctx);
    Response cpp_resp = server->handle_request(cpp_req);

    // Publish the returned webpage contents
    httpd_resp_set_status(
        req, HttpServerInterface::status_text(cpp_resp.status_code));
    httpd_resp_set_type(req, cpp_resp.content_type.c_str());
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

/** @copydoc HttpServerInterface::register_endpoint */
void HttpServerEsp32::register_endpoint(const std::string &uri_path,
                                        HttpMethod method)
{
    const httpd_uri_t uri = {
        .uri = uri_path.c_str(),
        .method = to_httpd_method(method),
        .handler = esp32_uri_handler,
        .user_ctx = static_cast<void *>(this),
    };
    httpd_register_uri_handler(connection, &uri);
}

/** @brief Start the ESP-IDF HTTP server and register stored endpoints. */
void HttpServerEsp32::start_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;
    // We have a lot of URIs. If this gets to be a problem, re-architect
    // web peripherals to use single endpoint /name?action
    // instead of /name/action. Or put all on a single endpoint /do?name_action.
    config.max_uri_handlers = 32;

    log_info(HTTP_SERVER_TAG, "Starting server on port: '%d",
             config.server_port);

    if (httpd_start(&connection, &config) != ESP_OK) {
        log_info(HTTP_SERVER_TAG, "Error starting server!");
        return;
    }

    connected();
}

/** @brief Handle Wi-Fi events and update the generic connection lifecycle. */
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
        log_info(HTTP_SERVER_TAG, "Disconnected. Retrying connection...");
        obj->disconnected();
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        log_info(HTTP_SERVER_TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        obj->start_webserver(); /* Launch the server once IP is obtained */
    }
}

}  // namespace rover::hal
