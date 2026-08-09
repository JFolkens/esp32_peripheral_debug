#include "pwm_web.h"

#include <sstream>

namespace rover::web
{

PwmWeb::PwmWeb(const std::string &name_, rover::hal::PwmInterface *pwm_)
    : PeripheralInterface(name_), pwm(pwm_)
{
    // Empty
}

std::string PwmWeb::html_state() const
{
    // PWM could be disabled or have a speed.
    bool is_on = pwm->is_on();
    float speed = pwm->get_speed();

    // Technically PWM could be on with a speed of zero.
    // That is too complex for webpage; setting to zero is
    // equivalent of turning off the PWM.
    is_on = is_on || (speed == 0);

    if (!is_on) {
        return name + " is set to OFF";
    } else {
        return name + " is set to " + std::to_string(speed) + "% speed";
    }
}

std::string PwmWeb::html_control() const
{
    // bool is_on = pwm->is_on();  // TODO: Color based on is_on
    float speed = pwm->get_speed();

    std::string slider_id = name + "_slider";
    std::string display_id = name + "_display";

    std::stringstream ss;
    ss << "<input type=\"range\" id=\"" << slider_id << "\" min=\"0\""
       << "max=\"100\" value=\"" << speed << "\">\n"
       << "<span id=\"" << display_id << "\">" << speed << "</span>\n"
       << "<script>\n"
       << "const slider = document.getElementById('" << slider_id << "');\n"
       << "const display = document.getElementById('" << display_id << "')\n"
       << "slider.addEventListener('input', (e) => {\n"
       << "    const val = e.target.value;\n"
       << "    display.textContent = val;\n"
       << "    fetch(`/" << name << "/update?value=${val}`, {method: 'GET'})"
       << "});"
       << "</script>";

    return ss.str();
}

std::vector<EndpointDefinition> PwmWeb::endpoints() const
{
    std::vector<EndpointDefinition> defs;
    defs.push_back(
        {"/" + name + "/update", rover::hal::HttpMethod::GET, "update"});
    return defs;
}

void PwmWeb::handle_action(const std::string &action)
{
    size_t num_start = action.find("?");

    if (num_start == std::string::npos) {
        // TODO: Throw error. Should never reach here (famous last words)
    }

    float speed = std::stof(action.substr(num_start + 1));

    if (speed == 0) {
        pwm->turn_off();
    } else {
        pwm->set_speed(speed);
    }
}
}  // namespace rover::web