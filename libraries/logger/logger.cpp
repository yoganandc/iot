#include <AltSoftSerial.h>
#include <stdarg.h>
#include <stdbool.h>
#include <Arduino.h>
#include <WString.h>
#include "logger.h"

static Stream *stream;

void log_init(int type)
{
  static AltSoftSerial soft_serial;

  if(type == LOG_UART) {
    stream = &Serial;
  }
  else if(type == LOG_SOFT) {
    soft_serial.begin(57600);
    stream = &soft_serial;
  }
}

void log_serial(const char *format, ...)
{
  if(stream) {
    va_list ap;
    va_start(ap, format);
    stream->vprintf(format, ap);
    va_end(ap);
  }
}

void log_serial(const __FlashStringHelper *format, ...)
{
  if(stream) {
    va_list ap;
    va_start(ap, format);
    stream->vprintf(format, ap);
    va_end(ap);
  }
}

