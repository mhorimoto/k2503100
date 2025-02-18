#include "k2503100.h"
#include "lib-lcd.h"
#include "lib-snmp.h"
//#include "lib-button.h"

void setup(void) {
    extern void lcd_setup(void);
    extern void button_setup(void);
    extern byte oidbase[];
    extern byte community[];
    extern byte gateway[];
    extern byte NDIOTR[];
    extern byte netmask[];
    extern byte ip[];
    extern byte mac[];
    snmp_setup();    
    lcd_setup();
    button_setup();
    Serial.begin(115200);
    Serial.println("SNMP Set Example for Teensy 4.1 k2503100-11");
    lcd_display(0,0,"k2503100-12");

}

void loop(void) {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
}
