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
#include "crypto_cpp/CryptoCpp.h"


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
    CryptoCpp::TestAll();

    // SecureFIleBlockStorage::TestAll();

    /// AES-GCM ????


    // SecurityBug::test();

    return EXIT_SUCCESS;
}

