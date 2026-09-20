#include "drivetrain_omni_web.h"

#include <cmath>
#include <sstream>

#include "../../hal/assets.h"
#include "../../hal/log/logging.h"

namespace rover::web
{

DrivetrainOmniWeb::DrivetrainOmniWeb(const std::string &name_,
                                     std::unique_ptr<rover::DrivetrainOmni> drivetrain)
    : PeripheralInterface(name_), _drivetrain(drivetrain)
{
    // Empty
}

std::string DrivetrainOmniWeb::html_state() const
{
    // Drivetrain interface is [-1, 1]. HTML displays percentage.

    // TODO
}

std::string DrivetrainOmniWeb::html_control() const
{
    OmniSpeed current_speed = _drivetrain->get_speeds();

    std::string stop_button = name + "_stop_button";
    std::string y_speed = name + "_y_speed_slider";
    std::string x_speed = name + "_x_speed_slider";
    std::string a_speed = name + "_a_speed_slider";

    std::stringstream ss;
    ss << "<button type=\"button\" id=\"" << stop_button << "\" class=\"button\""
       << "\" data-action=\"stop\">STOP</button>\n"
       << "<label><input type=\"range\" id=\"" << y_speed << "\" min=\"-100\""
       << " max=\"100\" value=\"" << current_speed.y * 100 << "\">Y</label>\n"
       << "<label><input type=\"range\" id=\"" << x_speed << "\" min=\"-100\""
       << " max=\"100\" value=\"" << current_speed.x * 100 << "\">X</label>\n"
       << "<label><input type=\"range\" id=\"" << a_speed << "\" min=\"-100\""
       << " max=\"100\" value=\"" << current_speed.a_cw * 100 << "\">Rotation</label>\n"
       << "<script>\n"
       << "(" << rover::hal::get_text_asset({"web", "assets", "drivetrain_omni_control.js"}) << ")"
       << "('" << name << "');\n"
       << "</script>\n";

    return ss.str();
}
}  // namespace rover::web