#include "http_server_interface.h"

#include <cctype>

#include "../log/logging.h"

namespace
{

std::string url_decode(const std::string &value)
{
    std::string decoded;
    for (size_t index = 0; index < value.size(); ++index) {
        if (value[index] == '+') {
            decoded += ' ';
        } else if (value[index] == '%' && index + 2 < value.size() &&
                   std::isxdigit(static_cast<unsigned char>(value[index + 1])) &&
                   std::isxdigit(static_cast<unsigned char>(value[index + 2]))) {
            const auto hex_value = [](char digit) {
                if (digit >= '0' && digit <= '9')
                    return digit - '0';
                if (digit >= 'a' && digit <= 'f')
                    return digit - 'a' + 10;
                return digit - 'A' + 10;
            };
            decoded +=
                static_cast<char>(hex_value(value[index + 1]) * 16 + hex_value(value[index + 2]));
            index += 2;
        } else {
            decoded += value[index];
        }
    }
    return decoded;
}

void parse_parameters(const std::string &encoded, rover::hal::Parameters &parameters)
{
    size_t start = 0;
    while (start <= encoded.size()) {
        const size_t end = encoded.find('&', start);
        const std::string item = encoded.substr(start, end - start);
        const size_t separator = item.find('=');
        if (separator != std::string::npos) {
            parameters[url_decode(item.substr(0, separator))] =
                url_decode(item.substr(separator + 1));
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

Response HttpServerInterface::handle_request(const Request &request) const
{
    log_info("HttpServerInterface", "Handling URI: %s", request.uri.c_str());
    const size_t query_start = request.uri.find('?');
    const std::string path = request.uri.substr(0, query_start);
    Request parsed_request = request;
    if (query_start != std::string::npos) {
        parse_parameters(request.uri.substr(query_start + 1), parsed_request.parameters);
    }
    parse_parameters(request.body, parsed_request.parameters);

    auto endpoint_it = endpoints.find({path, request.method});
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