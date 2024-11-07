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
#include <fstream>
#include <filesystem>
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


namespace FileUtilities
{
    constexpr size_t readBlockSize { 1024 };

    void PrintFileContent(const std::filesystem::path &filePath)
    {
        if (std::ifstream file(filePath); file.is_open() && file.good())
        {
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;
            }
        }
    }

    std::string ReadFile(const std::filesystem::path &filePath)
    {
        if (std::ifstream file(filePath); file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            size_t fileSize = file.tellg(), bytesRead = 0;
            file.seekg(0, std::ios_base::beg);

            std::string text(fileSize, '\0');
            while ((bytesRead += file.readsome(text.data() + bytesRead, readBlockSize)) < fileSize) { }
            return text;
        }
        return {};
    }

    std::vector<uint8_t> ReadFileAsBytes(const std::filesystem::path &filePath)
    {
        if (std::fstream file(filePath , std::ios::in | std::ios::binary); file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            size_t fileSize = file.tellg(), bytesRead = 0;
            file.seekg(0, std::ios_base::beg);

            std::vector<uint8_t> data (fileSize);
            while ((bytesRead += file.readsome(reinterpret_cast<char *>(data.data() + bytesRead), readBlockSize)) < fileSize) { }
            return data;
        }
        return {};
    }

    bool ReadFile2String(const std::filesystem::path &filePath,
                         std::string& dst)
    {
        if (std::ifstream file(filePath); file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            size_t fileSize = file.tellg(), bytesRead = 0;
            file.seekg(0, std::ios_base::beg);

            dst.resize(fileSize);
            while ((bytesRead += file.readsome(dst.data() + bytesRead, readBlockSize)) < fileSize) { }
            return true;
        }
        return false;
    }

    std::size_t getFileSize(const std::filesystem::path &filePath)
    {
        if (std::ifstream file(filePath); file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            const size_t fileSize = file.tellg();
            file.seekg(0, std::ios_base::beg);
            return fileSize;
        }
        return std::string::npos;
    }

    std::size_t getFileSizeFS(const std::filesystem::path &filePath)
    {
        return std::filesystem::file_size(filePath);
    }

    int32_t WriteToFile(const std::filesystem::path& filePath,
                        const std::string& text,
                        std::ios_base::openmode mode)
    {
        if (std::ofstream file(filePath, mode); file.is_open() && file.good())
        {
            const int32_t pos = static_cast<int32_t>(file.tellp());
            file.write(text.data(), std::ssize(text));
            return static_cast<int32_t>(file.tellp()) - pos;
        }
        return -1;
    }

    int32_t WriteToFileBytes(const std::filesystem::path& filePath,
                             const std::vector<uint8_t>& data,
                             std::ios_base::openmode mode = std::ios::out | std::ios::binary)
    {
        if (std::fstream file(filePath, mode); file.is_open() && file.good())
        {
            const int32_t pos = static_cast<int32_t>(file.tellp());
            file.write(reinterpret_cast<const char *>(data.data()), std::ssize(data));
            return static_cast<int32_t>(file.tellp()) - pos;
        }
        return -1;
    }

    int32_t WriteToFile(const std::filesystem::path& filePath,
                        const std::string& text)
    {
        return WriteToFile(filePath, text, std::ios_base::trunc);
    }

    int32_t AppendToFile(const std::filesystem::path& filePath,
                         const std::string& text)
    {
        return WriteToFile(filePath, text, std::ios_base::app);
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
        int outlen = 0;

        std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx {
                EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free
        };

        if (0 == EVP_EncryptInit(ctx.get(), EVP_aes_256_cbc(), key.data(), iv.data())) {
            std::cerr << "Error: EVP_EncryptInit() failed" << std::endl;
            return;
        }
        if (0 == EVP_EncryptUpdate(ctx.get(), output.data(), &outlen, message.data(), message.size())) {
            std::cerr << "Error: EVP_EncryptUpdate() failed" << std::endl;
            return;
        }
        size_t total_out = outlen;
        if (0 == EVP_EncryptFinal(ctx.get(), output.data()+total_out, &outlen)) {
            std::cerr << "Error: EVP_EncryptUpdate() failed" << std::endl;
            return;
        }

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

        int outlen {0 };
        if (0 == EVP_DecryptInit(ctx.get(), EVP_aes_256_cbc(), key.data(), iv.data())) {
            std::cerr << "Error: EVP_EncryptInit() failed" << std::endl;
            return;
        }
        if (0 == EVP_DecryptUpdate(ctx.get(), output.data(), &outlen, message.data(), message.size())) {
            std::cerr << "Error: EVP_DecryptUpdate() failed" << std::endl;
            return;
        }
        size_t total_out = outlen;
        if (0 == EVP_DecryptFinal(ctx.get(), output.data() + outlen, &outlen)) {
            std::cerr << "Error: EVP_DecryptUpdate() failed" << std::endl;
            return;
        }

        total_out += outlen;
        output.resize(total_out);
    }


    void Test()
    {
        const std::string iv = "1234567890123456", key = "passwordpasswordpasswordpassword";
        const std::string message = "Some Crypto Text";

        const std::vector<uint8_t> ivBytes { str2Bytes(iv) };
        std::vector<uint8_t> dataEncrypted, dataDecrypted;

        Encryption::encrypt(str2Bytes(key), str2Bytes(message), ivBytes, dataEncrypted);
        Encryption::decrypt(str2Bytes(key), dataEncrypted, ivBytes, dataDecrypted);

        std::cout << bytes2Str(dataEncrypted) << std::endl;
        std::cout << bytes2Str(dataDecrypted) << std::endl;
    }
};


