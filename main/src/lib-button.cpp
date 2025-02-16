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

    Serial.begin(115200);
    delay(3000);
    lcd.init();                      // initialize the lcd 
    lcd.backlight();
    lcd.setCursor(0,0);

    Serial.println("SNMP Set Example for Teensy 4.1 k2503100-11");
    lcd.print("k2503100-11");

    // Ethernet初期化
    Ethernet.init();
    Ethernet.begin(mac,ip,gateway,gateway,netmask);
    //Ethernet.begin(mac);
    
    Serial.print("IP Address: ");
    Serial.println(Ethernet.localIP());

    // UDP/SNMPの初期化
    udp.begin(9988);
}