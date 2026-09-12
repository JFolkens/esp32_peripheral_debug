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
    explicit DeviceWebApp(std::unique_ptr<rover::hal::HttpServerInterface> server_);

    void add_peripheral(std::unique_ptr<PeripheralInterface> peripheral);
    std::string render_page() const;

   private:
    std::unique_ptr<rover::hal::HttpServerInterface> server;
    std::vector<std::unique_ptr<PeripheralInterface>> peripherals;
};

}  // namespace rover::web
