#include "peripheral_interface.h"

#include <sstream>

namespace rover::web
{

PeripheralInterface::PeripheralInterface(std::string name_)
    : name(std::move(name_))
{
    // Empty
}

std::string PeripheralInterface::id() const
{
    return name;
}

std::string PeripheralInterface::render_html()
{
    // Every peripheral gets a box with their name at the top.
    // Then child class must add any displays (state) or actions (control).
    std::stringstream ss;
    ss << "    <div class=\"card\">\n"
       << "        <h2>" << name << "</h2>\n"
       << "    <p>" << html_state() << "</p>\n"
       << "    <p>" << html_control() << "</p>\n"
       << "    </div>\n";
    return ss.str();
}

}  // namespace rover::web
