#pragma once

#include <string.h>

#include "../hal/http_server/http_server_interface.h"

extern "C" {
#include "esp_http_server.h"
}

namespace rover::hal
{

class HttpServerEsp32 : public HttpServerInterface
{
   public:
    HttpServerEsp32(std::string wifi_ssid, std::string wifi_password);

    void add_endpoint(std::string uri, Endpoint e) override;

   private:
    httpd_handle_t connection;
    bool is_connected = false;

    void start_webserver();
    void register_endpoint(std::string uri_path, Endpoint endpoint);

    static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data);
};

}  // namespace rover::hal
