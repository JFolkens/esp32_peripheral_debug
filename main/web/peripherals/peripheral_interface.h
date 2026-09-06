#pragma once

#include <string>

#include "../../hal/http_server/http_server_interface.h"

namespace rover::web
{

enum class StateUpdateMode {
    Command,
    Poll
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

    std::string render_state() const;

    /**
     * @brief Get a formal list of named actions (endpoints) for this.
     */
    virtual void handle_update(const rover::hal::Parameters &parameters) = 0;

    virtual StateUpdateMode state_update_mode() const
    {
        return StateUpdateMode::Command;
    }

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
