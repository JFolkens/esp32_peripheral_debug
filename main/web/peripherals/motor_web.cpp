#include "motor_web.h"

#include <cmath>
#include <sstream>

#include "../../hal/log/logging.h"

namespace rover::web
{

MotorWeb::MotorWeb(const std::string &name_, rover::hal::MotorInterface *motor_)
    : PeripheralInterface(name_), motor(motor_)
{
    // Empty
}

std::string MotorWeb::html_state() const
{
    // Motor interface is [- 1]. HTML displays percentage.
    float speed_percent = motor->get_speed() * 100;
    bool is_on = std::abs(speed_percent) > 0.001;

    if (!is_on) {
        return name + "is set to OFF";
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

    // Fix: Dropping commands when user interacts fast
    // We will miss callbacks unless we di while processing
    const int user_throttle_ms = 300;

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
       << "(() => {\n"
       << "const slider = document.getElementById('" << slider_id << "');\n"
       << "const display = document.getElementById('" << display_id << "');\n"
       << "const directions = document.querySelectorAll('input[name=\\'" << direction_name
       << "\\']');\n"
       << "\n"
       << "let debounceTimer = null;\n"
       << "\n"
       << "function selectedDirection() {\n"
       << "    return document.querySelector('input[name=\\'" << direction_name
       << "\\']:checked').value;\n"
       << "}\n"
       << "\n"
       << "function sendValue(direction, value) {\n"
       << "    fetch(\n"
       << "        `/" << name
       << "/update?mode=${encodeURIComponent(direction)}&speed=${encodeURIComponent(value)}`,\n"
       << "          { method: 'GET' })\n"
       << "        .catch(() => {});\n"
       << "}\n"
       << "\n"
       << "slider.addEventListener('input', (e) => {\n"
       << "    const value = e.target.value;\n"
       << "    display.textContent = value;\n"
       << "    if (debounceTimer) clearTimeout(debounceTimer);\n"
       << "    debounceTimer = setTimeout(() => {\n"
       << "        debounceTimer = null;\n"
       << "        sendValue(selectedDirection(), value);\n"
       << "    }, " << user_throttle_ms << ");\n"
       << "});\n"
       << "\n"
       << "slider.addEventListener('change', (e) => {\n"
       << "    if (debounceTimer) {\n"
       << "        clearTimeout(debounceTimer); debounceTimer = null;\n"
       << "    }\n"
       << "    sendValue(selectedDirection(), e.target.value);\n"
       << "});\n"
       << "\n"
       << "directions.forEach((direction) => {\n"
       << "    direction.addEventListener('change', (e) => {\n"
       << "        if (debounceTimer) {\n"
       << "            clearTimeout(debounceTimer); debounceTimer = null;\n"
       << "        }\n"
       << "        const off = e.target.value === 'off';\n"
       << "        slider.disabled = off;\n"
       << "        if (off) display.textContent = '0';\n"
       << "        sendValue(e.target.value, off ? 0 : slider.value);\n"
       << "    });\n"
       << "});\n"
       << "})();\n"
       << "</script>\n";

    return ss.str();
}

std::vector<EndpointDefinition> MotorWeb::endpoints() const
{
    std::vector<EndpointDefinition> defs;
    defs.push_back({"/" + name + "/update", rover::hal::HttpMethod::GET, "update"});
    return defs;
}

void MotorWeb::handle_action(const rover::hal::Request &action)
{
    // URI should be in format:
    // /name/update?mode=<off|forward|reverse>?speed=<speed>
    const std::string mode_start_str = "?mode=";
    size_t mode_start = action.uri.find(mode_start_str);

    const std::string speed_start_str = "&speed=";
    size_t speed_start = action.uri.find(speed_start_str);

    if (mode_start == std::string::npos || speed_start == std::string::npos) {
        rover::hal::log_error("MotorWeb", "Malformed URI request: %s", action.uri.c_str());
        return;
    }

    // Beginning of mode, without "?mode="
    mode_start += mode_start_str.size();
    std::string mode = action.uri.substr(mode_start, speed_start - mode_start);
    std::string speed_str = action.uri.substr(speed_start + speed_start_str.size());

    if (mode == "off") {
        motor->stop();
    } else {
        float speed_percent = std::stof(speed_str);
        if (mode == "reverse") {
            speed_percent = -speed_percent;
        }
        motor->set_speed(speed_percent / 100.0f);
    }
}

}  // namespace  rover::web
