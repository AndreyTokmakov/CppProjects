/**============================================================================
Name        : Cryptography.cpp
Created on  : 26.11.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Cryptography tests and experiments
============================================================================**/

#include <iostream>
#include <vector>
#include <string_view>
#include <iomanip>

#include "experiments/sha256_Original.h"
#include "experiments/Sha256.h"

#include "rsa.h"
#include "base64.h"
#include "osrng.h"
#include "modes.h"



namespace Sha256Tests
{

    void Tests() {
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
        std::string cipher;

        auto aes = CryptoPP::AES::Encryption(key.data(), key.size());
        auto aes_cbc = CryptoPP::CBC_Mode_ExternalCipher::Encryption(aes, iv.data());

        CryptoPP::StringSource ss(
            input,
            true,
            new CryptoPP::StreamTransformationFilter(aes_cbc,new CryptoPP::Base64Encoder(new CryptoPP::StringSink(cipher)))
        );

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

    void Tests()
    {
        constexpr size_t AES_KEY_SIZE = 256 / 8; //AES-256
        constexpr std::string_view input {"This is a secret message."};

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
        std::cout << "plain_text: " << std::quoted(plain_text)<< std::endl;

        if (plain_text != input) {
            std::cerr << "Error: plain text doesn't match the input" << std::endl;
        }
    }
}


/// CryptoCPP: https://github.com/weidai11/cryptopp

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Sha256Tests::Tests();
    LibCryptoCpp::Tests();

    return EXIT_SUCCESS;
}

