#pragma once

#include <string.h>

#include "../hal/http_server/http_server_interface.h"

extern "C" {
#include "esp_http_server.h"
}

namespace rover::hal
{

/**
 * @brief ESP32 Wi-Fi and HTTP implementation for HttpServerInterface.
 */
class HttpServerEsp32 : public HttpServerInterface
{
   public:
    HttpServerEsp32(std::string wifi_ssid, std::string wifi_password);

   private:
    httpd_handle_t connection;

    /**
     * @brief Start the HTTP daemon after an IP address is acquired.
     */
    void start_webserver();

    /**
     * @brief Register one route with the ESP-IDF HTTP daemon.
     */
    void register_endpoint(const std::string &uri_path,
                           HttpMethod method) override;

    /**
     * @brief Handle Wi-Fi connection, disconnection, and IP events.
     */
    static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data);
};

}  // namespace rover::hal
