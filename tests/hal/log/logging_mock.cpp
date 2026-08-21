#include "logging_mock.h"

#include <cstdarg>
#include <cstdio>

#include "main/hal/log/logging.h"

namespace rover::tests::hal
{
std::vector<Message> g_messages;

void log(Message::Level level, const char *tag, const char *format,
         va_list args)
{
    char buffer[256];

    std::vsnprintf(buffer, sizeof(buffer), format, args);

    g_messages.push_back({level, tag, buffer});
}

void log_error(const char *tag, const char *message, ...)
{
    va_list args;
    va_start(args, message);

    log(Message::Level::Error, tag, message, args);

    va_end(args);
}

void warning(const char *tag, const char *message, ...)
{
    va_list args;
    va_start(args, message);

    log(Message::Level::Warning, tag, message, args);

    va_end(args);
}

void info(const char *tag, const char *message, ...)
{
    va_list args;
    va_start(args, message);

    log(Message::Level::Info, tag, message, args);

    va_end(args);
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