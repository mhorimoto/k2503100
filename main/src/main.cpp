#include <Arduino.h>
#include <Wire.h> 
#include <SPI.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <LiquidCrystal_I2C.h>
#include "k2503100.h"

#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#include "TeensyTimerInterrupt.h"

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "Teensy_ISR_Timer.h"


// You can select Teensy Hardware Timer  from TEENSY_TIMER_1 or TEENSY_TIMER_3
// Init Teensy timer TEENSY_TIMER_1
TeensyTimer ITimer(TEENSY_TIMER_1);
// Init Teensy_ISR_Timer
// Each Teensy_ISR_Timer can service 16 different ISR-based timers
Teensy_ISR_Timer ISR_Timer;

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
EthernetUDP udp;

volatile bool sirentest_flag,emgstop_flag,broadcast_flag;
volatile bool t1,t1t,t2,t3,bt;
volatile int  c,r;
int p1=0,p2=0,p3=0;
int lc=1 ;
int a17vr,a17pvr;
int sirenTimer,voiceTimer,mailTimer;
int psw;
int led[8][4] = {};


void TimerHandler()
{
  ISR_Timer.run();
}

void doingSomething1(void) {
    void sendSnmpSet(IPAddress,const char *,int,char *,const int,const char *);
    void LCDPrint1Line(int,const char *);
    extern const char *rlyoff;
    extern const char *oidbase;
    extern int psw;

    if (sirenTimer>0) {
        if (sirenTimer==1) {
            sendSnmpSet(NDIOTR,oidbase,SirenTEST,rlyoff,snmpPort,community);
            LCDPrint1Line(1," ");
            if (!broadcast_flag) {
                psw = STS0;
            }
        }
        sirenTimer--;
    } else {
        sirenTimer=0;
    }
    if (mailTimer>0) {
        mailTimer--;
    }
    if (voiceTimer>0) {
        voiceTimer--;
    }
    lcd.setCursor(15,0);
    lcd.print(psw);
}

void doingSomething3(void) {
    Serial.println("3sec");
}

//void cyclic1s() {
//
//}

void cyclic() {
    int lr,lc,k;
    digitalWrite(LED_BUILTIN,0);
    for (lr=0;lr<4;lr++) {
        digitalWrite((lr+ROW1),1);
        for (lc=0;lc<8;lc++) {
            digitalWrite((lc+COL1),led[lc][lr]);
//            digitalWrite((lc+COL1),1);
        }
        delayMicroseconds(200);
        k = ~k;
        digitalWrite((lr+ROW1),0);
    }
    digitalWrite(LED_BUILTIN,1);
}

void bufprt(void) {
    char tbuf[80];
    Serial.begin(115200);

    for (r=0;r<4;r++) {
        sprintf(tbuf,"%d %d %d %d %d %d %d %d",led[0][r],led[1][r],led[2][r],led[3][r],
                    led[4][r],led[5][r],led[6][r],led[7][r]);
        Serial.println(tbuf);
    }
}

const char *ver = "V0.15";
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield

void setup(void) {
    //char tbuf[80];
    bool stopflag = False;
    int t1,t2,t3;
    extern void pushsw1(void),pushsw2(void),pushsw3(void),LCDPrint1Line(int,const char *);
    extern void setPinMode(void);
    extern void sirentest(void),emgstop(void),broadcast(void);

    c = 0;
    r = 0;
    t1t = True;

    Serial.begin(115200);
    setPinMode();
    digitalWrite(LED_BROADCAST,LOW);

    // LCD初期化
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print(ver);

    // Ethernet初期化
    Serial.println("Initialize Ethernet");
    LCDPrint1Line(1,"INIT NET");
    Ethernet.init();
    Ethernet.begin(mac,ip,gateway,gateway,netmask);
    //
    // Ethernetの接続エラーに関して書く
    //
    // no point in carrying on, so do nothing forevermore:
    //
    stopflag = False;
    while (stopflag) {
        delay(1);
    }

    // print your local IP address:
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
    // UDP/SNMPの初期化
    udp.begin(9988);

    bt = False;
    digitalWrite(KROW1,LOW);
    digitalWrite(KROW2,LOW);
    digitalWrite(KROW3,LOW);
    digitalWrite(KROW4,LOW);
    
    Serial.println(ver);
    bufprt();
    attachInterrupt(digitalPinToInterrupt(button_SIRENTEST),sirentest,FALLING);
    attachInterrupt(digitalPinToInterrupt(button_EMGSTOP),emgstop,FALLING);
    attachInterrupt(digitalPinToInterrupt(button_BROADCAST),broadcast,FALLING);
// Interval in microsecs
    if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_MS * 1000, TimerHandler)) {
        Serial.print(F("Starting ITimer OK, millis() = ")); Serial.println(millis());
    } else {
        Serial.println(F("Can't set ITimer. Select another freq. or timer"));
    }
    // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
    // You can use up to 16 timer for each ISR_Timer
    ISR_Timer.setInterval(TIMER_INTERVAL_1S,  doingSomething1);
    //ISR_Timer.setInterval(TIMER_INTERVAL_2S,  doingSomething2);
    //ISR_Timer.setInterval(TIMER_INTERVAL_3S,  doingSomething3);
    ISR_Timer.setInterval(TIMER_INTERVAL_0_01S,cyclic);  // 10mSec Interval
    //ISR_Timer.setInterval(TIMER_INTERVAL_1S,cyclic1s);
    lcd.print(ver);
    sirenTimer = 0;
    voiceTimer = 0;
    mailTimer = 0;
    psw = STS0;
}

