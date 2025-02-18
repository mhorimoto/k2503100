#include <Arduino.h>
#include "lib-snmp.h"

EthernetUDP udp;

void snmp_setup(void) {
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

    packet[pos++] = 0x30;     // Sequence (Varbind)
    packet[pos++] = 0x14;       // Length (後で計算)
    //int varbind1_length_pos = pos;  // Varbindの長さの位置を記憶するポインタ

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
