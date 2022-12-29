/**============================================================================
Name        : FindProcessForTCPConnection.h
Created on  : 08.09.2022.
Author      : Tokmakov Andrei
Version     : 1.0
Copyright   : Your copyright notice
Description : FindProcessForTCPConnection
============================================================================**/

#include "FindProcessForTCPConnection.h"

#include <optional>
#include <iostream>
#include <string>
#include <string_view>
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <random>
#include <thread>
#include <future>
#include <mutex>
#include <charconv>
#include <numbers>
#include <ostream>
#include <regex>

#include <array>
#include <unordered_map>

#include <dirent.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

namespace {
    constexpr std::string_view TCP_FILE {R"(/proc/net/tcp)"};
    constexpr std::string_view PROC_FS_PATH { R"(/proc)" };
}

namespace LinuxFilesystemTests
{
    std::string fdType2Str(int type);
    std::string statType2Str(int type) ;
}

namespace Networking {

    constexpr uint32_t RECV_BUFFER_SIZE { 2048 };
    constexpr int INVALID_SOCKET { -1 };
    constexpr int SOCKET_ERROR { -1 };

    struct Socket {
        int socket { INVALID_SOCKET};

        Socket(int s): socket {s} {
        }

        Socket& operator=(int s) {
            if (socket != s)
                closeSocket(socket);

            socket = s;
            return *this;
        }

        ~Socket() {
            closeSocket(socket);
        }

        [[nodiscard]]
        inline bool isValid() const noexcept {
            return INVALID_SOCKET == socket;
        }

        [[nodiscard]]
        operator int() const { // No explicit
            return socket;
        }


        explicit operator bool() const noexcept {
            return (INVALID_SOCKET != socket);
        }

    private:

        static void closeSocket(int s)
        {
            std::cout << "Closing socket(" << s << ")\n";
            if (INVALID_SOCKET != s && SOCKET_ERROR == ::close(s)) {
                std::cout << "close() function failed with error: " << errno << std::endl;
            }
        }
    };
}


namespace Processes
{
    struct FileStatInfo {
        std::string path {}; // Remove???
        uint32_t iNode {0};
        bool isSocket = false;
    };

    [[nodiscard]]
    std::vector<uint32_t> getProcessList()
    {
        const std::regex processFileName("^[0-9]+$");
        std::vector<uint32_t> ids;
        if (DIR *dir = ::opendir(PROC_FS_PATH.data()); nullptr != dir) {
            dirent *ent;
            while (nullptr != (ent = ::readdir(dir))) {
                // std::cout << ent->d_name << std::endl;
                if (std::regex_match(ent->d_name, processFileName)){
                    ids.push_back(atoi(ent->d_name));
                }
            }
            ::closedir(dir);
        }
        return ids;
    }

    std::vector<std::pair<uint32_t, long>> getProcessList2()
    {
        const auto now = std::filesystem::file_time_type::clock::now();
        constexpr std::string_view dirPath {PROC_FS_PATH };
        const std::regex processFileName("^[0-9]+$");

        std::vector<std::pair<uint32_t, long>> ids;
        for (const auto& entry : std::filesystem::directory_iterator(dirPath))
        {
            const std::string& filename = entry.path().filename().string();
            std::filesystem::file_time_type writeTime  = std::filesystem::last_write_time(entry);
            if (entry.is_directory() && std::regex_match(filename, processFileName)) {
                ids.emplace_back(atoi(filename.c_str()),
                                 duration_cast<std::chrono::seconds>(now - writeTime).count());
            }
        }

        std::sort(ids.begin(), ids.end(), [](const auto& a, const auto& b) {
            return b.second >= a.second;
        });

        return ids;
    }


    [[nodiscard]]
    std::vector<uint32_t> getProcessFDs(uint32_t pid)
    {
        std::string path {PROC_FS_PATH};
        path.append("/").append(std::to_string(pid)).append("/fd");

        std::vector<uint32_t> fds;
        if (DIR *dir = opendir(path.data()); dir) {
            dirent *entry;
            while (nullptr != (entry = readdir(dir))) {
                uint32_t fd = atoi(entry->d_name);
                if (fd > 0)
                    fds.push_back(fd);
            }
            ::closedir(dir);
        } else {
            std::cout << "Failed to open " << path << std::endl;
        }
        return fds;
    }

    [[nodiscard]]
    FileStatInfo StatFile(uint32_t pid, uint32_t fd)
    {
        FileStatInfo stInfo;
        stInfo.path.assign(PROC_FS_PATH);
        stInfo.path.append("/").append(std::to_string(pid)).append("/fd/").append(std::to_string(fd));

        struct stat st{};
        if (-1 != ::stat(stInfo.path.c_str(), &st)) {
            stInfo.iNode = st.st_ino;
            stInfo.isSocket = ((st.st_mode & S_IFMT) == S_IFSOCK);
        }
        return stInfo;
    }

