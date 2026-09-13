#include <fstream>
#include <sstream>

#include "main/web/web_assets.h"

/**
 * Non-embedded implementation of loading web asset files.
 * Computer can access file system; embedded devices use
 * embedded assets.
 */
namespace
{
// std::views::join_with is not available until C++23
std::string join_with(const std::vector<std::string> strs, std::string delimeter)
{
    if (strs.empty()) {
        return "";
    }
    std::ostringstream os;
    os << strs[0];
    for (size_t i = 1; i < strs.size(); ++i) {
        os << delimeter << strs[i];
    }
    return os.str();
}

}  // namespace

namespace rover::web
{

std::string get_text_asset(const std::vector<std::string> &path)
{
    std::string full_path = join_with(path, "/");

    std::ifstream file(full_path);

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}

}  // namespace rover::web