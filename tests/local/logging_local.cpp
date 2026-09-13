#include "../hal/log/logging_mock.h"
#include "main/hal/log/logging.h"

using namespace rover::tests::hal;

/**
 * Non-embedded implementations of logging functions. Must be in rover::hal
 * because that is where function definitions live. Unit tests can use
 * std::cout; embedded devices have their own implementations.
 */
namespace rover::hal
{

void log_error(const char *tag, const char *message, ...)
{
    va_list args;
    va_start(args, message);

    _log(Message::Level::Error, tag, message, args);

    va_end(args);
}

void log_warning(const char *tag, const char *message, ...)
{
    va_list args;
    va_start(args, message);

    _log(Message::Level::Warning, tag, message, args);

    va_end(args);
}

void log_info(const char *tag, const char *message, ...)
{
    va_list args;
    va_start(args, message);

    _log(Message::Level::Info, tag, message, args);

    va_end(args);
}

}  // namespace rover::hal
