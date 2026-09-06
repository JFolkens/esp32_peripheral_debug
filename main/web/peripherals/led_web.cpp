#include "led_web.h"

#include <sstream>

#include "../../hal/log/logging.h"

namespace rover::web
{

LedWeb::LedWeb(const std::string &name_, rover::hal::GpioInterface *led_)
    : PeripheralInterface(name_), led(led_)
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
    // If we are on, show a b "Turn off". If we are off, show
    // a button "Turn on".
    bool is_on = led->get();
    std::string action = is_on ? "off" : "on";
    std::string label = is_on ? "Turn OFF" : "Turn ON";
    std::string class_name = is_on ? "button" : "button button-off";

    std::stringstream ss;
    ss << "<a href=/" << name << "/" << action << "><button class=\"" << class_name << "\">"
       << label << "</button></a>";
    return ss.str();
}

std::vector<EndpointDefinition> LedWeb::endpoints() const
{
    std::vector<EndpointDefinition> defs;
    defs.push_back({"/" + name + "/on", rover::hal::HttpMethod::GET, "on"});
    defs.push_back({"/" + name + "/off", rover::hal::HttpMethod::GET, "off"});
    return defs;
}

void LedWeb::handle_action(const rover::hal::Request &action)
{
    size_t cmd_start = action.uri.rfind("/");

    std::string cmd = action.uri.substr(cmd_start + 1);
    if (cmd == "on") {
        led->set(true);
    } else if (cmd == "off") {
        led->set(false);
    } else {
        rover::hal::log_error("LedWeb", "Unknown command: %s", cmd);
    }
}

}  // namespace
   // rover::web
