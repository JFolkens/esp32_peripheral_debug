#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../hal/http_server/http_server_interface.h"
#include "peripherals/peripheral_interface.h"

namespace rover::web
{

class DeviceWebApp
{
   public:
    explicit DeviceWebApp(rover::hal::HttpServerInterface &server_);

    void add_peripheral(PeripheralInterface *peripheral);
    std::string render_page() const;

   private:
    rover::hal::HttpServerInterface &server;
    std::vector<PeripheralInterface *> peripherals;
};

}  // namespace rover::web
