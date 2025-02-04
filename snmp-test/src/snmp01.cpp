#include <SPI.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

// Ethernet設定
byte mac[] = { 0x02, 0xA2, 0x73, 0x10, 0x00, 0x01 }; // TeensyのMACアドレス
IPAddress ip(192, 168, 42, 36);                     // TeensyのIPアドレス
IPAddress netmask(255, 255, 240,0);                 // サブネットマスク
IPAddress NDIOTR(192, 168, 42, 32);               // SNMPサーバーのIPアドレス
IPAddress gateway(192, 168, 42, 254);                  // ゲートウェイ
const int snmpPort = 161;                           // SNMPポート (通常161)

// SNMP設定
const char* community = "private"; // SNMPコミュニティ名 (v1用)
const char* oid =  "\x2B\x06\x01\x04\x01\x94\x1D\x01\x02\x02\x02\x06\x01\x05\x70"; // 設定するOID
// CEC-MIB::doTable.doEntry.doValue.112
const char* rlyon = "1";     // 設定する値
const char* value = "1";     // 設定する値
const char* rlyoff= "0";     // 設定する値

int buttonA2 = 0;
int pbA2 = 0;

EthernetUDP udp;

void setup() {
    pinMode(A2,INPUT_PULLUP);
    Serial.begin(115200);
    while (!Serial);

    Serial.println("SNMP Set Example for Teensy 4.1 k2503100-3");

    // Ethernet初期化
    Ethernet.begin(mac,ip,gateway,gateway,netmask);
    //Ethernet.begin(mac);
    
    Serial.print("IP Address: ");
    Serial.println(Ethernet.localIP());

    // UDP/SNMPの初期化
    udp.begin(9988);
}

void sendSnmpSet() {
    byte packet[1024]; // SNMPパケットを格納する配列
    int packetSize = 0;
    int communityLength = 0;

    communityLength = strlen(community);
    // `rlyon` を数値に変換
    int intValue = atoi(rlyon);
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
    packet[packetSize++] = 0x30;                          // Sequence
    packet[packetSize++] = 0;                            // Length (後で計算)

    packet[packetSize++] = 0x02;                          // Integer (Version)
    packet[packetSize++] = 0x01;
    packet[packetSize++] = 0x00;                          // SNMP v1

    packet[packetSize++] = 0x04;                          // Octet String (Community)
    packet[packetSize++] = communityLength;            // Community名の長さ
    memcpy(&packet[packetSize], community, communityLength+1);
    packetSize += communityLength;
    Serial.print("Community=");
    Serial.print(community);
    Serial.print("   Community length=");
    Serial.println(communityLength);
    
    packet[packetSize++] = 0xA3;                          // SNMP SET Request
    packet[packetSize++] = 0;                            // Length (後で計算)

    packet[packetSize++] = 0x02;                          // Integer (Request ID)
    packet[packetSize++] = 0x01;
    packet[packetSize++] = 0x01;

    packet[packetSize++] = 0x02;                          // Integer (Error Status)
    packet[packetSize++] = 0x01;
    packet[packetSize++] = 0x00;

    packet[packetSize++] = 0x02;                          // Integer (Error Index)
    packet[packetSize++] = 0x01;
    packet[packetSize++] = 0x00;

    packet[packetSize++] = 0x30;                          // Sequence (Varbind List)
    packet[packetSize++] = 0;                            // Length (後で計算)

    packet[packetSize++] = 0x30;                          // Sequence (Varbind)
    packet[packetSize++] = 0;                            // Length (後で計算)

    packet[packetSize++] = 0x06;                          // Object Identifier
    packet[packetSize++] = strlen(oid);                  // OIDの長さ
    Serial.print("OID length=");
    Serial.println(strlen(oid));
    memcpy(&packet[packetSize], oid, strlen(oid));
    packetSize += strlen(oid);

    packet[packetSize++] = 0x04;                          // Octet String (Value)
//    packet[packetSize++] = 0x02;                          // Octet String (Value)
//    packet[packetSize++] = valueSize;                     // 値の長さ
//    memcpy(&packet[packetSize], valueData, valueSize);
//    packetSize += valueSize;
    packet[packetSize++] = strlen(rlyon);                // 値の長さ
    memcpy(&packet[packetSize], rlyon, strlen(rlyon));
    packetSize += strlen(rlyon);

    // Varbindの長さを更新
    packet[29] = packetSize - 30;

    // Varbind Listの長さを更新
    packet[27] = packetSize - 28;

    // SNMP SET Requestの長さを更新
    packet[15] = packetSize - 16;

    // 全体の長さを更新
    packet[1] = packetSize - 2;

    // パケットを送信
    Serial.print("Packet size=");
    Serial.println(packetSize);
    udp.beginPacket(NDIOTR, snmpPort);
    udp.write(packet, packetSize);
    udp.endPacket();

    Serial.print("SNMP SET request sent! value=");
    Serial.println(value);
}


void loop() {
    buttonA2 = digitalRead(A2);
    if (buttonA2 == LOW && pbA2 == HIGH) {
        sendSnmpSet();
        pbA2 = buttonA2;
    } else if (buttonA2 == HIGH && pbA2 == LOW) {
        sendSnmpSet();
        pbA2 = buttonA2;
    }
}
