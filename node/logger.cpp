#include <stdarg.h>
#include "Arduino.h"
#include "logger.h"

void log_serial(const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  Serial.vprintf(format, ap);
  Serial.flush();
  va_end(ap);
}

