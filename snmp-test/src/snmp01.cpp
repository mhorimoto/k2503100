#include <SPI.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <LiquidCrystal_I2C.h>

#define SW1 14
#define SW2 15
#define SW3 16

// Ethernet設定
byte mac[] = { 0x02, 0xA2, 0x73, 0x10, 0x00, 0x01 }; // TeensyのMACアドレス
IPAddress ip(192, 168, 42, 36);                     // TeensyのIPアドレス
IPAddress netmask(255, 255, 240,0);                 // サブネットマスク
IPAddress NDIOTR(192, 168, 42, 32);               // SNMPサーバーのIPアドレス
IPAddress gateway(192, 168, 42, 254);                  // ゲートウェイ

// SNMP設定
const int snmpPort = 161;                           // SNMPポート (通常161)
const char* community = "private"; // SNMPコミュニティ名 (v1用)
//const char* oidbase =  "\x2B\x06\x01\x04\x01\x94\x1D\x01\x02\x02\x02\x06\x01\x05\x70"; // 設定するOID
// CEC-MIB::doTable.doEntry.doValue.112
const char* oidbase =  "\x2B\x06\x01\x04\x01\x94\x1D\x01\x02\x02\x02\x06\x01\x05"; // 設定するOID
const char* rlyon = "1";     // 設定する値
const char* value = "1";     // 設定する値
const char* rlyoff= "0";     // 設定する値

int buttonSW1 = HIGH;
int pbSW1 = HIGH;
int buttonSW2 = HIGH;
int pbSW2 = HIGH;
int buttonSW3 = HIGH;
int pbSW3 = HIGH;


LiquidCrystal_I2C lcd(0x27,16,2);
EthernetUDP udp;

void setup() {
    pinMode(SW1,INPUT_PULLUP);
    pinMode(SW2,INPUT_PULLUP);
    pinMode(SW3,INPUT_PULLUP);
    Serial.begin(115200);
    delay(3000);
    lcd.init();                      // initialize the lcd 
    lcd.backlight();
    lcd.setCursor(0,0);

    Serial.println("SNMP Set Example for Teensy 4.1 k2503100-9");
    lcd.print("k2503100-9");

    // Ethernet初期化
    Ethernet.init();
    Ethernet.begin(mac,ip,gateway,gateway,netmask);
    //Ethernet.begin(mac);
    
    Serial.print("IP Address: ");
    Serial.println(Ethernet.localIP());

    // UDP/SNMPの初期化
    udp.begin(9988);
}

int encode_length(uint8_t* buf, int pos, int length) {
    if (length < 128) {
        // 127 バイト以下はそのまま
        buf[pos] = length;
        return 1;
    } else if (length < 256) {
        // 128-255 バイト (1バイト拡張)
        buf[pos] = 0x81;
        buf[pos + 1] = length;
        return 2;
    } else {
        // 256 バイト以上 (2バイト拡張)
        buf[pos] = 0x82;
        buf[pos + 1] = (length >> 8) & 0xFF;
        buf[pos + 2] = length & 0xFF;
        return 3;
    }
}

