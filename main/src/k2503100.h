#include <Arduino.h>
#include <Wire.h> 
#include <SPI.h>
#include "lib-lcd.h"
#include "lib-snmp.h"

#define True  1
#define False 0

#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#include "TeensyTimerInterrupt.h"

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "Teensy_ISR_Timer.h"

#define HW_TIMER_INTERVAL_MS        1L
#define TIMER_INTERVAL_0_01S        1L
#define TIMER_INTERVAL_0_1S       100L
#define TIMER_INTERVAL_0_5S       500L
#define TIMER_INTERVAL_1S        1000L
#define TIMER_INTERVAL_1_5S      1500L
#define TIMER_INTERVAL_2S        2000L
#define TIMER_INTERVAL_3S        3000L

#define SW1 14
#define button_SIRENTEST 14
#define SW2 15
#define button_EMGSTOP 15
#define SW3 16
#define button_BROADCST 16
#define VR  A17
#define COL1  24
#define COL2  25
#define COL3  26
#define COL4  27
#define COL5  28
#define COL6  29
#define COL7  30
#define COL8  31

#define ROW1  2
#define ROW2  3
#define ROW3  4
#define ROW4  5

#define LED_BROADCAST  8

#define KCOL1 33
#define KCOL2 34
#define KCOL3 35
#define KCOL4 36
#define KCOL5 37
#define KCOL6 38
#define KCOL7 39
#define KCOL8 40

#define KROW1 20
#define KROW2 21
#define KROW3 22
#define KROW4 23

#define STS0   0    // Status 0 = Idle
#define STS1   1    // Status 1 = Selecting area (before broadcasting)
#define STS2   2    // Status 2 = Siren ON
#define STS3   3    // Status 3 = Voice ON and Mail Sent
#define STS3v  4    // Status 3V = Voice ON
#define STS3m  5    // Status 3M = Mail Sent

// ch defines MIB index is starting from 1
#define SirenCH01    1
#define SirenCH02    2
#define SirenCH03    3
#define SirenCH04    4
#define SirenCH05    5
#define SirenCH06    6
#define SirenCH07    7
#define SirenCH08    8
#define SirenCH09    9
#define SirenCH10   10
#define SirenCH11   11
#define SirenCH12   12
#define SirenCH13   13
#define SirenCH14   14
#define SirenCH15   15
#define SirenCH16   16
#define SirenCH17   17
#define SirenCH18   18
#define SirenCH19   19
#define SirenCH20   20
#define SirenCH21   21
#define SirenCH22   22
#define SirenCH23   23
#define SirenCH24   24
#define SirenCH25   25
#define SirenCH26   26
#define SirenCH27   27
#define SirenCH28   28
#define SirenCH29   29
#define SirenCH30   30
#define SirenCH31   31
#define SirenCH32   32
#define SirenTEST   65
#define VoiceCH01   33
#define VoiceCH02   34
#define VoiceCH03   35
#define VoiceCH04   36
#define VoiceCH05   37
#define VoiceCH06   38
#define VoiceCH07   39
#define VoiceCH08   40
#define VoiceCH09   41
#define VoiceCH10   42
#define VoiceCH11   43
#define VoiceCH12   44
#define VoiceCH13   45
#define VoiceCH14   46
#define VoiceCH15   47
#define VoiceCH16   48
#define VoiceCH17   49
#define VoiceCH18   50
#define VoiceCH19   51
#define VoiceCH20   52
#define VoiceCH21   53
#define VoiceCH22   54
#define VoiceCH23   55
#define VoiceCH24   56
#define VoiceCH25   57
#define VoiceCH26   58
#define VoiceCH27   59
#define VoiceCH28   60
#define VoiceCH29   61
#define VoiceCH30   62
#define VoiceCH31   63
#define VoiceCH32   64
#define EMG_STOP    66
#define MailCH01    81
#define MailCH02    82
#define MailCH03    83
#define MailCH04    84
#define MailCH05    85
#define MailCH06    86
#define MailCH07    87
#define MailCH08    88
#define MailCH09    89
#define MailCH10    90
#define MailCH11    91
#define MailCH12    92
#define MailCH13    93
#define MailCH14    94
#define MailCH15    95
#define MailCH16    96
#define MailCH17    97
#define MailCH18    98
#define MailCH19    99
#define MailCH20   100
#define MailCH21   101
#define MailCH22   102
#define MailCH23   103
#define MailCH24   104
#define MailCH25   105
#define MailCH26   106
#define MailCH27   107
#define MailCH28   108
#define MailCH29   109
#define MailCH30   110
#define MailCH31   111
#define MailCH32   112

