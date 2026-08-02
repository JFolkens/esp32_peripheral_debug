#include "led_web.h"

#include <sstream>

namespace rover::web
{

namespace
{

std::string html_button_for_state(const std::string &id, bool is_on)
{
    std::string action = is_on ? "off" : "on";
    std::string label = is_on ? "Turn OFF" : "Turn ON";
    std::string class_name = is_on ? "button" : "button button-off";

    std::stringstream ss;
    ss << "    <p>" << id << " is currently " << (is_on ? "ON" : "OFF")
       << "</p>\n"
       << "    <p><a href=/" << id << "/" << action << "><button class=\""
       << class_name << "\">" << label << "</button></a></p>\n";
    return ss.str();
}

}  // namespace

LedWeb::LedWeb(const std::string &name_, rover::hal::LedInterface *led_)
    : PeripheralInterface(name_), led(led_)
{
    // Empty
}

std::string LedWeb::render_html() const
{
    std::stringstream ss;
    ss << "    <div class=\"card\">\n"
       << "        <h2>" << name << "</h2>\n"
       << html_button_for_state(name, led->get()) << "\n"
       << "    </div>\n";
    return ss.str();
}

std::vector<EndpointDefinition> LedWeb::endpoints() const
{
    std::vector<EndpointDefinition> defs;
    defs.push_back({"/" + name + "/on", rover::hal::HttpMethod::GET, "on"});
    defs.push_back({"/" + name + "/off", rover::hal::HttpMethod::GET, "off"});
    return defs;
}

void LedWeb::handle_action(const std::string &action)
{
    if (action == "on") {
        led->set(true);
    }

    if (action == "off") {
        led->set(false);
    }
}

}  // namespace rover::web
