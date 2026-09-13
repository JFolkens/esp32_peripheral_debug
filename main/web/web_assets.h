#pragma once

#include <string>
#include <vector>

namespace rover::web
{

/**
 * @brief Load text asset from file path.
 *
 */
std::string get_text_asset(const std::vector<std::string> &path);
}  // namespace rover::web