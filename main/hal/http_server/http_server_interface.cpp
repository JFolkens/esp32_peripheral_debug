#include "http_server_interface.h"

namespace rover::hal
{

void HttpServerInterface::add_endpoint(std::string uri, Endpoint e)
{
    endpoints[uri] = e;
}

}  // namespace rover::hal