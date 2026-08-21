#include <iostream>

#include "main/hal/log/logging.h"

/**
 * This is not really a "mock". Logging is defined
 * by the embedded device. For testing on development platform,
 * we must actually implement logging on our computer.
 */
namespace rover::hal
{

void log_error(const char *tag, const char *message)
{
    std::cout << "ERROR: " << tag << " - " << message << std::endl;
}

void log_warning(const char *tag, const char *message)
{
    std::cout << "Warning: " << tag << " - " << message << std::endl;
}

void log_info(const char *tag, const char *message)
{
    std::cout << "Info: " << tag << " - " << message << std::endl;
}

}  // namespace rover::hal