    void Tests() {
        std::vector<uint32_t> procs1 = getProcessList();
        const auto& procs2 = getProcessList2();

        // std::cout << procs1.size() << "  " << procs2.size() << std::endl;

        for (const auto& [id, t]: procs2) {
            std::cout << id << "   " << t << std::endl;
        }
    }
}

// TODO: Remove from file -> Move to lib
namespace Networking::UtilitiesLocal
{

    [[nodiscard("Don't forget to use the return value somehow.")]]
    std::string HostToIp(std::string_view host) noexcept
    {
        const hostent* hostname { gethostbyname(host.data()) };
        if (hostname)
            return std::string { inet_ntoa(**(in_addr**)hostname->h_addr_list) };
        return std::string {};
    }

    void EstablishTCPConnection(std::string_view ipAddr,
                                uint16_t port = 80)
    {
        Socket socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (!socket) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        sockaddr_in server { PF_INET, htons(port) };
        server.sin_addr.s_addr = inet_addr(ipAddr.data());

        std::cout << "Connecting to server..." << std::endl;
        int error = ::connect(socket, (sockaddr*)&server, sizeof(server));
        if (SOCKET_ERROR == error) {
            std::cout << "Connect function failed with error: " << errno << std::endl;
            return;
        } else {
            std::cout << "Connected " << ipAddr << ":" << port << '\n';
        }
    }

    // TODO: Bad!!
    uint16_t __localPort = 0;

    void ConnectToAddrAndSleep(std::string_view ipAddress,
                               uint16_t port,
                               const std::chrono::duration<int64_t>& timeout = std::chrono::seconds (0))
    {
        Socket socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (!socket) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        sockaddr_in server { PF_INET, htons(port) };
        server.sin_addr.s_addr = inet_addr(ipAddress.data());

        std::cout << "Connecting to server..." << std::endl;
        int error = ::connect(socket, (sockaddr*)&server, sizeof(server));
        if (SOCKET_ERROR == error) {
            std::cout << "Connect function failed with error: " << errno << std::endl;
            return;
        } else {
            std::cout << "Connected " << ipAddress << ":" << port << '\n';
        }


        sockaddr_in sin {};
        socklen_t addrlen = sizeof(sin);
        if (getsockname(socket, (sockaddr *)&sin, &addrlen) == 0 &&
            sin.sin_family == AF_INET && addrlen == sizeof(sin))
        {
            __localPort = ntohs(sin.sin_port);
        }

        std::this_thread::sleep_for(timeout);
    }

    void ConnectToHostAndSleep(std::string_view hostName,
                               uint16_t port,
                               const std::chrono::duration<int64_t>& timeout = std::chrono::seconds (0))
    {
        const std::string ipAddr { HostToIp(hostName) };
        if (ipAddr.empty())
            return;

        ConnectToAddrAndSleep(hostName, port, timeout);
    }
};




namespace EncodingLocal
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
        int n = std::sprintf(ipStr.data(), "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
        ipStr.resize(n);
        return ipStr;
    }

    uint32_t v4IpToInt(std::string_view ipAddress) {
        return htonl(inet_addr(ipAddress.data()));
    }
}

/*
namespace SessionCaching
{
    struct TCPSession {
        uint32_t srcIp {0};
        uint32_t dstIp {0};
        uint16_t srcPort {0};
        uint16_t dstPort {0};
    };

    bool operator==(const TCPSession& sess1, const TCPSession& sess2) {
        return (sess1.srcIp == sess2.srcIp &&sess1.dstIp == sess2.dstIp &&
                sess1.srcPort == sess2.srcPort && sess1.dstPort == sess2.dstPort);
    }

    template <class T>
    inline void hash_combine(std::size_t & s, const T& v)
    {
        std::hash<T> h;
        s^= h(v) + 0x9e3779b9 + (s<< 6) + (s>> 2);
    }

    struct TCPSessionHasher
    {
        static inline constexpr std::hash<uint32_t> hasher {};

        std::size_t operator()(const TCPSession& session) const noexcept {
            std::size_t res = hasher(session.srcIp);
            res^= hasher(session.dstIp) + 0x9e3779b9 + (res<< 6) + (res>> 2);
            res^= hasher(session.srcPort) + 0x9e3779b9 + (res<< 6) + (res>> 2);
            res^= hasher(session.dstPort) + 0x9e3779b9 + (res<< 6) + (res>> 2);
            return res;
        }
    };

    using SessionProc = std::pair<uint32_t, uint32_t>;


    std::unordered_map<TCPSession,
            SessionProc,
            TCPSessionHasher> cache;
}
*/