namespace EncryptionEx
{
    void encrypt(const std::vector<uint8_t>& key,
                 const std::vector<uint8_t>& message,
                 const std::vector<uint8_t>& iv,
                 std::vector<uint8_t>& output)
    {
        output.resize(message.size() * AES_BLOCK_SIZE);
        int outlen = 0;

        std::unique_ptr<EVP_CIPHER_CTX, decltype(&::EVP_CIPHER_CTX_free)> ctx {
                EVP_CIPHER_CTX_new(), ::EVP_CIPHER_CTX_free
        };

        if (0 == EVP_EncryptInit_ex2(ctx.get(), EVP_aes_256_cbc(), key.data(), iv.data(), nullptr)) {
            std::cerr << "Error: EVP_EncryptInit() failed" << std::endl;
            return;
        }
        if (0 == EVP_EncryptUpdate(ctx.get(), output.data(), &outlen, message.data(), message.size())) {
            std::cerr << "Error: EVP_EncryptUpdate() failed" << std::endl;
            return;
        }
        size_t total_out = outlen;
        if (0 == EVP_EncryptFinal_ex(ctx.get(), output.data()+total_out, &outlen)) {
            std::cerr << "Error: EVP_EncryptUpdate() failed" << std::endl;
            return;
        }

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


        if (0 == EVP_EncryptInit_ex2(ctx.get(), EVP_aes_256_cbc(), key.data(), iv.data(), nullptr)) {
            std::cerr << "Error: EVP_EncryptInit() failed" << std::endl;
            return;
        }

        int outlen { 0 };
        if (0 == EVP_DecryptUpdate(ctx.get(), output.data(), &outlen, message.data(), message.size())) {
            std::cerr << "Error: EVP_DecryptUpdate() failed" << std::endl;
            return;
        }
        size_t total_out = outlen;
        if (0 == EVP_EncryptFinal_ex(ctx.get(), output.data() + outlen, &outlen)) {
            std::cerr << "Error: EVP_DecryptUpdate() failed" << std::endl;
            return;
        }

        total_out += outlen;
        output.resize(total_out);
    }

