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
    esp_log_va(ESP_LOG_CONFIG_INIT(ESP_LOG_ERROR | ESP_LOG_CONFIGS_DEFAULT), tag, message, args);
    va_end(args);
}

void log_warning(const char *tag, const char *message, ...)
{
    va_list args;
    va_start(args, message);
    esp_log_va(ESP_LOG_CONFIG_INIT(ESP_LOG_WARN | ESP_LOG_CONFIGS_DEFAULT), tag, message, args);
    va_end(args);
}

void log_info(const char *tag, const char *message, ...)
{
    va_list args;
    va_start(args, message);
    esp_log_va(ESP_LOG_CONFIG_INIT(ESP_LOG_INFO | ESP_LOG_CONFIGS_DEFAULT), tag, message, args);
    va_end(args);
}
}  // namespace rover::hal