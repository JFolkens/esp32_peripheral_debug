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
    // bool is_on = pwm->is_on();  // TODO: Color based on is_on
    float speed = pwm->get_speed();

    std::string slider_id = name + "_slider";
    std::string display_id = name + "_display";

    // Fix: Dropping commands when user interacts fast
    // We will miss callbacks unless we disable while processing
    const int user_throttle_ms = 300;

    std::stringstream ss;
    ss << "<input type=\"range\" id=\"" << slider_id << "\" min=\"0\""
       << "max=\"100\" value=\"" << speed << "\">\n"
       << "<span id=\"" << display_id << "\">" << speed << "</span>\n"
       << "<script>\n"
       << "const slider = document.getElementById('" << slider_id << "');\n"
       << "const display = document.getElementById('" << display_id << "');\n"
       << "\n"
       << "let debounceTimer = null;\n"
       << "\n"
       << "function sendValue(val) {\n"
       << "    fetch(\n"
       << "        `/" << name << "/update?value=${encodeURIComponent(val)}`,\n"
       << "          { method: 'GET' })\n"
       << "        .catch(() => {});\n"
       << "}\n"
       << "\n"
       << "slider.addEventListener('input', (e) => {\n"
       << "    const val = e.target.value;\n"
       << "    display.textContent = val;\n"
       << "    if (debounceTimer) clearTimeout(debounceTimer);\n"
       << "    debounceTimer = setTimeout(() => {\n"
       << "        debounceTimer = null;\n"
       << "        sendValue(val);\n"
       << "    }, " << user_throttle_ms << ");\n"
       << "});\n"
       << "\n"
       << "slider.addEventListener('change', (e) => {\n"
       << "    if (debounceTimer) {\n"
       << "          clearTimeout(debounceTimer); debounceTimer = null;\n"
       << "    }\n"
       << "    sendValue(e.target.value);\n"
       << "});\n"
       << "</script>\n";

    return ss.str();
}

std::vector<EndpointDefinition> PwmWeb::endpoints() const
{
    std::vector<EndpointDefinition> defs;
    defs.push_back(
        {"/" + name + "/update", rover::hal::HttpMethod::GET, "update"});
    return defs;
}

void PwmWeb::handle_action(const rover::hal::Request &action)
{
    const std::string param = "?value=";
    size_t num_start = action.uri.find(param);

    if (num_start == std::string::npos) {
        // TODO: Throw error. Should never reach here (famous last words)
    }

    std::string subs = action.uri.substr(num_start + param.size());
    float speed = std::stof(subs);

    if (speed == 0) {
        pwm->turn_off();
    } else {
        pwm->set_speed(speed);
    }
}
}  // namespace rover::web