void sendSnmpSet(byte *o,int i,char *v) {
    byte packet[1024]; // SNMPパケットを格納する配列
    int pos = 0;
    int communityLength = 0;

    communityLength = strlen(community);
    // `rlyon` を数値に変換
    int intValue = atoi(v);
    byte valueData[4];  // 32bit整数用
    int valueSize = 0;

    // 数値を ASN.1 INTEGER のバイト列に変換
    if (intValue < 128) {  // 1バイトで表現可能
        valueData[0] = intValue;
        valueSize = 1;
    } else if (intValue < 32768) { // 2バイトで表現可能
        valueData[0] = (intValue >> 8) & 0xFF;
        valueData[1] = intValue & 0xFF;
        valueSize = 2;
    } else { // 4バイト (最大32bit)
        valueData[0] = (intValue >> 24) & 0xFF;
        valueData[1] = (intValue >> 16) & 0xFF;
        valueData[2] = (intValue >> 8) & 0xFF;
        valueData[3] = intValue & 0xFF;
        valueSize = 4;
    }    

    // SNMPメッセージを構築
    packet[pos++] = 0x30; // Sequence
    int snmp_length_pos = pos; // SNMPの長さの位置を記憶するポインタ
    packet[pos++] = 0x32; // 長さ(計算後に修正)
    //packet[pos++] = 0x0; 
    //packet[pos++] = 0x0; 
    //packet[pos++] = 0x0; 

    // SNMP Version 1
    packet[pos++] = 0x02; // Integer (Version)
    packet[pos++] = 0x01; // 長さ
    packet[pos++] = 0x00; // SNMP v1

    // Set community
    packet[pos++] = 0x04; // Octet String (Community)
    packet[pos++] = communityLength;            // Community名の長さ
    memcpy(&packet[pos], community, communityLength);
    pos += communityLength;
    Serial.print("Community=");
    Serial.print(community);
    Serial.print("   Community length=");
    Serial.println(communityLength);
    
    // SNMP SET REQUEST PDU
    packet[pos++] = 0xA3;   // SNMP SET Request
    packet[pos++] = 0x24;   // Length (後で計算)
    //int pdu_length_pos = pos;  // PDUの長さの位置を記憶するポインタ
    //packet[pos++] = 0x0; 
    //packet[pos++] = 0x0; 
    //packet[pos++] = 0x0; 

    // SNMP REQUEST ID
    packet[pos++] = 0x02;   // Integer (Request ID)
    packet[pos++] = 0x04;   // Length = 4
    uint32_t requestId = random(0, 0x7FFFFFFF); // Request ID
    memcpy(&packet[pos], &requestId, 4);
    pos += 4;

    // Error Status & Error Index
    packet[pos++] = 0x02;     // Integer (Error Status)
    packet[pos++] = 0x01;
    packet[pos++] = 0x00;
    packet[pos++] = 0x02;     // Integer (Error Index)
    packet[pos++] = 0x01;
    packet[pos++] = 0x00;

    // Variable Bindings
    packet[pos++] = 0x30;     // Sequence (Varbind List)
    packet[pos++] = 0x16;       // Length
    //int varbind_length_pos = pos;  // Varbind Listの長さの位置を記憶するポインタ
    //packet[pos++] = 0x0; 
    //packet[pos++] = 0x0; 
    //packet[pos++] = 0x0; 

    packet[pos++] = 0x30;     // Sequence (Varbind)
    packet[pos++] = 0x14;       // Length (後で計算)
    //int varbind1_length_pos = pos;  // Varbindの長さの位置を記憶するポインタ
    //packet[pos++] = 0x0; 
    //packet[pos++] = 0x0; 
    //packet[pos++] = 0x0; 

    // Object Identifier
    packet[pos++] = 0x06;                          // Object Identifier
    packet[pos++] = strlen(o)+1;                  // OIDの長さ
    Serial.print("OID length=");
    Serial.println(strlen(o));
    memcpy(&packet[pos], o, strlen(o));
    pos += strlen(o);
    packet[pos++] = i; // インデックス
    // Value
    packet[pos++] = 0x02;                          // Octet String (Value)
    packet[pos++] = valueSize;                     // 値の長さ
    memcpy(&packet[pos], valueData, valueSize);
    pos += valueSize;

    //encode_length(packet,varbind1_length_pos,(pos-varbind1_length_pos-1));
    //encode_length(packet,varbind_length_pos,(pos-varbind_length_pos-1));
    //encode_length(packet,pdu_length_pos,(pos-pdu_length_pos-3));
    encode_length(packet,snmp_length_pos,(pos-snmp_length_pos-1));

    Serial.print("Packet data: ");
    for (int i = 0; i < pos; i++) {
        Serial.print("0x");
        Serial.print(packet[i], HEX);
        Serial.print(" ");
    }
    Serial.println();

    // パケットを送信
    Serial.print("Packet size=");
    Serial.println(pos);
    udp.beginPacket(NDIOTR, snmpPort);
    size_t bytesSent = udp.write(packet, pos);
    udp.endPacket();
    Serial.print("Bytes sent: ");
    Serial.println(bytesSent);

    if (bytesSent == 0) {
        Serial.println("Failed to send SNMP SET request!");
    } else {
        Serial.println("SNMP SET request sent!");
    }
}


void loop() {
    buttonSW3 = digitalRead(A2);
    if (buttonSW3 == LOW && pbSW3 == HIGH) {
        Serial.print("Button pushed: ");
        Serial.print("SW3=");
        Serial.print(buttonSW3); 
        Serial.print(" pbSW3=");
        Serial.println(pbSW3);
        lcd.setCursor(0,1);
        lcd.print("                ");
        lcd.setCursor(0,1);
        lcd.print("Button pushed");
        sendSnmpSet(oidbase,112,rlyon);
        pbSW3 = buttonSW3;
    } else if (buttonSW3 == HIGH && pbSW3 == LOW) {
        Serial.print("Button released: ");
        Serial.print("SW3=");
        Serial.print(buttonSW3); 
        Serial.print(" pbSW3=");
        Serial.println(pbSW3);
        sendSnmpSet(oidbase,112,rlyoff);
        lcd.setCursor(0,1);
        lcd.print("             ");
        lcd.setCursor(0,1);
        lcd.print("Button released");
        pbSW3 = buttonSW3;
    }
}
