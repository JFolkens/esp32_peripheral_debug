#include "peripheral_interface.h"

#include <sstream>

namespace rover::web
{

PeripheralInterface::PeripheralInterface(std::string name_) : name(std::move(name_))
{
    // Empty
}

std::string PeripheralInterface::id() const
{
    return name;
}

std::string PeripheralInterface::update_and_render_state(
    const std::map<std::string, std::string> &parameters)
{
    handle_update(parameters);
    return html_state();
}

std::string PeripheralInterface::html_state_and_control()
{
    // Every peripheral gets a box with their name at the top.
    // Then child class must add any displays (state) or actions (control).
    std::stringstream ss;
    ss << "    <div class=\"card\" id=\"" << name << "_card\">\n"
       << "        <h2>" << name << "</h2>\n"
       << "    <p id=\"" << name << "_state\">" << html_state() << "</p>\n"
       << "    <p id=\"" << name << "_control\">" << html_control() << "</p>\n"
       << "    </div>\n";
    return ss.str();
}

}  // namespace rover::web
