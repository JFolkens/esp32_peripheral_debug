#include "pwm_web.h"

#include <sstream>

#include "../../hal/log/logging.h"

namespace rover::web
{

PwmWeb::PwmWeb(const std::string &name_, rover::hal::PwmInterface *pwm_)
    : PeripheralInterface(name_), pwm(pwm_)
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
    // bool is_on = pwm->is_on();
    // // TODO: Color based on is_on
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
       << "(() => {\n"
       << "const slider = document.getElementById('" << slider_id << "');\n"
       << "const display = document.getElementById('" << display_id << "');\n"
       << "\n"
       << "let debounceTimer = null;\n"
       << "\n"
       << "async function sendValue(val) {\n"
       << "    const response = await fetch(\n"
       << "        `/" << name << "/update?speed=${encodeURIComponent(val)}`,\n"
       << "          { method: 'POST' });\n"
       << "    if (response.ok) {\n"
       << "        document.getElementById('" << name << "_state').innerHTML =\n"
       << "            await response.text();\n"
       << "    }\n"
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
       << "})();\n"
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