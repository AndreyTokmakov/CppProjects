/**============================================================================
Name        : Cryptography.cpp
Created on  : 26.11.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Cryptography tests and experiments
============================================================================**/

#include <iostream>
#include <array>
#include <vector>
#include <string_view>
#include <iomanip>
#include <format>
#include <fstream>
#include <filesystem>

#include "experiments/sha256_Original.h"
#include "experiments/Sha256.h"
#include "crc/CRC.h"
#include "sha/Sha1.h"
#include "SecureFIleBlockStorage/SecureFIleBlockStorage.h"
#include "utils/FileUtilities.h"

#include "rsa.h"
#include "hex.h"
#include "crc.h"
#include "base64.h"
#include "osrng.h"
#include "modes.h"


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


namespace Sha256Tests
{
    void Tests()
    {
        std::string input = "1232323";
        std::string expected = "1092ab2771a1b8d2d88fad71b09d03875c6df0c429ab02faf522524d44cbe4a8";

        std::cout << std::boolalpha << (expected == sha256_Orig(input)) << std::endl;
        std::cout << std::boolalpha << (expected == toSha256(input)) << std::endl;


        std::cout << "=======================================================================" << std::endl;
        std::cout << "'" << sha256_Orig(input) << "'\n";
        std::cout << "'" << sha256_Orig(input).length() << "'\n";
        std::cout << "=======================================================================" << std::endl;
        std::cout << "'" << toSha256(input) << "'\n";
        std::cout << "'" << toSha256(input).length() << "'\n";
    }
}



namespace LibCryptoCpp
{
    std::string encrypt(const std::string& input,
                       const std::vector<uint8_t>& key,
                       const std::vector<uint8_t>& iv)
    {
        CryptoPP::AES::Encryption aes { CryptoPP::AES::Encryption(key.data(), key.size()) };
        CryptoPP::CBC_Mode_ExternalCipher::Encryption aesCbc { CryptoPP::CBC_Mode_ExternalCipher::Encryption(aes, iv.data()) };

        /* std::unique_ptr<CryptoPP::Base64Encoder> encoder {
                std::make_unique<CryptoPP::Base64Encoder>(new CryptoPP::StringSink(cipher))
        };

        std::unique_ptr<CryptoPP::StreamTransformationFilter> transformFiler {
                std::make_unique<CryptoPP::StreamTransformationFilter>(aes_cbc, encoder.get())
        }; */

        std::string cipher;
        CryptoPP::Base64Encoder* encoder = new CryptoPP::Base64Encoder(new CryptoPP::StringSink(cipher));
        CryptoPP::StreamTransformationFilter* transformFiler = new CryptoPP::StreamTransformationFilter(aesCbc, encoder);
        CryptoPP::StringSource ss(input, true, transformFiler);

        // std::vector<uint8_t> data;
        // CryptoPP::VectorSource vs(data, true, transformFiler);

        return cipher;
    }

    std::string decrypt(const std::string& cipherText,
                        const std::vector<uint8_t>& key,
                        const std::vector<uint8_t>& iv)
    {
        std::string plain_text;

        CryptoPP::AES::Decryption aes = CryptoPP::AES::Decryption(key.data(), key.size());
        CryptoPP::CBC_Mode_ExternalCipher::Decryption aes_cbc = CryptoPP::CBC_Mode_ExternalCipher::Decryption(aes, iv.data());

        CryptoPP::StringSource ss(cipherText,
            true,
            new CryptoPP::Base64Decoder(new CryptoPP::StreamTransformationFilter(aes_cbc,new CryptoPP::StringSink(plain_text)))
        );

        return plain_text;
    }

    void Tests_RandomPass()
    {
        constexpr size_t AES_KEY_SIZE = 256 / 8; //AES-256
        constexpr std::string_view input{"This is a secret message."};

        std::vector<uint8_t> key(AES_KEY_SIZE);
        std::vector<uint8_t> iv(CryptoPP::AES::BLOCKSIZE);

        std::cout << "key.size() : " << key.size() << std::endl;
        std::cout << "iv.size()  : " << iv.size() << std::endl;

        CryptoPP::BlockingRng rand;
        rand.GenerateBlock(key.data(), key.size());
        rand.GenerateBlock(iv.data(), iv.size());

        const std::string cipher = encrypt(input.data(), key, iv);
        const std::string plain_text = decrypt(cipher, key, iv);

        std::cout << "cipher    : " << std::quoted(cipher) << std::endl;
        std::cout << "plain_text: " << std::quoted(plain_text) << std::endl;

        if (plain_text != input) {
            std::cerr << "Error: plain text doesn't match the input" << std::endl;
        }
    }

    void Tests()
    {
        constexpr std::string_view input{"This is a secret message."};
        const std::vector<uint8_t> key {
            0x8c, 0xd7, 0x6f, 0xf1, 0x32, 0xaa, 0x44, 0xb5, 0x44,0x71, 0x90, 0xf3, 0x4f, 0x52, 0xfd, 0x88,
            0x3c, 0x4a,0xe3, 0x0, 0x42, 0xd9, 0x93, 0x40, 0xf5, 0x96, 0xa2, 0x30, 0x70, 0xf3, 0x3c, 0x78
        };
        const std::vector<uint8_t> iv {
            0x9d, 0x85, 0xc7, 0x69, 0x7a, 0xec, 0xd4, 0x93, 0xa3, 0x4b, 0x1, 0x87, 0xb3, 0xf0, 0x46, 0x88
        };

        const std::string encryptedData = encrypt(input.data(), key, iv);
        const std::filesystem::path dataFile { R"(../../Security/data/data.txt)"};
        FileUtilities::WriteToFile(dataFile, encryptedData);

        const std::string dataFromFile = FileUtilities::ReadFile(dataFile);
        const std::string plain_text = decrypt(dataFromFile, key, iv);

        std::cout << "cipher    : " << std::quoted(encryptedData) << std::endl;
        std::cout << "plain_text: " << std::quoted(plain_text) << std::endl;

        if (plain_text != input) {
            std::cerr << "Error: plain text doesn't match the input" << std::endl;
        }
    }
}

namespace SecurityBug
{
    void f1()
    {
        /*
        std::array<char, 8> secret {'p','a','s','s','w','o','r','d'};
        std::cout << std::string(secret.data(), secret.size()) << std::endl;
        */

        char secret[] = {'p','a','s','s','w','o','r','d'};
        std::cout << secret << std::endl;
    }

    void f2()
    {
        /*
        std::array<char, 8> secret;
        std::cout << std::string(secret.data(), secret.size()) << std::endl;
        */

        char secret[8];
        std::cout << secret << std::endl;
    }

    void test()
    {
        f1();
        f2();
    }
}


/// CryptoCPP: https://github.com/weidai11/cryptopp

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Sha256Tests::Tests();
    // LibCryptoCpp::Tests_RandomPass();
    // LibCryptoCpp::Tests();
    // CRC::TestAll();
    // Sha1::TestAll();

    // SecureFIleBlockStorage::TestAll();

    /// AES-GCM ????


    SecurityBug::test();

    return EXIT_SUCCESS;
}

