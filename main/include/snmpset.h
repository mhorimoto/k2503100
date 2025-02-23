// Ethernet設定
extern byte mac[]; // TeensyのMACアドレス
extern IPAddress ip();                     // TeensyのIPアドレス
extern IPAddress netmask();                 // サブネットマスク
extern IPAddress NDIOTR();               // SNMPサーバーのIPアドレス
extern IPAddress gateway();                  // ゲートウェイ
// byte mac[] = { 0x02, 0xA2, 0x73, 0x10, 0x00, 0x01 }; // TeensyのMACアドレス
// IPAddress ip(192, 168, 42, 36);                     // TeensyのIPアドレス
// IPAddress netmask(255, 255, 240,0);                 // サブネットマスク
// IPAddress NDIOTR(192, 168, 42, 32);               // SNMPサーバーのIPアドレス
// IPAddress gateway(192, 168, 42, 254);                  // ゲートウェイ

// SNMP設定
extern const int snmpPort;
extern const char *community;
extern const char *oidbase;
extern const char *rlyon;
extern const char *xrlyoff;


//const char* community = "private"; // SNMPコミュニティ名 (v1用)
////const char* oidbase =  "\x2B\x06\x01\x04\x01\x94\x1D\x01\x02\x02\x02\x06\x01\x05\x70"; // 設定するOID
//// CEC-MIB::doTable.doEntry.doValue.112
//const char* oidbase =  "\x2B\x06\x01\x04\x01\x94\x1D\x01\x02\x02\x02\x06\x01\x05"; // 設定するOID
//const char* rlyon = "1";     // 設定する値
//const char* rlyoff= "0";     // 設定する値

