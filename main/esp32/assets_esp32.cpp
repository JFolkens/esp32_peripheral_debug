#include "assets_esp32.h"

#include <string>
#include <vector>

#include "../hal/assets.h"
#include "../hal/log/logging.h"

namespace rover::hal
{

std::string get_text_asset(const std::vector<std::string> &path)
{
    /* Asset file names must be unique (not just unique paths).
     * That is due to how ESP-IDF links symbol names - see CMakeLists.txt
     * DO NOT change this function. Changing this function will not change
     * how ESP-IDF is linking symbols, and asset lookup will fail.
     */
    std::string file_name = path.back();

    size_t asset_idx = embedded_asset_count;
    for (ssize_t i = 0; i < embedded_asset_count; ++i) {
        if (file_name == embedded_assets[i].file_name) {
            if (asset_idx < i) {
                rover::hal::log_error("ASSET", "Asset file name not unique: %s", file_name);
            } else {
                asset_idx = i;
            }
        }
    }
    if (asset_idx == embedded_asset_count) {
        rover::hal::log_error("ASSET", "Asset file not found: %s", file_name);
        return {};
    }
    const char *asset_start = reinterpret_cast<const char *>(embedded_assets[asset_idx].start);
    size_t asset_length = embedded_assets[asset_idx].end - embedded_assets[asset_idx].start;

    // EMBED_TXTFILES will append terminator. We want the raw string
    // Calling functions should append terminator themselves if they want one.
    if (asset_length > 0 && asset_start[asset_length - 1] == '\0') {
        --asset_length;
    }
    return std::string(asset_start, asset_length);
}

}  // namespace rover::hal