int chkcols(int r) {
    int v;
    delay(50);
    for(v=0;v<8;v++) {
        if (digitalRead(v+33)==HIGH) {
            bt = True;
            return(v+1);
        }
    }
    bt = False;
    return(0);
}
void loop(void) {
    int lc,lr,idx;
    void sendSnmpSet(IPAddress,const char *,int,char *,const int,const char *);
    extern void LCDPrint1Line(int,const char *);
    extern const char *rlyoff, *rlyon;
    extern const char *oidbase;
    extern void dhcpmnt(void);

    r++;
    if (r>4) r=1;
    digitalWrite(KROW1,LOW);
    digitalWrite(KROW2,LOW);
    digitalWrite(KROW3,LOW);
    digitalWrite(KROW4,LOW);
    a17vr = analogRead(VR);
    switch(r) {
        case 1:
            digitalWrite(KROW1,HIGH);
            c = chkcols(r);
            break;
        case 2:
            digitalWrite(KROW2,HIGH);
            c = chkcols(r);
            break;
        case 3:
            digitalWrite(KROW3,HIGH);
            c = chkcols(r);
            break;
        case 4:
            digitalWrite(KROW4,HIGH);
            c = chkcols(r);
            break;
    }
    if (psw==STS0) {
        if (sirentest_flag) {
            sirentest_flag = False;
            Serial.println("Siren Test");
            LCDPrint1Line(1,"SIREN TEST");
            if (sirenTimer==0) {
                sendSnmpSet(NDIOTR,oidbase,SirenTEST,rlyon,snmpPort,community);
                sirenTimer = 3;
                psw = STS2;
            } else {
                sirenTimer = 0;
            }
        }
    }
    if (emgstop_flag) {
        emgstop_flag = False;
        Serial.println("EMERGENCY STOP");
        LCDPrint1Line(1,"EMERGENCY STOP");
        psw = STS0;
        sirentest_flag = False;
        broadcast_flag = False;
        for(lc=0;lc<8;lc++) {
            for (int lr=0;lr<4;lr++) {
                led[lc][lr] = 0;
            }
        }
        digitalWrite(LED_BROADCAST,LOW);
        sirenTimer = 0;

        for(int x=1;x<=MailCH32;x++) {
            sendSnmpSet(NDIOTR,oidbase,x,rlyoff,snmpPort,community);
            delay(10);
        }
        sendSnmpSet(NDIOTR,oidbase,EMG_STOP,rlyon,snmpPort,community);
        delay(1000);
        sendSnmpSet(NDIOTR,oidbase,EMG_STOP,rlyoff,snmpPort,community);
        psw = STS0;
        LCDPrint1Line(1," ");
    }
    if (broadcast_flag) {
        if (psw==STS1) {
            Serial.println("Broadcast siren");
            LCDPrint1Line(1,"BROADCAST SIREN");
            for(lc=0;lc<8;lc++) {
                for(lr=0;lr<4;lr++) {
                    idx = lc + (lr*8);
                    if (led[lc][lr]==1) {
                        sendSnmpSet(NDIOTR,oidbase,SirenCH01+idx,rlyon,snmpPort,community);
                    } else {
                        sendSnmpSet(NDIOTR,oidbase,SirenCH01+idx,rlyoff,snmpPort,community);
                    }
                }
            }
            sirenTimer = 5;
            psw = STS2;
            digitalWrite(LED_BROADCAST,HIGH);
        } else if (psw==STS2) {
            if (sirenTimer==0) {
                psw = STS3;
                // Stop siren
                for(idx=1;idx<=SirenCH32;idx++) {
                    sendSnmpSet(NDIOTR,oidbase,idx,rlyoff,snmpPort,community);
                    delay(10);
                }
                LCDPrint1Line(1,"VOICE AND MAIL");
                for (lc=0;lc<8;lc++) {
                    for (lr=0;lr<4;lr++) {
                        if (led[lc][lr]==1) {
                            sendSnmpSet(NDIOTR,oidbase,VoiceCH01+(lc+(lr*8)),rlyon,snmpPort,community);
                            delay(10);
                            sendSnmpSet(NDIOTR,oidbase,MailCH01+(lc+(lr*8)),rlyon,snmpPort,community);
                            delay(10);
                        } else {
                            sendSnmpSet(NDIOTR,oidbase,VoiceCH01+(lc+(lr*8)),rlyoff,snmpPort,community);
                            delay(10);
                            sendSnmpSet(NDIOTR,oidbase,MailCH01+(lc+(lr*8)),rlyoff,snmpPort,community);
                            delay(10);
                        }
                    }
                }
                mailTimer = 3;
                voiceTimer = 60;
            }
        } else if (psw==STS3) {
            if (mailTimer==0) {
                for (idx=0;idx<32;idx++) {
                    sendSnmpSet(NDIOTR,oidbase,MailCH01+idx,rlyoff,snmpPort,community);
                    delay(10);
                }
                LCDPrint1Line(1,"VOICE");
                mailTimer = 0;
                psw = STS3v;
            }
        } else if (psw==STS3v) {
            if (voiceTimer==0) {
                for (idx=0;idx<32;idx++) {
                    sendSnmpSet(NDIOTR,oidbase,VoiceCH01+idx,rlyoff,snmpPort,community);
                    delay(10);
                }
                for(lc=0;lc<8;lc++) {
                    for(lr=0;lr<4;lr++) {
                        led[lc][lr]=0;
                    }
                }   
                LCDPrint1Line(1,"END");
                mailTimer = 0;
                psw = STS0;
                broadcast_flag = False;
                digitalWrite(LED_BROADCAST,LOW);
            }
        }
    }
    if (bt) {
        led[c-1][r-1] = (led[c-1][r-1]==0)? 1:0;
        Serial.print("CR=");
        Serial.print(c);
        Serial.print("x");
        Serial.println(r);
        bt = False;
        // check led status
        int z = 0;
        for (lc=0;lc<8;lc++) {
            for (lr=0;lr<4;lr++) {
                z += led[lc][lr];
            }
            if (z>0) {
                psw=STS1;
                sirentest_flag=False;
            } else {
                psw=STS0;
                sirentest_flag=False;
            }
        }
    }
    if (a17vr!=a17pvr) {
        a17pvr = a17vr;
    }
}

