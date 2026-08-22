#include "http_server_interface.h"

#include "../log/logging.h"

namespace rover::hal
{

void HttpServerInterface::add_endpoint(std::string uri, HttpMethod method,
                                       const endpoint &ep)
{
    const std::pair<std::string, HttpMethod> key = std::make_pair(uri, method);
    const bool is_new_endpoint = endpoints.find(key) == endpoints.end();
    endpoints[key] = ep;

    if (is_connected && is_new_endpoint) {
        register_endpoint(uri, method, ep);
    } else if (!is_new_endpoint) {
        // Doesn't print method, but atleast shows warning
        log_warning("HttpServerInterface", "URI already registered: %s", uri);
    }
}

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
        register_endpoint(route.first, route.second, endpoint);
    }
}

void HttpServerInterface::disconnected()
{
    is_connected = false;
}

}  // namespace rover::hal