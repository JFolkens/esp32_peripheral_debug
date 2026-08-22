#pragma once

#include <string.h>

#include "../hal/http_server/http_server_interface.h"

extern "C" {
#include "esp_http_server.h"
}

namespace rover::hal
{

/**
 * @brief ESP32 implementation for HttpServerInterface.
 */
class HttpServerEsp32 : public HttpServerInterface
{
   public:
    HttpServerEsp32(std::string wifi_ssid, std::string wifi_password);

   private:
    httpd_handle_t connection;

    /**
     * @brief Launch webpage once IP address is acquired.
     */
    void start_webserver();

    /**
     * @brief Handle Wi-Fi connection, disconnection, and IP events.
     *
     * @note This function is static because it must match the C interface
     * and can not use Objects. Parameter `event_data` contains pointer `this`.
     */
    static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data);
};

}  // namespace rover::hal
