#pragma once

#include <string>
#include <vector>

#include "../../hal/http_server/http_server_interface.h"

namespace rover::web
{

struct EndpointDefinition
{
    std::string uri;
    rover::hal::HttpMethod method = rover::hal::HttpMethod::GET;
    std::string action;
};

class PeripheralInterface
{
   public:
    explicit PeripheralInterface(std::string name_);
    virtual ~PeripheralInterface() = default;

    /**
     * @brief Get the id (name) of this.
     */
    std::string id() const;

    /**
     * @brief Return a HTML rendering of state and available controls.
     */
    std::string render_html();

    /**
     * @brief Get a formal list of named actions (endpoints) for this.
     */
    virtual std::vector<EndpointDefinition> endpoints() const = 0;

    /**
     * @brief Respond to a user action.
     */
    virtual void handle_action(const std::string &action) = 0;

   protected:
    /**
     * @brief Child classes must provide an HTML block representing their state.
     */
    virtual std::string html_state() const = 0;

    /**
     * @brief Child classes must provide an HTML block for control options.
     */
    virtual std::string html_control() const = 0;

    std::string name;
};

}  // namespace rover::web
