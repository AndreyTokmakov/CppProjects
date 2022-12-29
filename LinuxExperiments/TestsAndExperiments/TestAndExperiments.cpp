/**============================================================================
Name        : TestAndExperiments.сзз
Created on  : 07.09.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : TestAndExperiments
============================================================================**/

#include "TestAndExperiments.h"


#include <optional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>
#include <cmath>
#include <exception>
#include <random>
#include <thread>
#include <future>
#include <mutex>
#include <charconv>
#include <numeric>
#include <numbers>
#include <ostream>

#include <array>
#include <set>
#include <unordered_set>
#include <unordered_map>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "../Networking/Utilities.h"

namespace Encoding
{
    static constexpr uint8_t hexCode(unsigned char symbol) noexcept {
        if (symbol >= '0' && symbol <= '9')
            return symbol - '0';
        if (symbol >= 'A' && symbol <= 'F')
            return symbol - 'A' + 10;
        if (symbol >= 'a' && symbol <= 'f')
            return symbol - 'a' + 10;
        return 0;
    }

    static constexpr uint8_t hex2UChar(std::string_view hexValue) noexcept {
        return 16 * hexCode(hexValue[0]) + hexCode(hexValue[1]);
    }

    std::vector<uint8_t> hex2Bytes(std::string_view hexString) noexcept {
        std::vector<uint8_t> bytes;
        bytes.reserve(hexString.length()/2);
        for (size_t length = hexString.length(), i = 0; i < length; i += 2)
            bytes.push_back(hex2UChar(hexString.substr(i, 2)));
        return bytes;
    }

    // TODO: Could be slow
    uint32_t hext2Int(std::string_view hex)
    {
        uint32_t num;
        std::istringstream iss(hex.data());
        iss >> std::hex >> num;
        return num;
    }

    std::string int2IP(uint32_t ip)
    {
        std::array<unsigned char, 4> bytes {};
        for (size_t i = 0; i < bytes.size(); ++i) {
            bytes[i] = (ip >> i*8) & 0xFF;
        }
        std::string ipStr(16, '\0');
        int n = std::sprintf(ipStr.data(), "%d.%d.%d.%d", bytes[0], bytes[1], bytes[2], bytes[3]);
        ipStr.resize(n);
        return ipStr;
    }
}


namespace TestAndExperiments
{
    std::string fdType2Str(int type) {
        switch(type) {
            case DT_BLK: return "Block device";
            case DT_CHR: return "Character device";
            case DT_DIR: return "Directory";
            case DT_FIFO: return "FIFO";
            case DT_LNK: return "Symbolic link";
            case DT_REG : return "Regular file";
            case DT_SOCK: return "UNIX domain socket";
            case DT_UNKNOWN: return "DT_UNKNOWN";
            default: return "DT_UNKNOWN 2";
        }
    }

    std::string statType2Str(int type)
    {
        if ((type & S_IFMT) == S_IFSOCK)
            return "Socket";
        else if ((type & S_IFMT) == S_IFLNK)
            return "Symbolic link";
        else if ((type & S_IFMT) == S_IFREG)
            return "Regular file";
        else if ((type & S_IFMT) == S_IFBLK)
            return "Block device";
        else if ((type & S_IFMT) == S_IFDIR)
            return "Directory";
        else if ((type & S_IFMT) == S_IFCHR)
            return "Symbolic device";
        else if ((type & S_IFMT) == S_IFIFO)
            return "FIFO";
        else
            return "UNKNOWN";
    }


