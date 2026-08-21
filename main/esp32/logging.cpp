#include "../hal/log/logging.h"

extern "C" {
#include "esp_log.h"
}

namespace rover::hal
{

void log_error(const char *tag, const char *message, ...)
{
    va_list args;
    va_start(args, message);
    esp_log_writev(ESP_LOG_INFO, tag, message, args);
}

void log_warning(const char *tag, const char *message, ...)
{
    va_list args;
    va_start(args, message);
    esp_log_writev(ESP_LOG_WARN, tag, message, args);
}

void log_info(const char *tag, const char *message, ...)
{
    va_list args;
    va_start(args, message);
    esp_log_writev(ESP_LOG_INFO, tag, message, args);
}
}  // namespace rover::hal