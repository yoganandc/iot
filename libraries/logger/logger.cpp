#include <stdarg.h>
#include <stdbool.h>
#include "Arduino.h"
#include "WString.h"
#include "logger.h"

static bool connected;

void log_init(bool is_connected)
{
  connected = is_connected;
}

void log_serial(const char *format, ...)
{
  if(connected) {
    va_list ap;
    va_start(ap, format);
    Serial.vprintf(format, ap);
    va_end(ap);
  }
}

void log_serial(const __FlashStringHelper *format, ...)
{
  if(connected) {
    va_list ap;
    va_start(ap, format);
    Serial.vprintf(format, ap);
    va_end(ap);
  }
}

