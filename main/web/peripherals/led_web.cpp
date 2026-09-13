#include "led_web.h"

#include <sstream>

#include "../../hal/log/logging.h"

extern const uint8_t _binary_led_control_js_start[];

namespace rover::web
{

LedWeb::LedWeb(const std::string &name_, std::unique_ptr<rover::hal::GpioInterface> led_)
    : PeripheralInterface(name_), led(std::move(led_))
{
    // Empty
}

std::string LedWeb::html_state() const
{
    // LED is either on or off.
    std::string state = led->get() ? "on" : "off";

    return name + " is currently " + state;
}

std::string LedWeb::html_control() const
{
    // If we are on, show a button "Turn off". If we are off, show
    // a button "Turn on".
    bool is_on = led->get();
    std::string action = is_on ? "off" : "on";
    std::string label = is_on ? "Turn OFF" : "Turn ON";
    std::string class_name = is_on ? "button" : "button button-off";

    std::stringstream ss;
    ss << "<button type=\"button\" id=\"" << name << "_button\" class=\"" << class_name
       << "\" data-action=\"" << action << "\">" << label << "</button>\n"
       << "<script>\n"
       << "const LED_NAME = '" << name << "';\n"
       << (const char *)&_binary_led_control_js_start << "\n"
       << "</script>";
    return ss.str();
}

void LedWeb::handle_update(const std::map<std::string, std::string> &parameters)
{
    const auto action_it = parameters.find("action");
    if (action_it == parameters.end())
        return;
    const std::string &cmd = action_it->second;
    if (cmd == "on") {
        led->set(true);
    } else if (cmd == "off") {
        led->set(false);
    } else {
        rover::hal::log_error("LedWeb", "Unknown command: %s", cmd);
    }
}

}  // namespace  rover::web
