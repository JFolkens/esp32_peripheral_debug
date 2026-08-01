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

typedef struct Endpoint_t
{
    std::function<Response(const Request &)> handler;
    HttpMethod method;
} Endpoint;

class HttpServerInterface
{
   public:
    virtual ~HttpServerInterface() = default;

    virtual void add_endpoint(std::string uri, Endpoint e);

   protected:
    std::map<std::string, Endpoint> endpoints;
};

}  // namespace rover::hal