    void ReadProcFDs() {
        constexpr std::string_view path { R"(/proc/1/fd)" };

        DIR *dir = opendir(path.data());
        if (!dir) {
            std::cout << "Failed to open " << path << std::endl;
            return;
        };

        dirent *entry;
        while (nullptr != (entry = readdir(dir))) {
            // std::cout << entry->d_ino << "  " << entry->d_name <<  "   " << fdType2Str(entry->d_type) << std::endl;

            std::string fdPath(path);
            fdPath.append("/").append(entry->d_name);
            // std::cout <<fdPath << std::endl;

            struct stat st;
            if (-1 != ::stat(fdPath.c_str(), &st))
            {
                std::cout << fdPath << "  " << statType2Str(st.st_mode)
                          << "   " << st.st_mode << "   " << st.st_ino << std::endl;
            }
        };

        ::closedir(dir);
    }

    struct Params {
        std::string srcIp;
        std::string dstIp;
        uint32_t srcPort = 0;
        uint32_t dstPort = 0;
        uint32_t iNode = 0;
    };

    Params parseLine(std::string_view line)
    {
        Params params {};
        auto pos  = line.find(": ") ;
        if (std::string::npos == pos)
            return {};
        auto pos2  = line.find(':', (pos += 2)) ;
        if (std::string::npos == pos2)
            return {};

        params.srcIp = Encoding::int2IP(Encoding::hext2Int(line.substr(pos, pos2 - pos)));
        auto pos3  = line.find(' ', (pos2 += 1)) ;
        if (std::string::npos == pos3)
            return {};

        params.srcPort = Encoding::hext2Int(line.substr(pos2, pos3 - pos2));
        auto pos4  = line.find(':', (pos3 += 1));
        if (std::string::npos == pos4)
            return {};

        params.dstIp = Encoding::int2IP(Encoding::hext2Int(line.substr(pos3, pos4 - pos3)));
        auto pos5 = line.find(' ', (pos4 += 1)) ;
        if (std::string::npos == pos5)
            return {};

        params.dstPort = Encoding::hext2Int(line.substr(pos4, pos5 - pos4));
        return params;
    }

    void GetTCPConnectionsFromFileSystem()
    {
        constexpr std::string_view path { R"(/proc/net/tcp)"};

        std::vector<std::string> content {};
        if (std::ifstream file = std::ifstream(path.data()); file.is_open() && file.good()) {
            while (std::getline(file, content.emplace_back())) {  }
        }

        for (const auto& line: content) {
            parseLine(line);
        }
    }

    //------------------------------------------------------------------------------------

    void FindTCPConnectionInFileSystem(std::string_view srcIp, uint32_t srcPort,
                                       std::string_view dstIp, uint32_t dstPort)
    {
        constexpr std::string_view path { R"(/proc/net/tcp)"};

        std::vector<std::string> content {};
        if (std::ifstream file = std::ifstream(path.data()); file.is_open() && file.good()) {
            while (std::getline(file, content.emplace_back())) {  }
        }

        for (const auto& line: content) {
            const Params params = parseLine(line);

            std::cout << line << std::endl;
            std::cout << params.srcIp << ":" << params.srcPort << " -> "
                      << params.dstIp << ":" << params.dstPort << std::endl;

            if (params.srcIp == srcIp && params.srcPort == srcPort &&
                params.dstIp == dstIp && params.dstPort == dstPort) {
                std::cout << "OK" << std::endl;
                break;
            }
        }
    }

    void EstablishConnection_FindProcess()
    {
        // TODO: get the connection params
        auto task = std::async([]{
            Networking::Utilities::ConnectToHostAndSleep("ya.ru", 80, std::chrono::seconds(10));
        });

        std::this_thread::sleep_for(std::chrono::seconds(2));

        FindTCPConnectionInFileSystem("192.168.31.154", 47950, "87.250.250.242", 80);

        task.wait();
    }
};


void TestAndExperiments::TestAll()
{
    // ReadProcFDs();
    // GetTCPConnectionsFromFileSystem();

    EstablishConnection_FindProcess();


    // std::cout << Encoding::hext2Int("23BF") << std::endl;
    // std::cout << Encoding::int2IP(Encoding::hext2Int("64C0695B")) << std::endl;


};

