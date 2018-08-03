#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus

#include <WString.h>
void log_serial(const __FlashStringHelper *format, ...);

extern "C"
{
#endif

#include <stdbool.h>

#define LOG_NONE 0
#define LOG_UART 1
#define LOG_SOFT 2

void log_init(int type);
void log_serial(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
