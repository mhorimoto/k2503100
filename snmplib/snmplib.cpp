#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OID_LEN 128  // OIDの最大長（十分なバッファサイズ）

/**
 * OID文字列 ("1.3.6.1.2.1.1.5.0") をバイナリ表現に変換する関数
 * @param oid_str ドット区切りのOID文字列
 * @param oid_bin 変換後のバイナリデータを格納する配列
 * @return バイナリデータの長さ（エラー時は -1）
 */
int convert_oid_to_binary(const char *oid_str, unsigned char *oid_bin) {
    int oid[MAX_OID_LEN];  // 一時的な数値配列
    int oid_len = 0;
    char *token;
    char oid_copy[256];

    if (!oid_str || !oid_bin) {
        return -1; // 入力が無効
    }

    // 文字列をコピー（strtokを使うため）
    strncpy(oid_copy, oid_str, sizeof(oid_copy) - 1);
    oid_copy[sizeof(oid_copy) - 1] = '\0';

    // OID文字列を整数配列に変換
    token = strtok(oid_copy, ".");
    while (token && oid_len < MAX_OID_LEN) {
        oid[oid_len++] = atoi(token);
        token = strtok(NULL, ".");
    }

    if (oid_len < 2) {
        return -1; // OIDが短すぎる
    }

    int bin_len = 0;

    // 最初の2つの値を特殊処理（(X * 40) + Y）
    oid_bin[bin_len++] = (oid[0] * 40) + oid[1];

    // 残りの値をASN.1 BERエンコード
    for (int i = 2; i < oid_len; i++) {
        int value = oid[i];
        unsigned char temp[5];  // 128進エンコード用の一時バッファ（最大5バイト）
        int temp_len = 0;

        if (value == 0) {
            oid_bin[bin_len++] = 0;
            continue;
        }

        // 128進エンコード
        while (value > 0) {
            temp[temp_len++] = (value & 0x7F) | (temp_len ? 0x80 : 0x00);
            value >>= 7;
        }

        // 順序を逆転して格納
        for (int j = temp_len - 1; j >= 0; j--) {
            oid_bin[bin_len++] = temp[j];
        }
    }

    return bin_len; // 変換後のバイナリ長
}

// 動作確認用
int main() {
    const char *oid_str = "1.3.6.1.4.1.2589.1.2.2.2.6.1.5.112";
    unsigned char oid_bin[MAX_OID_LEN];
    int bin_len = convert_oid_to_binary(oid_str, oid_bin);

    if (bin_len > 0) {
        printf("Binary OID: ");
        for (int i = 0; i < bin_len; i++) {
            printf("\\x%02X", oid_bin[i]);
        }
        printf("\n");
    } else {
        printf("OID変換に失敗しました。\n");
    }
}
