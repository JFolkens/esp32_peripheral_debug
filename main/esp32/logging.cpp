#include "../hal/log/logging.h"

extern "C" {
#include "esp_log.h"
}

namespace rover::hal
{

void log_error(const char *tag, const char *message)
{
    ESP_LOGE(tag, "%s", message);
}

void log_warning(const char *tag, const char *message)
{
    ESP_LOGW(tag, "%s", message);
}

void log_info(const char *tag, const char *message)
{
    ESP_LOGI(tag, "%s", message);
}
}  // namespace rover::hal