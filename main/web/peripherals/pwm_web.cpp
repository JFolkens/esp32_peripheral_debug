#include "pwm_web.h"

#include <sstream>

#include "../../hal/log/logging.h"

namespace rover::web
{

const uint8_t pwm_control_script[] asm("_binary_web_assets_pwm_control_js_start");

PwmWeb::PwmWeb(const std::string &name_, std::unique_ptr<rover::hal::PwmInterface> pwm_)
    : PeripheralInterface(name_), pwm(std::move(pwm_))
{
    // Empty
}

std::string PwmWeb::html_state() const
{
    // PWM could be disabled or ha a speed.
    bool is_on = pwm->is_on();
    float speed = pwm->get_speed();

    // Technically PWM could be on with a speed of zero.
    // That is too complex for webpage; setting to zero
    // is equivalent of turning off the PWM.
    is_on = is_on || (speed == 0);

    if (!is_on) {
        return name + " is set to OFF";
    } else {
        // Avoid std::to_string which pulls in the giant file
        // "floating_to_chars" We know speed <= 100 and printing with zero
        // decimal places is fine
        char buffer[4];
        std::snprintf(buffer, sizeof(buffer), "%.0f", speed);
        return name + " is set to " + buffer + "% speed";
    }
}

std::string PwmWeb::html_control() const
{
    float speed = pwm->get_speed();

    std::string slider_id = name + "_slider";
    std::string display_id = name + "_display";

    std::stringstream ss;
    ss << "<input type=\"range\" id=\"" << slider_id << "\" min=\"0\""
       << "max=\"100\" value=\"" << speed << "\">\n"
       << "<span id=\"" << display_id << "\">" << speed << "</span>\n"
       << "<script>\n"
       << "const PWM_NAME = '" << name << "';\n"
       << (const char *)pwm_control_script << "\n"
       << "</script>\n";

    return ss.str();
}

void PwmWeb::handle_update(const std::map<std::string, std::string> &parameters)
{
    const auto speed_it = parameters.find("speed");
    if (speed_it == parameters.end()) {
        rover::hal::log_info("PwmWeb", "PWM updated but no speed parameter given");
        return;
    }

    float speed = std::stof(speed_it->second);

    // 5% is low enough that user likely meant to drag slider to zero.
    // If this was an LED dimmer, it would appear "off" at 5%,
    // but continue to drain battery. So we coerce user value to 0.
    if (speed <= 5.0f) {
        pwm->turn_off();
    } else {
        pwm->set_speed(speed);
    }
}
}  // namespace  rover::web