//============================================================================
// Name        : Sha256.cpp
// Created on  : 08.03.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Sha256 encoding
//============================================================================

#include <cstring>
#include "Sha256.h"

#define SHA2_SHFR(x, n)    (x >> n)
#define SHA2_ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))
#define SHA2_CH(x, y, z)  ((x & y) ^ (~x & z))
#define SHA2_MAJ(x, y, z) ((x & y) ^ (x & z) ^ (y & z))
#define SHA256_F1(x) (SHA2_ROTR(x,  2) ^ SHA2_ROTR(x, 13) ^ SHA2_ROTR(x, 22))
#define SHA256_F2(x) (SHA2_ROTR(x,  6) ^ SHA2_ROTR(x, 11) ^ SHA2_ROTR(x, 25))
#define SHA256_F3(x) (SHA2_ROTR(x,  7) ^ SHA2_ROTR(x, 18) ^ SHA2_SHFR(x,  3))
#define SHA256_F4(x) (SHA2_ROTR(x, 17) ^ SHA2_ROTR(x, 19) ^ SHA2_SHFR(x, 10))
#define SHA2_UNPACK32(x, str)                 \
{                                             \
    *((str) + 3) = (uint8_t) ((x)      );     \
    *((str) + 2) = (uint8_t) ((x) >>  8);     \
    *((str) + 1) = (uint8_t) ((x) >> 16);     \
    *((str) + 0) = (uint8_t) ((x) >> 24);     \
}
#define SHA2_PACK32(str, x)                   \
{                                             \
    *(x) =   ((uint32_t) *((str) + 3)      )  \
           | ((uint32_t) *((str) + 2) <<  8)  \
           | ((uint32_t) *((str) + 1) << 16)  \
           | ((uint32_t) *((str) + 0) << 24); \
}

void Sha256::transform(const uint8_t *message,
                       uint32_t block_nb)
{
    uint32_t w[64] {}, wv[8] {};
    uint32_t t1 = 0, t2 = 0;
    const unsigned char *sub_block;
    for (int i = 0, j = 0; i < (int) block_nb; i++) {
        sub_block = message + (i << 6);
        for (j = 0; j < 16; j++) {
            SHA2_PACK32(&sub_block[j << 2], &w[j]);
        }
        for (j = 16; j < 64; j++) {
            w[j] =  SHA256_F4(w[j -  2]) + w[j -  7] + SHA256_F3(w[j - 15]) + w[j - 16];
        }
        for (j = 0; j < 8; j++) {
            wv[j] = m_h[j];
        }
        for (j = 0; j < 64; j++) {
            t1 = wv[7] + SHA256_F2(wv[4]) + SHA2_CH(wv[4], wv[5], wv[6]) + sha256_k[j] + w[j];
            t2 = SHA256_F1(wv[0]) + SHA2_MAJ(wv[0], wv[1], wv[2]);
            wv[7] = wv[6];
            wv[6] = wv[5];
            wv[5] = wv[4];
            wv[4] = wv[3] + t1;
            wv[3] = wv[2];
            wv[2] = wv[1];
            wv[1] = wv[0];
            wv[0] = t1 + t2;
        }
        for (j = 0; j < 8; j++) {
            m_h[j] += wv[j];
        }
    }
}

void Sha256::update(std::string_view input)
{
    const uint32_t tmp_len = SHA224_256_BLOCK_SIZE - m_len;
    uint32_t rem_len = input.length() < tmp_len ? input.length() : tmp_len;
    memcpy(&m_block[m_len], input.data(), rem_len);
    if (m_len + input.length() < SHA224_256_BLOCK_SIZE) {
        m_len += input.length();
        return;
    }

    const uint32_t new_len = input.length() - rem_len;
    const uint32_t block_nb = new_len / SHA224_256_BLOCK_SIZE;
    const uint8_t* shifted_message = reinterpret_cast<const uint8_t*>(input.data()) + rem_len;

    transform(m_block, 1);
    transform(shifted_message, block_nb);
    rem_len = new_len % SHA224_256_BLOCK_SIZE;
    memcpy(m_block, &shifted_message[block_nb << 6], rem_len);
    m_len = rem_len;
    m_tot_len += (block_nb + 1) << 6;
}

void Sha256::final(uint8_t *digest)
{
    const uint32_t block_nb = (1 + ((SHA224_256_BLOCK_SIZE - 9) < (m_len % SHA224_256_BLOCK_SIZE)));
    const uint32_t len_b = (m_tot_len + m_len) << 3;
    const uint32_t pm_len = block_nb << 6;

    memset(m_block + m_len, 0, pm_len - m_len);
    m_block[m_len] = 0x80;
    SHA2_UNPACK32(len_b, m_block + pm_len - 4);
    transform(m_block, block_nb);
    for (int i = 0 ; i < 8; i++) {
        SHA2_UNPACK32(m_h[i], &digest[i << 2]);
    }
}

std::string toSha256(std::string_view input)
{
    std::array<u_int8_t, Sha256::DIGEST_SIZE> digest {0};

    Sha256 ctx = Sha256();
    ctx.update(input);
    ctx.final(digest.data());

    std::string buffer(2 * Sha256::DIGEST_SIZE,'\0');
    for (size_t i = 0; i < Sha256::DIGEST_SIZE; i++)
        sprintf(buffer.data() + i*2, "%02x", digest[i]);
    return buffer;
}
