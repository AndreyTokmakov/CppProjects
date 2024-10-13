/**============================================================================
Name        : Encryption.cpp
Created on  : 13.10.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Encryption.cpp
============================================================================**/

#include "Encryption.h"

#include <iostream>
#include <utility>
#include <vector>
#include <memory>

#include <openssl/pem.h>
#include <openssl/conf.h>
#include <openssl/x509v3.h>
#include <openssl/engine.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/dh.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/stack.h>
#include <openssl/pkcs12.h>

#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/evperr.h>
#include <openssl/aes.h>
#include <openssl/crypto.h>


namespace
{
    static std::vector<uint8_t> str2Bytes(const std::string& message)
    {
        std::vector<uint8_t> out(message.size());
        for (size_t n = 0; n < message.size(); n++) {
            out[n] = message[n];
        }
        return out;
    }

    static std::string bytes2Str(const std::vector<uint8_t>& bytes)
    {
        return std::string(bytes.begin(), bytes.end());
    }
}

namespace Encryption
{


    void encrypt(const std::vector<uint8_t>& key,
                 const std::vector<uint8_t>& message,
                 const std::vector<uint8_t>& iv,
                 std::vector<uint8_t>& output)
    {
        output.resize(message.size() * AES_BLOCK_SIZE);
        const size_t messageLen = message.size();
        int outlen = 0;

        std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx {
                EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free
        };

        int res = EVP_EncryptInit(ctx.get(), EVP_aes_256_cbc(), key.data(), iv.data());
        res = EVP_EncryptUpdate(ctx.get(), output.data(), &outlen, message.data(), messageLen);
        size_t total_out = outlen;
        res = EVP_EncryptFinal(ctx.get(), output.data()+total_out, &outlen);
        total_out += outlen;

        output.resize(total_out);
    }

    void decrypt(const std::vector<uint8_t>& key,
                 const std::vector<uint8_t>& message,
                 const std::vector<uint8_t>& iv,
                 std::vector<uint8_t>& output)
    {
        output.resize(message.size() * 3);
        std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx {
                EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free
        };

        const size_t messageLen = message.size();
        int res { 0 }, outlen {0 };
        res = EVP_DecryptInit(ctx.get(), EVP_aes_256_cbc(), key.data(), iv.data());
        res = EVP_DecryptUpdate(ctx.get(), output.data(), &outlen, message.data(), messageLen);
        size_t total_out = outlen;
        res = EVP_DecryptFinal(ctx.get(), output.data() + outlen, &outlen);
        total_out += outlen;

        output.resize(total_out);
    }
};

// https://helpmanual.io/man3/EVP_CIPHER_CTX_new-ssl/

void Encryption::TestAll()
{
    const std::string iv = "1234567890123456", key = "passwordpasswordpasswordpassword";
    const std::string message = "Some secret Message";

    const std::vector<uint8_t> ivBytes { str2Bytes(iv) };
    std::vector<uint8_t> dataEncrypted, dataDecrypted;

    Encryption::encrypt(str2Bytes(key), str2Bytes(message), ivBytes, dataEncrypted);
    std::cout << bytes2Str(dataEncrypted) << std::endl;

    Encryption::decrypt(str2Bytes(key), dataEncrypted, ivBytes, dataDecrypted);
    std::cout << bytes2Str(dataDecrypted) << std::endl;
};