#include "http_server_interface.h"

namespace rover::hal
{

void HttpServerInterface::add_endpoint(std::string uri, HttpMethod method,
                                       const endpoint &e)
{
    endpoints[{uri, method}] = e;
}

}  // namespace rover::hal