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


namespace PcapAnalyzer
{
    struct GlobalHeader final
    {
        uint32_t magicNumber {};    /* magic number */
        uint16_t versionMajor {};   /* major version number */
        uint16_t versionMinor {};   /* minor version number */
        uint32_t thiszone {};       /* GMT to local correction */
        uint32_t sigfigs {};        /* accuracy of timestamps */
        uint32_t snaplen {};        /* max length of captured packets, in octets */
        uint32_t network {};        /* data link type */
    };

    struct PacketHeader final
    {
        uint32_t ts_sec;         /* timestamp seconds */
        uint32_t ts_usec;        /* timestamp microseconds */
        uint32_t incl_len;       /* number of octets of packet saved in file */
        uint32_t orig_len;       /* actual length of packet */
    };

    [[nodiscard]]
    std::vector<uint8_t> readPcapFile(std::string_view path)
    {
        std::vector<uint8_t> buffer {};
        if (std::ifstream file(path.data(), std::ios::ate | std::ios::binary); file.is_open() && file.good()) {
            const size_t length = static_cast<size_t>(file.tellg());
            buffer.resize(length);

            file.seekg(0, std::ios_base::beg);
            file.read(reinterpret_cast<char*>(buffer.data()), length);
            // const auto bytesRead = static_cast<size_t>(file.gcount());
        }

        return buffer;
    }


    void testGlobalHeader()
    {
        constexpr std::string_view path { R"(/home/andtokm/DiskS/Temp/Dumps/auth_packetp.pcapng)"};
        const std::vector<uint8_t> buffer { readPcapFile(path) };

        std::cout << "GlobalHeader size = " << sizeof(GlobalHeader) << std::endl;
        std::cout << "PacketHeader size = " << sizeof(PacketHeader) << std::endl;

        const GlobalHeader* globalHeader = (GlobalHeader*)(buffer.data());
        std::cout << "magicNumber      : " << htons(globalHeader->magicNumber) << std::endl;
        std::cout << "versionMajor     : " << globalHeader->versionMajor << std::endl;
        std::cout << "versionMinor     : " << globalHeader->versionMinor << std::endl;
        std::cout << "snaplen          : " << htons(globalHeader->snaplen) << std::endl;
        std::cout << "network          : " << htons(globalHeader->network) << std::endl;


        const PacketHeader* pktHeader = (PacketHeader*)(buffer.data() + sizeof(GlobalHeader));
        std::cout << "\nincl_len      : " << htons(pktHeader->incl_len) << std::endl;
        std::cout << "orig_len      : " << htons(pktHeader->orig_len) << std::endl;

    }
}

namespace PcapAnalyzer::WiFi
{
    using namespace std::string_view_literals;

    constexpr std::array<std::pair<std::string_view, uint16_t>, 32> presentFlagsBits {{
        {"TSFT"sv, 0},
        {"Flags"sv, 1},
        {"Rate"sv, 2},
        {"Channel"sv, 3},
        {"FHSS"sv, 4},
        {"Antenna signal"sv, 5},
        {"Antenna noise"sv, 6},
        {"Lock quality"sv, 7},
        {"TX attenuation"sv, 8},
        {"dB TX attenuation"sv, 9},
        {"dBm TX power"sv, 10},
        {"Antenna"sv, 11},
        {"dB antenna signal"sv, 12},
        {"dB antenna noise"sv, 13},
        {"RX flags"sv, 14},
        {"TX flags"sv, 15},
        {"______"sv, 16},
        {"______"sv, 17},
        {"______"sv, 18},
        {"MCS"sv, 19},
        {"A-MPDU status"sv, 20},
        {"VHT"sv, 21},
        {"timestamp"sv, 22},
        {"HE"sv, 23},
        {"HE-MU"sv, 24},
        {"HE-MU-other-user"sv, 25},
        {"0-length-PSDU"sv, 26},
        {"L-SIG"sv, 27},
        {"TLV fields in radiotap"sv, 28},
        {"Radiotap Namespace"sv, 29},
        {"Vendor Namespace"sv, 30},
        {"Unknown"sv, 31}
     }};


    struct RadiotapHeader
    {
        uint8_t  version;
        uint8_t  pad;
        uint16_t length;        /* entire length */
        uint32_t presentFlags;  /* fields present */
    } __attribute__((packed, aligned(1)));


    bool isBitSet(const uint32_t value,
                  const uint16_t bit)
    {
        return value & (1u << bit) ;
    }

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

        std::vector<uint8_t> buffer { readPcapFile(path) };

        // TODO: 18 * 16 ?? Why we need this offset
        buffer.erase(buffer.begin(), buffer.begin() + 18 * 16);
        // std::cout << bin2hex(buffer) << std::endl;


        const RadiotapHeader* radioTap = (RadiotapHeader*)(buffer.data());
        std::cout << "Length  : " << radioTap->length << std::endl;
        std::cout << "Pad     : " << htons(radioTap->pad) << std::endl;
        std::cout << "Version : " << htons(radioTap->version) << std::endl;

        for (const auto& [name, bit]: presentFlagsBits)
        {
            std::cout << '\t' << name << "    " << std::boolalpha
                      << isBitSet(radioTap->presentFlags, bit) << std::endl;
        }


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
    testGlobalHeader();

    // WiFi::ReadAndParseFile();
}