    void Test()
    {
        const std::string iv = "1234567890123456", key = "some_password";
        const std::string secretData = "-----BEGIN PGP PUBLIC KEY BLOCK-----\n"
                                       "xsFNBGVDTIABEACUuJWQqyYtyZ78+ABbS1XhR5AT5FzaGFo+emWmlIdgcYuAh5Qm\n"
                                       "7JaLVQOCZEP5aKcjIAb8rboK+G5/WLIdcoTz4pBm+SXHEwfE6RB5BQjtHLYDPuoT\n"
                                       "k0OKA9wzOwu3o48NfCS1Mp94z0li4uoNvjKFxXj0FvZr07OH1FPCyzz1F2F/7yXX\n"
                                       "wdeTWWbFwU67NxqXvuY1PscxU/CqNYiNFy2+tpsyBtMsCBggCfb/f8Ci6+mYnK8j\n"
                                       "HgJEA3jOZ6p5Fhxel7rPFqfrUAATmlzLxtmW2Q0KTFIrhNECmTz5iM5DetBleJX9\n"
                                       "Nwe5QXXujWXuF3CR64Dlynu4JUsaO6FEhrmoakDksSVhSSv4V0uEbGKQ3ezs5FIb\n"
                                       "PL2KifUgnfC6exFHjxLG8ZTf0FoEID+tag6AlzTuoYZgOxfYFUVbyW7kSkYcUaJ6\n"
                                       "VGVDk71lmlMenTyHiGbWpy5FH0gRnGc0bkHZ49Ak1Gs/kOe96YuQ+BA0k/7NnQYT\n"
                                       "I1JJyxNPFf6EF8XZ5PQ5r2YJXrME1UHyZKCmviyItJZIeWq0ppAOp6r8vgGUBL3R\n"
                                       "xNzwfMT0fXKhTMmqi3DPS22+0JWRvs1CiH+BQeDCAVM9f/3Ny11pFcnteeUrMCa9\n"
                                       "v4rxPY1StZINzDlpGvQH1ztXF5YZ4AuINZRH3HMhQK7h8kIpOB6nRH9gewARAQAB\n"
                                       "zT1BYnJhaGFtIExpbmNvbG4gKEhvbmVzdCBBYmUpIDxhYnJhaGFtLmxpbmNvbG5A\n"
                                       "d2hpdGVob3VzZS5nb3Y+wsGKBBMBCAA0BQJlQ0yBAhsOBAsJCAcFFQgJCgsFFgID\n"
                                       "AQACHgEWIQS0ICq4W8XKR91psjInjzsSygStCwAKCRAnjzsSygStC55pEACUPhsu\n"
                                       "2oJP5JNhYCFtLN16c+80+S4tt2pXGXS9vWZShlwIo2n9v+khy0EgB76RPNjFVdzO\n"
                                       "fEISZ4r5I2CF7w5zb6iKTwBIsYJ7siXbsrm+vBchUr+ONtsuXNvT/ZQULkEmoeQO\n"
                                       "GpvFtqUgfQOTsxLGk5jNjuS4HWBsVdCPYLAIcIfifPqxTlL6CZ78P71vzSUO69Gq\n"
                                       "iFAihh97wChAb0qxEqlPterYHOldx3GfLaBstLGP2rUyiSLCPAl+KJdmEKXvx3MP\n"
                                       "LYJhOLb3bs+rireXCjRoCDPcnKhszJndhvI1ZfoQ9FhUNQ10sHXDRYLKAtpqwc6W\n"
                                       "KaIHP16BtvlwhsF/GlwXiTPS1anPEH+o5mCEqlHwXyD8YZnxRCuA9dg+sgkbT2yU\n"
                                       "d9dJDFpBPlXPJcu0JVHiCGscc1jv/aoXiKpEwuF9qjFV8n3MGYEkEyzj6onuO+Aj\n"
                                       "cdf+gvk84kJ//xFc0rJgId3s45Lv6SLlal+Og95nwn9JOS1xTkPYDJztT33AcSOP\n"
                                       "VKCxdX1p2AfhjGkG2lHr9Wp0cXE8b4TRgBOIDt85zsb+nnQj77XTzB3CTu7lRL5f\n"
                                       "VfN+hui9DALdM+5j+yeToeLtU6CpCTNcgwDmEP8paiGtObad8aBySrVLvfgDDJm8\n"
                                       "YN3u2sNMN8wzH1i3s482LALcNBZYDe+VpHZu/g==\n"
                                       "=/kav\n"
                                       "-----END PGP PUBLIC KEY BLOCK-----";
        const std::string payload =  secretData;

        const std::vector<uint8_t> ivBytes { str2Bytes(iv) };
        std::vector<uint8_t> dataEncrypted, dataDecrypted;

        Encryption::encrypt(str2Bytes(key), str2Bytes(payload), ivBytes, dataEncrypted);
        Encryption::decrypt(str2Bytes(key), dataEncrypted, ivBytes, dataDecrypted);

        std::string result = bytes2Str(dataDecrypted);
        result.erase(secretData.size());
        std::cout << result << std::endl;
    }


