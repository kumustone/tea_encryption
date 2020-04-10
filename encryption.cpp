//
// Created by liubo on 2020/3/26.
//

#include <stdio.h>
#include <cstdint>
#include "databuffer.h"

#define TEA_BLOCK_SIZE 8

static uint32_t tea_key[4] = {
        0x34561234, 0x111f3423, 0x34d57910, 0x00989034
};

static uint32_t salt = 0x12031243;
static int tea_round = 16;

static void confuse_key(uint32_t *key) {
    for (int i = 4; i > 0; i--) {
        key[4 - i] = tea_key[i - 1] ^ salt;
    }
}

static void tea_encrypt(uint32_t *v, uint32_t *k) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0, i;
    uint32_t delta = 0x9e3779b9;
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];

    for (i = 0; i < tea_round; i++) {
        sum += delta;
        v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
    }

    v[0] = v0;
    v[1] = v1;
}

static void tea_decrypt(uint32_t *v, uint32_t *k) {
    uint32_t v0 = v[0], v1 = v[1], sum, i;
    sum = (tea_round == 16) ? 0xE3779B90 : 0xC6EF3720;

    uint32_t delta = 0x9e3779b9;
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];
    for (i = 0; i < tea_round; i++) {
        v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        sum -= delta;
    }

    v[0] = v0;
    v[1] = v1;
}

bool encrypt(const void *input, int input_len, DataBuffer &out) {
    if (input == NULL || input_len <= 0)
        return false;

    unsigned int rest_len = input_len % TEA_BLOCK_SIZE;
    //padding是必须带的，即便是TEA_BLOCK_SIZE的整数倍，也要加panding；
    unsigned int padding_len = TEA_BLOCK_SIZE - rest_len;

    int blocks = (input_len + padding_len) / TEA_BLOCK_SIZE;
    out.expand(blocks * TEA_BLOCK_SIZE);
    out.writeBytes((const void *) input, input_len);

    //放入padding
    for (int i = 0; i < padding_len; i++) {
        out.writeInt8(padding_len);
    }

    uint32_t key[4];
    confuse_key(key);

    uint32_t *data = (uint32_t *) out.getData();
    for (int i = 0; i < blocks; i++) {
        tea_encrypt((uint32_t *) (data + 2 * i), key);
    }

    return true;
}

bool decrypt(const void *input, int input_len, DataBuffer &out) {
    if (input == NULL || input_len < 8)
        return false;

    int blocks = input_len / 8;
    out.expand(blocks * 8);
    out.writeBytes((const void *) input, blocks * 8);

    uint32_t key[4];
    confuse_key(key);

    uint32_t *data = (uint32_t *) out.getData();
    for (int i = 0; i < blocks; i++) {
        tea_decrypt((uint32_t *) (data + 2 * i), key);
        if (i == blocks - 1) {
            //最后一个block，必定包含padding，需要把padding拿出来；
            uint8_t padding_len = ((uint8_t *) (data + 2 * i))[TEA_BLOCK_SIZE - 1];
            out.stripData(padding_len);
        }
    }

    return true;
}



