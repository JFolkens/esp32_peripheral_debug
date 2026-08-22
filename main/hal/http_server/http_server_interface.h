#pragma once

#include <functional>
#include <map>
#include <string>

namespace rover::hal
{

// ---- Helper data types for interacting with HttpServer
enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE
};

struct Request
{
    std::string uri;
    std::string body;
    HttpMethod method;
};

struct Response
{
    int status_code = 200;
    std::string content_type = "text/plain";
    std::string body;
};

using endpoint = std::function<Response(const Request &)>;

class HttpServerInterface
{
   public:
    virtual ~HttpServerInterface() = default;

    virtual void add_endpoint(std::string uri, HttpMethod method,
                              const endpoint &e);

    Response handle_request(const Request &request) const;

   protected:
    // The purpose of storing endpoints is so that connection can
    // be re-created while server maintains functionality.
    std::map<std::pair<std::string, HttpMethod>, endpoint> endpoints;
};

}  // namespace rover::hal
