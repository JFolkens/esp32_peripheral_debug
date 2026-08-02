#pragma once

#include <string>
#include <vector>

#include "../../hal/http_server/http_server_interface.h"

namespace rover::web
{

struct EndpointDefinition
{
    std::string uri;
    rover::hal::HttpMethod method = rover::hal::HttpMethod::GET;
    std::string action;
};

class PeripheralInterface
{
   public:
    explicit PeripheralInterface(std::string name_);

    virtual ~PeripheralInterface() = default;

    std::string id() const;

    virtual std::string render_html() const = 0;
    virtual std::vector<EndpointDefinition> endpoints() const = 0;
    virtual void handle_action(const std::string &action) = 0;

   protected:
    std::string name;
};

}  // namespace rover::web
