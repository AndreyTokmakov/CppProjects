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

#include "../Utilities/Utilities.h"


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

    // For PcapNG
    struct SectionHeaderBlock final
    {
        uint32_t blockType {};
        uint32_t blockTotalLength {};
        uint32_t byteOrderMagic {};
        uint16_t versionMajor {};   /* major version number */
        uint16_t versionMinor {};   /* minor version number */
        uint64_t sectionLength {};
        // ????
        uint64_t options {};
    };

    enum class LinkType
    {
        /**	BSD loopback encapsulation **/
        Null = 0,

        /** IEEE 802.3 Ethernet **/
        Ethernet = 1,

        /** AX.25 packet **/
        Ax25 = 3,

        /** IEEE 802.5 Token Ring **/
        IEEE_802_5_TokenRing = 6,

        /** IEEE 802.11 wireless LAN **/
        IEEE_802_11_Wireless = 105,

        /** Radiotap - followed by an 802.11 header **/
        RadioTap = 127
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
}

namespace PcapAnalyzer
{

    void printGlobalHeader(const uint8_t* packetData)
    {
        const GlobalHeader* globalHeader { reinterpret_cast<const GlobalHeader*>(packetData) };

        std::cout << "GlobalHeader (size: " << sizeof(GlobalHeader) << ")\n";
        std::cout << "\tmagicNumber  : " << globalHeader->magicNumber << std::endl;
        std::cout << "\tversionMajor : " << globalHeader->versionMajor << std::endl;
        std::cout << "\tversionMinor : " << globalHeader->versionMinor << std::endl;
        std::cout << "\tsnaplen      : " << globalHeader->snaplen << std::endl;
        std::cout << "\tnetwork      : " << globalHeader->network << std::endl;
    }

    void printPacketHeader(const uint8_t* ptr)
    {
        const PacketHeader* pktHeader { reinterpret_cast<const PacketHeader*>(ptr) };

        std::cout << "PacketHeader (size: " << sizeof(PacketHeader) << ")\n";
        std::cout << "\tincl_len : " << pktHeader->incl_len << std::endl;
        std::cout << "\torig_len : " << pktHeader->orig_len << std::endl;
    }

    void handlePacket([[maybe_unused]] const uint8_t* packetBytes,
                      const PacketHeader& pktHeader)
    {
        std::cout << "Bytes captured: " << pktHeader.incl_len << std::endl;
    }

    void AnalyzePcapTest()
    {
        constexpr std::string_view path { R"(../../Networking/PcapAnalyzer/data/Xiaomi_WiFi_Authentiocation.pcap)"};
        const std::vector<uint8_t> buffer { readPcapFile(path) };
        const size_t dataBlockSize { buffer.size() - sizeof(GlobalHeader)};

        const uint8_t* const packetBlockPtr { buffer.data() + sizeof(GlobalHeader) };
        for (size_t nextBlockPos = 0; dataBlockSize > nextBlockPos; )
        {
            const PacketHeader *pktHeader { reinterpret_cast<const PacketHeader*>(packetBlockPtr + nextBlockPos) };
            handlePacket(packetBlockPtr + nextBlockPos + sizeof(PacketHeader), *pktHeader);
            nextBlockPos += sizeof(PacketHeader) + pktHeader->incl_len;
        }
    }

    void AnalyzeHeader()
    {

    }
}



namespace PcapAnalyzer::WiFi
{
    using namespace std::string_view_literals;

    constexpr std::string_view beacon0 { R"(../../Networking/PcapAnalyzer/data/beacon0.pcap)"};
    constexpr std::string_view beacon1 { R"(../../Networking/PcapAnalyzer/data/beacon1.cap)"};
    constexpr std::string_view beacon2 { R"(../../Networking/PcapAnalyzer/data/beacon2.cap)"};

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

    struct WiFiMACHeader
    {
        uint16_t control { 0 };
        uint16_t duration { 0 };
        uint8_t  da [6] {};
        uint8_t  sa [6] {};
        uint8_t  bss [6] {};
        uint16_t sec { 0 };
    } __attribute__((packed, aligned(1)));

    struct BeaconFixedParams
    {
        // A value representing the time on the access point, which is the number of microseconds the AP has been active.
        // When timestamp reach its max (2^64 microsecond or ~580,000 years) it will reset to 0.
        // This field contain in Beacon Frame & Probe Response frame.
        uint64_t timestamp {};

        // Beacon Interval field represent the number of time units (TU) between  target beacon transmission times (TBTT).
        // Default value is 100TU (102.4 milliseconds)
        uint16_t beaconInterval {};

        // This field contains number of subfields that are used to indicate requested or advertised optional capabilities.
        uint16_t capabilityInfo {};
    } __attribute__((packed, aligned(1)));

    struct TagParams
    {
        uint8_t number {};
        uint8_t length {};
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
        constexpr std::string_view path { R"(../../Networking/PcapAnalyzer/data/auth_packet.pcap)"};
        const std::vector<uint8_t> buffer { readPcapFile(path) };

        // const GlobalHeader* globalHeader = (GlobalHeader*)(buffer.data());
        // const PacketHeader* pktHeader = (PacketHeader*)(buffer.data() + sizeof(GlobalHeader));
        const uint8_t* packetData = buffer.data() + sizeof(PacketHeader) + sizeof(GlobalHeader);