namespace LinuxFilesystemTests
{

    std::string fdType2Str(int type) {
        switch (type) {
            case DT_BLK:
                return "Block device";
            case DT_CHR:
                return "Character device";
            case DT_DIR:
                return "Directory";
            case DT_FIFO:
                return "FIFO";
            case DT_LNK:
                return "Symbolic link";
            case DT_REG :
                return "Regular file";
            case DT_SOCK:
                return "UNIX domain socket";
            case DT_UNKNOWN:
                return "DT_UNKNOWN";
            default:
                return "DT_UNKNOWN 2";
        }
    }

    std::string statType2Str(int type) {
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


    void ReadDir() {
        constexpr std::string_view path{R"(/proc/1/fd)"};

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
            if (-1 != ::stat(fdPath.c_str(), &st)) {
                std::cout << fdPath << "  " << statType2Str(st.st_mode)
                          << "   " << st.st_mode << "   " << st.st_ino << std::endl;
            }
        };

        ::closedir(dir);
    }
}

namespace TCPProcessLookup
{
    using SessionProc = std::pair<uint32_t, uint32_t>;
    std::unordered_map<uint32_t, SessionProc> cache;

    struct TCPSession {
        uint32_t srcIp {0};
        uint32_t dstIp {0};
        uint16_t srcPort {0};
        uint16_t dstPort {0};
    };

    struct Params {
        uint32_t srcIp;
        uint32_t dstIp;
        uint16_t srcPort = 0;
        uint16_t dstPort = 0;
        uint32_t iNode = 0;
    };

    std::vector<std::string> split(std::string_view input,
                                   std::string_view delimiter = " ") {
        std::vector<std::string> output;
        for (size_t first = 0; first < input.size(); ) {
            const auto second = input.find_first_of(delimiter, first);
            if (first != second)
                output.emplace_back(input.substr(first, second - first));
            if (second == std::string_view::npos)
                break;
            first = second + 1;
        }
        return output;
    }

    // TODO: Refactor
    Params parseLine(std::string_view line)
    {
        Params params {};
        const std::vector<std::string> parts { split(line, " ") };
        if (parts.size() < 10)
            return params;

        {
            const std::string_view str {parts[1]};
            const auto pos  = str.find(":");
            if (std::string::npos == pos)
                return params;
            params.srcIp = htonl(EncodingLocal::hext2Int(str.substr(0, pos)));
            params.srcPort = EncodingLocal::hext2Int(str.substr(pos + 1, str.size() - pos - 1));
        }

        {
            const std::string_view str {parts[2]};
            const auto pos  = str.find(":");
            if (std::string::npos == pos)
                return params;
            params.dstIp = htonl(EncodingLocal::hext2Int(str.substr(0, pos)));
            params.dstPort = EncodingLocal::hext2Int(str.substr(pos + 1, str.size() - pos - 1));
        }

        params.iNode = atoi(parts[9].c_str());
        return params;
    }

    void GetTCPConnectionsFromFileSystem()
    {
        std::vector<std::string> content {};
        if (std::ifstream file = std::ifstream(TCP_FILE.data()); file.is_open() && file.good()) {
            while (std::getline(file, content.emplace_back())) {  }
        }
        content.pop_back();

        for (const auto& line: content) {
            const Params params = parseLine(line);
            std::cout << params.srcIp << ":" << params.srcPort << " -> " << params.dstIp << ":"
                      << params.dstPort << " | iNode: " << params.iNode << std::endl;
        }
    }

    //------------------------------------------------------------------------------------

    void printCache() {
        std::cout << "Cache size: " << cache.size() << std::endl;
        for (const auto& [k, v]: cache) {
            std::cout << k << " {pid: " << v.first << ", fd: " << v.second << "]\n";
        }
    }

