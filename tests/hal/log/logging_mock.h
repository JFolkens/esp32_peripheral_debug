// test/mocks/LoggerMock.hpp

#pragma once

#include <string>
#include <vector>

namespace rover::tests::hal
{
struct Message
{
    enum class Level {
        Error,
        Warning,
        Info
    };

    Level level;
    const char *tag;
    std::string text;
};

void clear();

const std::vector<Message> &messages();

const Message *lastMessage();

// Use log_error, log_warning, log_info
// This is exposed only for use in logging.cpp
void _log(Message::Level level, const char *tag, const char *format, va_list args);
}  // namespace rover::tests::hal