#ifndef LIB_SNMP_H
#define LIB_SNMP_H
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>

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

#else  // LIB_SNMP_H
void snmp_setup(void);
int encode_length(uint8_t* buf, int pos, int length);
void sendSnmpSet(byte *o,int i,char *v);
extern EthernetUDP udp;
extern byte mac[];
extern IPAddress ip;
extern IPAddress netmask;
extern IPAddress NDIOTR;
extern IPAddress gateway;
extern const int snmpPort;
extern const char* community;
extern const char* oidbase;

#endif // LIB_SNMP_H
