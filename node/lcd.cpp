#include <LiquidCrystal.h>
#include <stdint.h>
#include <stdarg.h>
#include "Arduino.h"
#include "WString.h"
#include "lcd.h"
#include "logger.h"

#define V0 5
#define CONTRAST 0.9
#define NUM_NODES 12

static LiquidCrystal *lcd;

static void lcd_printf(const char *format, ...);
static void lcd_printf(const __FlashStringHelper *format, ...);

void lcd_init()
{
  pinMode(V0, OUTPUT);
  analogWrite(V0, (1 - CONTRAST) * 255);
  
  static LiquidCrystal lcd0(19, 18, 17, 16, 15, 14);
  lcd = &lcd0;

  lcd->begin(20, 4);
  lcd_printf(F("Starting up..."));
}

void lcd_tabulate(uint8_t *costs, uint8_t *next_hops, uint8_t num, uint8_t skip)
{
  bool skipped = false;
  
  for(int i = 0; i < NUM_NODES; i++) {
    lcd->setCursor((i/4) * 7, i % 4);
    
    if(i == skip) {
      skipped = true;
    }
    
    if(i > 7) { // last col
      if(i < num && costs[i + skipped] < 0xFF) {
        lcd_printf(F("%1X|%1X|%02X"), i + skipped, next_hops[i + skipped], costs[i + skipped]);
      }
      else {
        lcd_printf(F("%1X|?|FF"), i + skipped);
      }
    }
    else {
      if(i < num && costs[i + skipped] < 0xFF) {
        lcd_printf(F("%1X|%1X|%02X "), i + skipped, next_hops[i + skipped], costs[i + skipped]);
      }
      else {
        lcd_printf(F("%1X|?|FF "), i + skipped);
      }
    }
  }
}

void lcd_status()
{
  
}

void lcd_msg(const __FlashStringHelper *format, ...)
{
  lcd->clear();
  lcd->home();
  
  va_list ap;
  va_start(ap, format);
  lcd->vprintf(format, ap);
  va_end(ap);
}

void lcd_msg(const char *format, ...)
{
  lcd->clear();
  lcd->home();
  
  va_list ap;
  va_start(ap, format);
  lcd->vprintf(format, ap);
  va_end(ap);
}

static void lcd_printf(const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  lcd->vprintf(format, ap);
  va_end(ap);
}

static void lcd_printf(const __FlashStringHelper *format, ...)
{
  va_list ap;
  va_start(ap, format);
  lcd->vprintf(format, ap);
  va_end(ap);
}

