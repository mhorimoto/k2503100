#ifndef LIB_LCD_H
#define LIB_LCD_H
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_I2C_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2


#else  // LIB_LCD_H
void lcd_setup(void) ;
void lcd_display(int row, int col, char *str) ;
void lcd_clear(void) ;
void lcd_backlight(int on) ;
void lcd_cursor(int on) ;
void lcd_blink(int on) ;
void lcd_scrollDisplayLeft(void) ;
void lcd_scrollDisplayRight(void) ;
void lcd_setCursor(int col, int row) ;

#endif