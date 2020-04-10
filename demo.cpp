//
// Created by liubo on 2020/3/26.
//

#include "databuffer.h"
#include "encryption.h"

void show_hex(const char *data, int len, const char *str) {
    unsigned char *d = (unsigned char *) data;
    if (str != NULL)
        printf("%s:", str);
    for (int i = 0; i < len; i++) {
        printf("%02x ", d[i]);
    }

    printf("\n");

    return;
}

bool cmp(const char *s1, int l1, const char *s2, int l2) {
    if (l1 != l2) {
        return false;
    }

    return memcmp(s1, s2, l1) == 0;
}

void test_tea(const char *data, int len) {
    DataBuffer encrypt_data, decrypt_data;
    encrypt(data, len, encrypt_data);
    decrypt(encrypt_data.getData(), encrypt_data.getDataLen(), decrypt_data);

    if (cmp(data, len, decrypt_data.getData(), decrypt_data.getDataLen())) {
        printf("test success!\n");
    } else {
        printf("test fail!!!\n");
    }
}

int main() {
    //测试一个字符串；
    const char *str = "hello china ....... ";
    test_tea(str, strlen(str));

    //测试一个二进制的数据包
    char data[1024] = {0x01};
    test_tea(data, 1024);
}
