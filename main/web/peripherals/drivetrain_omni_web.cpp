#include "drivetrain_omni_web.h"

#include <cmath>
#include <sstream>

#include "../../hal/assets.h"
#include "../../hal/log/logging.h"

namespace rover::web
{

DrivetrainOmniWeb::DrivetrainOmniWeb(const std::string &name_,
                                     std::unique_ptr<rover::DrivetrainOmni> drivetrain)
    : PeripheralInterface(name_), _drivetrain(std::move(drivetrain))
{
    // Empty
}

std::string DrivetrainOmniWeb::html_state() const
{
    // Drivetrain interface is [-1, 1]. HTML displays percentage.

    // TODO
    return "";
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
       << ">STOP</button>\n"
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

void DrivetrainOmniWeb::handle_update(const std::map<std::string, std::string> &parameters)
{
    // Command can be either 1) STOP or 2) Change Speed
    const auto stop_it = parameters.find("stop");
    if (stop_it != parameters.end()) {
        _drivetrain->stop();
        return;
    }

    const auto x_it = parameters.find("x");
    const auto y_it = parameters.find("y");
    const auto a_it = parameters.find("a");

    if (x_it == parameters.end() || y_it == parameters.end() || a_it == parameters.end()) {
        rover::hal::log_error("DrivetrainOmniWeb", "Omni speed requires x/y/a components");
        return;
    }

    rover::hal::log_info("DrivetrainOmniWeb", "Speed update: x=%s, y=%s, a=%s",
                         x_it->second.c_str(), y_it->second.c_str(), a_it->second.c_str());
    OmniSpeed speed_update;
    speed_update.x = std::stoi(x_it->second) / 100.0;
    speed_update.y = std::stoi(y_it->second) / 100.0;
    speed_update.a_cw = std::stoi(a_it->second) / 100.0;

    _drivetrain->drive(speed_update);
}
}  // namespace rover::web