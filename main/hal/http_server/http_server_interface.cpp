#include "http_server_interface.h"

namespace rover::hal
{

/** @copydoc HttpServerInterface::add_endpoint */
void HttpServerInterface::add_endpoint(std::string uri, HttpMethod method,
                                       const endpoint &e)
{
    const auto key = std::make_pair(uri, method);
    const bool is_new_endpoint = endpoints.find(key) == endpoints.end();
    endpoints[key] = e;

    if (is_connected && is_new_endpoint) {
        register_endpoint(uri, method);
    }
}

/** @copydoc HttpServerInterface::handle_request */
Response HttpServerInterface::handle_request(const Request &request) const
{
    auto endpoint_it = endpoints.find({request.uri, request.method});
    if (endpoint_it == endpoints.end()) {
        Response response;
        response.status_code = 404;
        response.body = "Not Found";
        return response;
    }

    return endpoint_it->second(request);
}

void HttpServerInterface::connected()
{
    if (is_connected) {
        return;
    }

    is_connected = true;
    for (const auto &[route, endpoint] : endpoints) {
        register_endpoint(route.first, route.second);
    }
}

void HttpServerInterface::disconnected()
{
    is_connected = false;
}

void HttpServerInterface::register_endpoint(const std::string &, HttpMethod)
{
}

}  // namespace rover::hal