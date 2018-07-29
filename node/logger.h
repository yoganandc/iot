#ifndef LOGGER_H
#define LOGGER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>

void log_init(bool connected);
void log_serial(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif
