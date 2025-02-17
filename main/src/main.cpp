#include "k2503100.h"

void setup(void) {
    lcd_setup();
    button_setup();
    Serial.begin(115200);
    Serial.println("SNMP Set Example for Teensy 4.1 k2503100-11");
    lcd.print("k2503100-12");

}

void loop(void) {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);
  delay(1000);
  digitalWrite(13, LOW);
  delay(1000);
}
