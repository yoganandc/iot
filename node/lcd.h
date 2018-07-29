#ifndef LCD_H
#define LCD_H

#ifdef __cplusplus

#include "WString.h"
void lcd_msg(const __FlashStringHelper *format, ...);

extern "C"
{
#endif

#include <stdint.h>

void lcd_init();
void lcd_tabulate(uint8_t *costs, uint8_t *next_hops, uint8_t num, uint8_t skip);
void lcd_msg(const char *format, ...);
void lcd_status();

#ifdef __cplusplus
}
#endif

#endif
