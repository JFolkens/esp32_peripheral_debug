#include "logging_mock.h"

#include <cstdarg>
#include <cstdio>

#include "main/hal/log/logging.h"

namespace rover::tests::hal
{
std::vector<Message> g_messages;

void _log(Message::Level level, const char *tag, const char *format, va_list args)
{
    char buffer[256];

    std::vsnprintf(buffer, sizeof(buffer), format, args);

    g_messages.push_back({level, tag, buffer});
}

void clear()
{
    g_messages.clear();
}

const std::vector<Message> &messages()
{
    return g_messages;
}

const Message *lastMessage()
{
    if (g_messages.empty()) {
        return nullptr;
    }

    return &g_messages.back();
}
}  // namespace rover::tests::hal
