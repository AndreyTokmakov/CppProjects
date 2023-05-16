/**============================================================================
Name        : PcapAnalyzer.cpp
Created on  : 16.05.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : PcapAnalyzer
============================================================================**/

#include "PcapAnalyzer.h"

#include <iostream>
#include <fstream>
#include <array>
#include <vector>

#include <arpa/inet.h>

namespace PcapAnalyzer::WiFi
{
    struct RadiotapHeader
    {
        uint8_t it_version;
        uint8_t it_pad;
        uint16_t it_len;      /* entire length */
        uint32_t it_present;  /* fields present */
    } __attribute__((packed, aligned(1)));


    std::string bin2hex(const std::vector<char> buffer)
    {
        std::string res;
        constexpr std::array<char, 16> hex { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
        for(const char ch : buffer)
        {
            const unsigned char c = static_cast<unsigned char>(ch);
            res += hex[c >> 4];
            res += hex[c & 0xf];
            res += ' ';
        }

        return res;
    }

    void ReadAndParseFile()
    {
        constexpr std::string_view path { R"(/home/andtokm/DiskS/Temp/Dumps/auth_packetp.pcapng)"};

        // std::vector<uint8_t> buffer {};
        std::vector<char> buffer {};
        if (std::ifstream file(path.data(), std::ios::ate | std::ios::binary); file.is_open() && file.good()) {
            const size_t length = static_cast<size_t>(file.tellg());
            buffer.resize(length);

            file.seekg(0, std::ios_base::beg);
            file.read(reinterpret_cast<char*>(buffer.data()), length);
            // const auto bytesRead = static_cast<size_t>(file.gcount());
        }

        // TODO: 18 * 16 ?? Why we need this offset
        buffer.erase(buffer.begin(), buffer.begin() + 18 * 16);
        // std::cout << bin2hex(buffer) << std::endl;


        const RadiotapHeader* radioTap = (RadiotapHeader*)(buffer.data());
        std::cout << "Length  : " << radioTap->it_len << std::endl;
        std::cout << "Pad     : " << htons(radioTap->it_pad) << std::endl;
        std::cout << "Version : " << htons(radioTap->it_version) << std::endl;



        /*
        if (std::ifstream file(path.data(), std::ios::binary); file.is_open() && file.good())
        {
            ptrdiff_t bytesRead {0}, bytesTotal {0};
            std::string buffer(blockSize, '0');
            while (0 < (bytesRead = file.read(buffer.data(), blockSize).gcount())) {
                bytesTotal += bytesRead;
            }
            std::cout << "bytesTotal = " << bytesTotal << std::endl;
        }
         */
    }
};

void PcapAnalyzer::TestAll()
{
    WiFi::ReadAndParseFile();
}