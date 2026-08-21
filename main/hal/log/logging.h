/**
 * @file logging.h
 *
 * @brief Functions for logging.
 */
#pragma once

#include <cstdarg>
#include <string>

namespace rover::hal
{
/**
 * @brief Log an error.
 */
void log_error(const char *tag, const char *message, ...);

/**
 * @brief Log a warning.
 */
void log_warning(const char *tag, const char *message, ...);

/**
 * @brief Log info.
 */
void log_info(const char *tag, const char *message, ...);

}  // namespace rover::hal