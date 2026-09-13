#pragma once

#include <cstddef>

namespace rover::hal
{

struct EmbeddedAsset
{
    const char *file_name;
    const unsigned char *start;
    const unsigned char *end;
};

}  // namespace rover::hal

/**
 * Do not edit - the rest of this file is auto-generated
 * by CMakeLists.txt.
 *
 * This means that the `esp32` folder is agnostic to new asset files being
 * added in the application layer. If that becomes too complicated,
 * `web_assets_esp32.h` can go back to being a hard-coded list of asset files.
 * See CMakeLists.txt for strategies to debug missing asset files.
 */

extern const unsigned char _binary_motor_control_js_start[];
extern const unsigned char _binary_motor_control_js_end[];
extern const unsigned char _binary_led_control_js_start[];
extern const unsigned char _binary_led_control_js_end[];
extern const unsigned char _binary_pwm_control_js_start[];
extern const unsigned char _binary_pwm_control_js_end[];
extern const unsigned char _binary_styles_css_start[];
extern const unsigned char _binary_styles_css_end[];

namespace rover::hal
{
inline constexpr EmbeddedAsset embedded_assets[] = {
    {"motor_control.js", _binary_motor_control_js_start, _binary_motor_control_js_end},
    {"led_control.js", _binary_led_control_js_start, _binary_led_control_js_end},
    {"pwm_control.js", _binary_pwm_control_js_start, _binary_pwm_control_js_end},
    {"style.css", _binary_styles_css_start, _binary_styles_css_end},
};

inline constexpr std::size_t embedded_asset_count =
    sizeof(embedded_assets) / sizeof(embedded_assets[0]);
}  // namespace rover::hal