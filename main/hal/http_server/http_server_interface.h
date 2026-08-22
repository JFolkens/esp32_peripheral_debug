#pragma once

#include <functional>
#include <map>
#include <string>

namespace rover::hal
{

/** @brief HTTP methods supported by the server interface. */
enum class HttpMethod {
    GET,
    POST,
    PUT,
    DELETE
};

/** @brief A request passed from an HTTP transport to an endpoint. */
struct Request
{
    std::string uri;
    std::string body;
    HttpMethod method;
};

/** @brief The response returned by an endpoint. */
struct Response
{
    int status_code = 200;
    std::string content_type = "text/plain";
    std::string body;
};

using endpoint = std::function<Response(const Request &)>;

/**
 * @brief Interface for HTTP Server.
 *
 * The interface owns endpoint callbacks and coordinates their registration
 * upon connection or disconnection event.
 */
class HttpServerInterface
{
   public:
    virtual ~HttpServerInterface() = default;

    /**
     * @brief Add a URI endpoint with associated callback.
     */
    void add_endpoint(std::string uri, HttpMethod method, const endpoint &e);

    /** Dispatch a request or return a 404 response for an unknown route. */
    Response handle_request(const Request &request) const;

   protected:
    void connected();
    void disconnected();

    virtual void register_endpoint(const std::string &uri, HttpMethod method);

    std::map<std::pair<std::string, HttpMethod>, endpoint> endpoints;
    bool is_connected = false;
};

}  // namespace rover::hal
