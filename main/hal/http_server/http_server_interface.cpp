#include "http_server_interface.h"

namespace rover::hal
{

void HttpServerInterface::add_endpoint(std::string uri, HttpMethod method,
                                       const endpoint &e)
{
    endpoints[{uri, method}] = e;
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

}  // namespace rover::hal