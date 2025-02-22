# k2503100
リレーは使わない、SNMPだけ送信する＆Teensy4.1

## ディレクトリの説明

+ DOCS  
  設計に関する検討資料が含まれている。
+ KiCAD  
  KiCADによる回路設計の情報が含まれている。
  本装置はボタンスイッチを配置した基板が本体ケースの構成要素として大きな位置づけなので、
  本体ケースの製作に要する寸法図データもこのディレクトリに含めている。
+ PARTS_SPEC  
  製作に使用した部品の個別の仕様書やカタログが含まれている。
+ main  
  プログラムのソースコードが収められている。
  コンパイルなどはplatformIOを用いている。
+ snmp-manager  
  snmp-managerライブラリを用いた試験プログラムが収められている。普通は使わない。
+ snmp-test  
  snmp動作のテストプログラムが収められている。普通は使わない。
+ snmplib  
  snmpsetの動作確認用のプログラムが収められている。普通は使わない。
+ tools
  Ubuntu(Debian)系でplatformIOを使うときのudevruleファイルが収められている。

# CAUTION VERY IMPORTANT 重要な注意

PCB Revision 1.20まではLEDドライブ用のTrのフットパターンを間違えているので使わないように。

Don't use the wrong foot pattern of Tr for LED drive until PCB Revision 1.20.
