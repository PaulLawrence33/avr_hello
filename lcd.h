#ifndef __LCD_H
#define __LCD_H

void lcd_init();
void lcd_clear();
void lcd_printf(uint8_t x, uint8_t y, const char *format, ...);
void lcd_printf_large(uint8_t x, uint8_t y, const char *format, ...);

#endif
