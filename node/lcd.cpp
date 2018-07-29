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

  for(int i = 0; i < NUM_NODES; i++) {
    int row = i % 4;
    int col = (i/4) * 7;
    lcd->setCursor(col, row);

    lcd->print(i + 1, HEX);
    if(col == 14) {
      lcd->print(F("|?|FF"));
    }
    else {
      lcd->print(F("|?|FF "));
    }
  }
}

void lcd_tabulate(uint8_t *costs, uint8_t *next_hops, uint8_t num)
{
  for(int i = 0; i < NUM_NODES; i++) {
    int row = i % 4;
    int col = (i/4) * 7;
    lcd->setCursor(col, row);

    if(col == 14) {
      if(i < num) {
        lcd_printf(F("%1X|%1X|%02X"), i + 1, next_hops[i], costs[i]);
      }
      else {
        lcd_printf(F("%1X|?|FF"), i + 1);
      }
    }
    else {
      if(i < num) {
        lcd_printf(F("%1X|%1X|%02X "), i + 1, next_hops[i], costs[i]);
      }
      else {
        lcd_printf(F("%1X|?|FF "), i + 1);
      }
    }
  }
}

void lcd_status()
{
  
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