        const RadiotapHeader* radioTap = (RadiotapHeader*)(packetData);
        std::cout << "Length  : " << radioTap->length << std::endl;
        std::cout << "Pad     : " << htons(radioTap->pad) << std::endl;
        std::cout << "Version : " << htons(radioTap->version) << std::endl;

        for (const auto& [name, bit]: presentFlagsBits)
        {
            std::cout << '\t' << name << "    " << std::boolalpha
                      << isBitSet(radioTap->presentFlags, bit) << std::endl;
        }
    }

    // TODO: To study: https://mrncciew.com/2014/10/08/802-11-mgmt-beacon-frame/
    void parseBacon(const uint8_t* packetBytes,
                    const PacketHeader& pktHeader)
    {
        const size_t packetLenTotal { pktHeader.incl_len  };
        std::cout << "Bytes captured: " << packetLenTotal << std::endl;

        const WiFiMACHeader* macHeader { reinterpret_cast<const WiFiMACHeader*>(packetBytes) };
        std::cout << "Destination address: "; Utilities::PrintMACAddress(macHeader->da); std::cout << std::endl;
        std::cout << "Transmitter address: "; Utilities::PrintMACAddress(macHeader->sa); std::cout << std::endl;
        std::cout << "BSS ID             : "; Utilities::PrintMACAddress(macHeader->bss); std::cout << std::endl;

        const BeaconFixedParams* fixedParams { reinterpret_cast<const BeaconFixedParams*>(packetBytes + sizeof(WiFiMACHeader)) };

        std::cout << "Timestamp          : " << fixedParams->timestamp << std::endl;
        std::cout << "Beacon Interval    : " << fixedParams->beaconInterval << std::endl;
        std::cout << "Capability Info    : " << fixedParams->capabilityInfo << std::endl;

        std::cout << "================================= Tags ====================================================\n";

        constexpr size_t tagsOffset { sizeof(WiFiMACHeader) + sizeof(BeaconFixedParams) };
        const uint8_t* tagPtr { packetBytes + tagsOffset };
        for (size_t offset = 0; packetLenTotal > (offset + tagsOffset + sizeof(TagParams)) ; /* ++idx */) {
            const TagParams* tagInfo { reinterpret_cast<const TagParams*>(tagPtr + offset) };
            offset += int(tagInfo->length) + sizeof(TagParams);

            std::cout << "Tag: " << int(tagInfo->number) << ", Len " << int(tagInfo->length)
                      << " | POS: " << offset  + tagsOffset << std::endl;
        }
    }

    void AnalyzeBeacons()
    {
        const std::vector<uint8_t> buffer { readPcapFile(beacon0) };
        const size_t dataBlockSize { buffer.size() - sizeof(GlobalHeader)};

        const uint8_t* const packetBlockPtr { buffer.data() + sizeof(GlobalHeader) };
        for (size_t nextBlockPos = 0; dataBlockSize > nextBlockPos; )
        {
            const PacketHeader *pktHeader { reinterpret_cast<const PacketHeader*>(packetBlockPtr + nextBlockPos) };
            parseBacon(packetBlockPtr + nextBlockPos + sizeof(PacketHeader), *pktHeader);
            nextBlockPos += sizeof(PacketHeader) + pktHeader->incl_len;
        }
    }
};

void PcapAnalyzer::TestAll()
{
    // AnalyzePcapTest();

    // WiFi::ReadAndParseFile();
    WiFi::AnalyzeBeacons();
}

/** Management Frame Information Elements

0   	- Service Set Identity (SSID)
1   	- Supported Rates
2   	- FH Parameter Set
3   	- DS Parameter Set
4   	- CF Parameter Set
5   	- Traffic Indication Map (TIM)
6   	- IBSS Parameter Set
7  	 	- Country Information  (802.11d)
8   	- Hopping Pattern Parameters (802.11d)
9   	- Hopping Pattern Table  (802.11d)
10  	- Request(802.11d)
11 		- BSS Load
12		- EDCA Parameter Set
13      - TSPEC
14 		- TCLAS
15 		- IE Type: Schedule"
16		- Challenge text
17-31   - Reserved (formerly for challenge text extension, before 802.11 shared key authentication was discontinued)
32 		- Power Constraint (802.11h)
33		- Power Capability (802.11h)
34 		- Transmit Power Control (TPC) Request (802.11h)
35 		- TPC Report (802.11h)
36 		- Supported Channels (802.11h)
37		- Channel Switch Announcement (802.11h)
38		- Measurement Request (802.11h)
39		- Measurement Report (802.11h)
40 		- Quiet (802.11h)
41 		- IBSS DFS (802.11h)
42		- ERP information (802.11h)
43 		- TS Delay
44 		- TCLAS Processing
45		- HT Capabilities (802.11n)
46 		- QoS Capabilit
48 		- RSN Information | Robust Security Network (802.11i)
50 		- Extended Supported Rates (802.11g)
59 		- Supported Operating Classes
61		- HT Information (802.11n)
127		- Extended Capabilities
221		- Vendor Specific
32-255 	- Reserved; unused


*/