void dhcpmnt(void) {
    return;
    switch (Ethernet.maintain()) {
    case 1:
        //renewed fail
        Serial.println("Error: renewed fail");
        break;
    case 2:
        //renewed success
        Serial.println("Renewed success");
        //print your local IP address:
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
        break;
    case 3:
        //rebind fail
        Serial.println("Error: rebind fail");
        break;
    case 4:
        //rebind success
        Serial.println("Rebind success");
        //print your local IP address:
        Serial.print("My IP address: ");
        Serial.println(Ethernet.localIP());
        break;
    default:
        //nothing happened
        break;
    }
}

void LCDPrint1Line(int l,const char *str) {
    lcd.setCursor(0,l);
    lcd.print("                   ");
    lcd.setCursor(0,l);
    lcd.print(str);
}
 
void sirentest(void) {
    delay(5);
    int s1 = digitalRead(button_SIRENTEST);
    if (s1==sirentest_flag) sirentest_flag = 1;
}

void emgstop(void) {
    delay(5);
    int s2 = digitalRead(button_EMGSTOP);
    if (s2==emgstop_flag) emgstop_flag = 1;
}

void broadcast(void) {
    delay(5);
    int s2 = digitalRead(button_BROADCAST);
    if (s2==broadcast_flag) broadcast_flag = 1;
}