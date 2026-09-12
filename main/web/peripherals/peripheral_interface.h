#pragma once

#include <string>

#include "../../hal/http_server/http_server_interface.h"

namespace rover::web
{

enum class PeripheralType {
    Control,
    Sensor
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
    std::string html_state_and_control();

    /**
     * @brief Return an HTML rendering of state.
     */
    virtual std::string html_state() const = 0;

    /**
     * @brief Apply an update and render the resulting state.
     */
    std::string update_and_render_state(const std::map<std::string, std::string> &parameters);

    /**
     * @brief Respond to a web user interaction.
     */
    virtual void handle_update(const std::map<std::string, std::string> &parameters) = 0;

    /**
     * @brief Peripherals by default are controls. Override for sensors.
     *
     * The difference between a Control and a Sensor is refresh behavior.
     * Controls (LEDs, motors) only change state when user interacts with them.
     * Sensors update on a timer.
     */
    virtual PeripheralType state_update_mode() const
    {
        return PeripheralType::Control;
    }

   protected:
    /**
     * @brief Child classes must provide an HTML block for control options.
     */
    virtual std::string html_control() const = 0;

    std::string name;
};

}  // namespace rover::web
