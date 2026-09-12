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

/**
 * @brief Http Server emits Requests for application handling.
 *
 * For example: http://192.168.1.10/my/path?key=5&key2=red
 *
 * path: "my/path"
 * method: (HttpMethod::GET or HttpMethod::POST)
 * parameters: {"key": "5", "key2", "red"}
 */
struct Request
{
    std::string path;       // Endpoint path
    HttpMethod method;      // GET or POST
    std::map<std::string, std::string> parameters;
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
     *
     * This function must be public instead of protected because the low-level callback
     * functions are C-based and can't handle object pointers.
     */
    Response handle_request(const std::string uri, const std::string body, HttpMethod method) const;

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
    std::map<std::pair<std::string, HttpMethod>, endpoint> endpoints;
};

}  // namespace rover::hal