    /**

    openssl enc -aes-256-cbc -in api_key.txt -out api_key.out -pass pass:some_password
    openssl aes-256-cbc -d -in api_key.out -out api_key_dest.txt -pass pass:some_password

    openssl enc -aes-256-cbc -pbkdf2 -in api_key.txt -out api_key.out -pass pass:some_password
    openssl aes-256-cbc -pbkdf2 -d -in api_key.out -out api_key_dest.txt -pass pass:some_password

    openssl enc -aes-256-cbc -in api_key.txt -out api_key.out -iv 1234567890123456 -pbkdf2
    openssl aes-256-cbc -d -in api_key.out -out api_key_dest.txt -iv 1234567890123456 -pbkdf2

    openssl enc -aes-256-cbc -pbkdf2 -in api_key.txt -out api_key.out -iv 1234567890123456 -pass pass:some_password
    openssl aes-256-cbc -pbkdf2 -d -in api_key.out -out api_key_dest.txt -iv 1234567890123456 -pass pass:some_password

    **/

    void Encrypt_Decrypt_ViaFile()
    {
        const std::string iv = "1234567890123456", key = "some_password";
        const std::filesystem::path fileInput { R"(/home/andtokm/DiskS/Temp/SSL/api_key.txt)" };
        // const std::filesystem::path fileEncrypted { R"(/home/andtokm/DiskS/Temp/SSL/api_key.out)" };

        const std::string dataToEncrypt = FileUtilities::ReadFile(fileInput);
        const std::vector<uint8_t> ivBytes { str2Bytes(iv) };

        std::vector<uint8_t> dataEncrypted;
        Encryption::encrypt(str2Bytes(key), str2Bytes(dataToEncrypt), ivBytes, dataEncrypted);

        // FileUtilities::WriteToFileBytes(fileEncrypted, dataEncrypted);

        const std::vector<uint8_t> encryptedData = FileUtilities::ReadFileAsBytes(fileInput);

        std::vector<uint8_t> dataDecrypted = dataEncrypted;
        Encryption::decrypt(str2Bytes(key), dataEncrypted, ivBytes, dataDecrypted);
    }


    std::string ReadFileSlow(const std::filesystem::path &filePath)
    {
        std::string text;
        if (true)
        {
            //file.seekg(0, std::ios_base::end);
            //size_t fileSize = file.tellg(), bytesRead = 0;
            //file.seekg(0, std::ios_base::beg);

            text.assign("qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq_111111111111111111111111111_123");
        }
        return text;
    }


    void Encrypt_Decrypt_ViaFileEx()
    {

        const std::filesystem::path fileInput { R"(/home/andtokm/DiskS/Temp/SSL/api_key.txt)" };
        const std::filesystem::path fileEncrypted { R"(/home/andtokm/DiskS/Temp/SSL/api_key.out)" };
        ReadFileSlow(fileInput);

        const std::string iv = "1234567890123456", key = "some_password";
        const std::string dataToEncrypt = "qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq_111111111111111111111111111_123";

        // FileUtilities::ReadFile(fileInput);

        // std::cout << (dataToEncrypt == dataToEncrypt1) << std::endl;

        const std::vector<uint8_t> ivBytes { str2Bytes(iv) };
        std::vector<uint8_t> dataEncrypted, dataDecrypted;

        Encryption::encrypt(str2Bytes(key), str2Bytes(dataToEncrypt), ivBytes, dataEncrypted);
        Encryption::decrypt(str2Bytes(key), dataEncrypted, ivBytes, dataDecrypted);

        const std::string result = bytes2Str(dataDecrypted);
        std::cout << result << std::endl;
    }
};

// https://helpmanual.io/man3/EVP_CIPHER_CTX_new-ssl/

void Encryption::TestAll()
{
    // Encryption::Test();
    // EncryptionEx::Test();

    // EncryptionEx::Encrypt_Decrypt_ViaFile();
    EncryptionEx::Encrypt_Decrypt_ViaFileEx();
};