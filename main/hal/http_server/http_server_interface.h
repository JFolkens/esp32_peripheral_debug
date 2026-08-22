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

/** @brief Request from HTTP server. Parameter to endpoint callback. */
struct Request
{
    std::string uri;
    std::string body;
    HttpMethod method;
};

/** @brief Response to HTTP server request. Return value for endpoint callback.
 */
struct Response
{
    int status_code = 200;
    std::string content_type = "text/html";
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
    void add_endpoint(std::string uri, HttpMethod method, const endpoint &ep);

    /**
     * @brief Respond to URI endpoint request or return a 404 response
     * for an unknown endpoint.
     */
    Response handle_request(const Request &request) const;

    /**
     * @brief Convert an HTTP status code to an ESP-IDF status string.
     */
    static const char *status_text(int status_code)
    {
        switch (status_code) {
            case 200:
                return "200 OK";
            case 404:
                return "404 Not Found";
            case 405:
                return "405 Method Not Allowed";
            case 500:
                return "500 Internal Server Error";
            default:
                return "500 Internal Server Error";
        }
    }

   protected:
    void connected();
    void disconnected();

    virtual void register_endpoint(const std::string &uri,
                                   HttpMethod method) = 0;

    std::map<std::pair<std::string, HttpMethod>, endpoint> endpoints;
    bool is_connected = false;
};

}  // namespace rover::hal
