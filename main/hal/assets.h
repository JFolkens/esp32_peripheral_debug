#pragma once

#include <string>
#include <vector>

namespace rover::hal
{

/** @brief Load text asset from file path. */
std::string get_text_asset(const std::vector<std::string> &path);

}  // namespace rover::hal