#include "motor_web.h"

#include <cmath>
#include <sstream>

#include "../../hal/log/logging.h"

extern const uint8_t _binary_motor_control_js_start[];

namespace rover::web
{

MotorWeb::MotorWeb(const std::string &name_, std::unique_ptr<rover::hal::MotorInterface> motor_)
    : PeripheralInterface(name_), motor(std::move(motor_))
{
    // Empty
}

std::string MotorWeb::html_state() const
{
    // Motor interface is [- 1]. HTML displays percentage.
    float speed_percent = motor->get_speed() * 100;
    bool is_on = std::abs(speed_percent) > 0.001;

    if (!is_on) {
        return name + " is set to OFF";
    } else {
        // Avoid std::to_string which pulls in the giant file "floating_to_chars".
        // We know speed <= 100 and printing with zero decimal places is fine
        char buffer[4];
        std::snprintf(buffer, sizeof(buffer), "%.0f", speed_percent);
        return name + " is set to " + buffer + "% speed";
    }
}

std::string MotorWeb::html_control() const
{
    float speed = motor->get_speed();
    float speed_percent = std::abs(speed) * 100;
    bool is_off = std::abs(speed) <= 0.001;

    std::string direction_name = name + "_direction";
    std::string slider_id = name + "_slider";
    std::string display_id = name + "_display";

    std::stringstream ss;
    ss << "<label><input type=\"radio\" name=\"" << direction_name << "\" value=\"off\""
       << (is_off ? " checked" : "") << "> Off</label>\n"
       << "<label><input type=\"radio\" name=\"" << direction_name << "\" value=\"forward\""
       << (!is_off && speed > 0 ? " checked" : "") << "> Forward</label>\n"
       << "<label><input type=\"radio\" name=\"" << direction_name << "\" value=\"reverse\""
       << (!is_off && speed < 0 ? " checked" : "") << "> Reverse</label>\n"
       << "<input type=\"range\" id=\"" << slider_id << "\" min=\"0\""
       << " max=\"100\" value=\"" << speed_percent << "\"" << (is_off ? " disabled" : "") << ">\n"
       << "<span id=\"" << display_id << "\">" << speed_percent << "</span>\n"
       << "<script>\n"
       << "const MOTOR_NAME = '" << name << "';\n"
       << (const char *)&_binary_motor_control_js_start << "\n"
       << "</script>\n";

    return ss.str();
}

void MotorWeb::handle_update(const std::map<std::string, std::string> &parameters)
{
    const auto mode_it = parameters.find("mode");
    const auto speed_it = parameters.find("speed");
    if (mode_it == parameters.end() || speed_it == parameters.end())
        return;

    const std::string &mode = mode_it->second;

    if (mode == "off") {
        motor->stop();
    } else {
        float speed_percent = std::stof(speed_it->second);
        if (mode == "reverse") {
            speed_percent = -speed_percent;
        }
        motor->set_speed(speed_percent / 100.0f);
    }
}

}  // namespace  rover::web