    void FindTCPConnectionInFileSystem(const TCPSession& session)
    {
        constexpr std::string_view path { R"(/proc/net/tcp)"};

        /** Start time measure: **/
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        std::vector<std::string> content {};
        if (std::ifstream file = std::ifstream(path.data()); file.is_open() && file.good()) {
            while (std::getline(file, content.emplace_back())) {  }
        }

        std::optional<Params> connectionParams;
        for (const auto& line: content) {
            const Params params = parseLine(line);
            if (params.srcIp == session.srcIp && params.srcPort == session.srcPort &&
                params.dstIp == session.dstIp && params.dstPort == session.dstPort) {
                connectionParams = params;
                break;
            }
        }

        if (!connectionParams.has_value()) {
            std::cout << "Failed to find connection in '" << path << "'" << std::endl;
            return;
        }

        auto [iter, ok] = cache.emplace(connectionParams->iNode, SessionProc{});
        if (ok) {
            std::cout << "Failed to found in cache\n";
        }

        const Params& params = connectionParams.value();
        std::cout << params.srcIp << ":" << params.srcPort << " -> " << params.dstIp << ":"
                  << params.dstPort << " | iNode: " << params.iNode << std::endl;

        const std::vector<uint32_t> processList = Processes::getProcessList();
        // std::cout << "Process list size: " << processList.size() << std::endl;
        for (const uint32_t pid: processList) {
            const std::vector<uint32_t> fdList = Processes::getProcessFDs(pid);
            for (const uint32_t fd: fdList) {
                const Processes::FileStatInfo stat = Processes::StatFile(pid, fd);
                if (stat.isSocket && stat.iNode == params.iNode)
                {
                    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
                    std::cout << "Process found in " << time_span.count() << " seconds.\n";

                    std::cout << "   " << stat.path << "  " << stat.iNode << ". isSocket: "
                              << std::boolalpha << stat.isSocket << std::endl;
                    cache[stat.iNode] = {pid, fd};
                    // printCache();

                    return;
                }
                cache[stat.iNode] = {pid, fd};
            }
        }
    }


    void FindTCPConnectionInFileSystem2(const TCPSession& session)
    {
        constexpr std::string_view path { R"(/proc/net/tcp)"};

        /** Start time measure: **/
        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        std::vector<std::string> content {};
        if (std::ifstream file = std::ifstream(path.data()); file.is_open() && file.good()) {
            while (std::getline(file, content.emplace_back())) {  }
        }

        std::optional<Params> connectionParams;
        for (const auto& line: content) {
            const Params params = parseLine(line);
            if (params.srcIp == session.srcIp && params.srcPort == session.srcPort &&
                params.dstIp == session.dstIp && params.dstPort == session.dstPort) {
                connectionParams = params;
                break;
            }
        }

        if (!connectionParams.has_value()) {
            std::cout << "Failed to find connection in '" << path << "'" << std::endl;
            return;
        }

        auto [iter, ok] = cache.emplace(connectionParams->iNode, SessionProc{});
        if (ok) {
            std::cout << "Failed to found in cache\n";
        }

        const Params& params = connectionParams.value();
        std::cout << params.srcIp << ":" << params.srcPort << " -> " << params.dstIp << ":"
                  << params.dstPort << " | iNode: " << params.iNode << std::endl;

        const std::vector<std::pair<uint32_t, long>> processList = Processes::getProcessList2();
        for (const auto& [pid, time]: processList) {
            const std::vector<uint32_t> fdList = Processes::getProcessFDs(pid);
            for (const uint32_t fd: fdList) {
                const Processes::FileStatInfo stat = Processes::StatFile(pid, fd);
                if (stat.isSocket && stat.iNode == params.iNode)
                {
                    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
                    std::cout << "Process found in " << time_span.count() << " seconds.\n";

                    std::cout << "   " << stat.path << "  " << stat.iNode << ". isSocket: "
                              << std::boolalpha << stat.isSocket << std::endl;
                    cache[stat.iNode] = {pid, fd};
                    // printCache();

                    return;
                }
                cache[stat.iNode] = {pid, fd};
            }
        }
    }

    void EstablishConnection_FindProcess()
    {
        constexpr std::string_view serverIP {"87.250.250.242"};
        constexpr uint16_t serverPort { 80 };

        // TODO: get the connection params
        auto task = std::async([&]{
            Networking::UtilitiesLocal::ConnectToAddrAndSleep(serverIP, serverPort,
                                                         std::chrono::seconds(1));
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if (uint16_t localPort = Networking::UtilitiesLocal::__localPort; localPort) {
            const TCPSession session {EncodingLocal::v4IpToInt("192.168.31.154"),
                                      EncodingLocal::v4IpToInt(serverIP),
                                      localPort, serverPort};
            // FindTCPConnectionInFileSystem(session);
            FindTCPConnectionInFileSystem2(session);
        }

        task.wait();
    }
}


void FindProcessForTCPConnection::TestAll()
{
    // LinuxFilesystemTests::ReadDir();
    // LinuxFilesystemTests::ReadDir_R();
    // LinuxFilesystemTests::GetTCPConnectionsFromFileSystem();

    TCPProcessLookup::EstablishConnection_FindProcess();

    // Processes::Tests();
}
