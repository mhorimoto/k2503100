#include <Arduino.h>
#inclide "lib-lcd.h"

LiquidCrystal_I2C lcd(LCD_I2C_ADDR,LCD_COLS,LCD_ROWS);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void lcd_setup(void) {
    lcd.init();                      // initialize the lcd 
    lcd.backlight();
    lcd.setCursor(0,0);
}

void lcd_display(int row, int col, char *str) {
    lcd.setCursor(col,row);
    lcd.print(str);
}
void lcd_clear(void) {
    lcd.clear();
}
void lcd_backlight(int on) {
    if (on) {
        lcd.backlight();
    } else {
        lcd.noBacklight();
    }
}
void lcd_cursor(int on) {
    if (on) {
        lcd.cursor();
    } else {
        lcd.noCursor();
    }
}
void lcd_blink(int on) {
    if (on) {
        lcd.blink();
    } else {
        lcd.noBlink();
    }
}
void lcd_scrollDisplayLeft(void) {
    lcd.scrollDisplayLeft();
}
void lcd_scrollDisplayRight(void) {
    lcd.scrollDisplayRight();
}
void lcd_setCursor(int col, int row) {
    lcd.setCursor(col,row);
}