#include "lib-button.h"

void button_setup(void) {
    pinMode(VR,INPUT);
    pinMode(button_SIRENTEST,INPUT_PULLUP);
    pinMode(button_BROADCST,INPUT_PULLUP);
    pinMode(button_EMGSTOP,INPUT_PULLUP);
    pinMode(LED_BROADCAST,OUTPUT);
    pinMode(KROW1,OUTPUT);
    pinMode(KROW2,OUTPUT);
    pinMode(KROW3,OUTPUT);
    pinMode(KROW4,OUTPUT);
    pinMode(KCOL1,INPUT_PULLDOWN);
    pinMode(KCOL2,INPUT_PULLDOWN);
    pinMode(KCOL3,INPUT_PULLDOWN);
    pinMode(KCOL4,INPUT_PULLDOWN);
    pinMode(KCOL5,INPUT_PULLDOWN);
    pinMode(KCOL6,INPUT_PULLDOWN);
    pinMode(KCOL7,INPUT_PULLDOWN);
    pinMode(KCOL8,INPUT_PULLDOWN);
    pinMode(COL1, OUTPUT);
    pinMode(COL2, OUTPUT);
    pinMode(COL3, OUTPUT);
    pinMode(COL4, OUTPUT);
    pinMode(COL5, OUTPUT);
    pinMode(COL6, OUTPUT);
    pinMode(COL7, OUTPUT);
    pinMode(COL8, OUTPUT);

}