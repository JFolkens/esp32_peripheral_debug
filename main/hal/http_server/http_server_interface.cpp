#include "http_server_interface.h"

#include <cctype>

#include "../log/logging.h"

namespace
{

/**
 * Helper method for generating key:value pairs from web address
 */
void parse_uri_parameters(const std::string &encoded, std::map<std::string, std::string> &parameters)
{
    size_t start = 0;
    while (start <= encoded.size()) {
        const size_t end = encoded.find('&', start);
        const std::string item =
            end == std::string::npos ? encoded.substr(start) : encoded.substr(start, end - start);
        const size_t separator = item.find('=');
        if (separator != std::string::npos) {
            parameters[item.substr(0, separator)] = item.substr(separator + 1);
        }
        if (end == std::string::npos)
            break;
        start = end + 1;
    }
}

}  // namespace

namespace rover::hal
{

void HttpServerInterface::add_endpoint(std::string uri, HttpMethod method, const endpoint &ep)
{
    const std::pair<std::string, HttpMethod> key = std::make_pair(uri, method);
    const bool is_new_endpoint = endpoints.find(key) == endpoints.end();
    endpoints[key] = ep;

    if (!is_new_endpoint) {
        // Doesn't print method, but atleast shows warning
        log_warning("HttpServerInterface", "URI already registered: %s", uri);
    }
}

Response HttpServerInterface::handle_request(const std::string uri, const std::string body,
                                             HttpMethod method) const
{
    log_info("HttpServerInterface", "Handling URI: %s", uri.c_str());
    const size_t query_start = uri.find('?');
    const std::string path = uri.substr(0, query_start);
    Request parsed_request{path, method, {}};

    // Turn both URI parameters and body parameters into Request.parameters
    if (query_start != std::string::npos) {
        parse_uri_parameters(uri.substr(query_start + 1), parsed_request.parameters);
    }
    parse_uri_parameters(body, parsed_request.parameters);

    auto endpoint_it = endpoints.find({path, method});
    if (endpoint_it == endpoints.end()) {
        Response response;
        response.status_code = 404;
        response.content_type = "text/plain";
        response.body = "Not Found";
        return response;
    }

    return endpoint_it->second(parsed_request);
}

}  // namespace rover::hal