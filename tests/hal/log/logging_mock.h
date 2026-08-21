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
}  // namespace rover::tests::hal