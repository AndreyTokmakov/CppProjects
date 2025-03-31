/**============================================================================
Name        : UDSAsynchServer.cpp
Created on  : 31.03.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : UDSAsynchServer.cpp
============================================================================**/

#include "UDSAsynchServer.h"

#include <iostream>
#include <string_view>
#include <expected>
#include <thread>
#include <vector>
#include <filesystem>


#include <sys/poll.h>
#include <unistd.h>
#include <cerrno>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <fcntl.h>


namespace
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    using Socket = int32_t;

    constexpr int32_t RESULT_SUCCESS = 0;
    constexpr int32_t INVALID_HANDLE = -1;
    constexpr int32_t SOCKET_ERROR = -1;

    std::string err2String(const int32_t errorCode)
    {
        switch (errorCode)
        {
            case EPERM:   return "EPERM: Operation not permitted"s;
            case ENOENT:  return "ENOENT: No such file or directory"s;
            case ESRCH:   return "ESRCH: No such process"s;
            case EINTR:   return "EINTR: Interrupted system call"s;
            case EIO:     return "EIO: I/O error"s;
            case ENXIO:   return "ENXIO: No such device or address"s;
            case E2BIG:   return "E2BIG: Argument list too long"s;
            case ENOEXEC: return "ENOEXEC: Exec format error"s;
            case EBADF: return "EBADF: Bad file number"s;
            case ECHILD: return "ECHILD: No child processes"s;
            case EAGAIN: return "EAGAIN: Try again"s;
            case ENOMEM: return "ENOMEM: Out of memory"s;
            case EACCES: return "EACCES:  Permission denied"s;
            case EFAULT: return "EFAULT: Bad address"s;
            case ENOTBLK: return "ENOTBLK: Block device required"s;
            case EBUSY: return "EBUSY: Device or resource busy"s;
            case EEXIST: return "EEXIST: File exists"s;
            case EXDEV: return "EXDEV: Cross-device link"s;
            case ENODEV: return "ENODEV: No such device"s;
            case ENOTDIR: return "ENOTDIR: Not a directory"s;
            case EISDIR: return "EISDIR: Is a directory "s;
            case EINVAL: return "EINVAL: Invalid argument"s;
            case ENFILE: return "ENFILE: File table overflow"s;
            case EMFILE: return "EMFILE: Too many open files"s;
            case ENOTTY: return "ENOTTY: Not a typewriter"s;
            case ETXTBSY: return "ETXTBSY: Text file busy "s;
            case EFBIG: return "EFBIG: File too large"s;
            case ENOSPC: return "ENOSPC: No space left on device"s;
            case ESPIPE: return "ESPIPE: Illegal seek"s;
            case EROFS: return "EROFS: Read-only file system"s;
            case EMLINK: return "EMLINK: Too many links"s;
            case EPIPE: return "EPIPE: Broken pipe"s;
            case EDOM: return "EDOM: Math argument out of domain of func"s;
            case ERANGE: return "ERANGE: Math result not representable"s;
            default: return "Unknown"s;
        }
    }

    template<typename ResultType = bool , ResultType value = false>
    ResultType error(const std::string_view message)
    {
        std::cerr << message << ". Error (" << errno << "): " << err2String(errno) << std::endl;
        return value;
    }

    template<typename ResultType = bool , ResultType value = false>
    ResultType error(const std::string_view message, const int32_t errorCode)
    {
        std::cerr << message << errorCode << std::endl;
        return value;
    }

    struct Buffer
    {
        // TODO: Rename
        std::vector<unsigned char> buffer;
        size_t size;

        explicit Buffer(const size_t size = 1024 * 2 ) : buffer(size), size { 0 } {
        }

        void validateAvailableSize(const size_t bytesRequired)
        {
            if (bytesRequired > (buffer.size() - size))
            {
                buffer.resize(size + bytesRequired);
                std::cout << "Buffer size ---> " << buffer.size() << std::endl;
            }
        }
    };
}

namespace UDSAsynchServer
{
    struct UDSServer
    {
        constexpr static size_t maxReceiveSize { 2 * 1024 };
        constexpr static size_t MAX_DESCRIPTORS { 256 };
        constexpr static int32_t TIMEOUT { 3 * 60 * 1000 };

        Socket serverSocket { INVALID_HANDLE };
        std::filesystem::path filePath ;

        std::array<pollfd, MAX_DESCRIPTORS> fds {};
        uint32_t handlesCount { 0 };

        explicit UDSServer(std::filesystem::path udmSockPath);
        ~UDSServer();

        static void closeEvent(pollfd& pollEvent);
        void removeClosedHandles();

        [[nodiscard]]
        std::expected<bool, std::string> init() const;

        bool start();

        [[nodiscard]]
        static std::expected<bool, std::string> setSocketToNonBlock(Socket socket) ;
    };
}

namespace UDSAsynchServer
{
    UDSServer::UDSServer(std::filesystem::path udmSockPath): filePath { std::move( udmSockPath ) }
    {
        serverSocket = ::socket(AF_UNIX, SOCK_STREAM, 0);
        if (INVALID_HANDLE == serverSocket) {
            throw std::runtime_error("CLIENT: Create socket failed.");
        }

        if (const int32_t result = ::unlink(filePath.c_str()); INVALID_HANDLE == result)
        {
            if (const int error = errno; ENOENT != error && RESULT_SUCCESS != error) {
                throw std::runtime_error("Failed to unlink " + filePath.string() + " file");
            }
        }

        fds[0].fd = serverSocket;
        fds[0].events = POLLIN;

        handlesCount = 1;
    }

    UDSServer::~UDSServer()
    {
        if (const int32_t result = ::remove(filePath.c_str()); INVALID_HANDLE == result) {
            error("remove() failed. Result = ", result);
        }
        if (const int32_t result = ::close(serverSocket); INVALID_HANDLE == result) {
            error("close() failed. Result = ", result);
        }
    }

    [[nodiscard]]
    std::expected<bool, std::string> UDSServer::setSocketToNonBlock(const Socket socket)
    {
        const int flags = ::fcntl(socket, F_GETFL, 0);
        if (SOCKET_ERROR == ::fcntl(socket, F_SETFL, flags | O_NONBLOCK)) {
            return std::unexpected{"fcntl(F_SETFL, O_NONBLOCK) failed. Error: " + err2String(errno)};
        }
        return true;
    }

    [[nodiscard]]
    std::expected<bool, std::string> UDSServer::init() const
    {
        if (const auto result = setSocketToNonBlock(serverSocket); !result.has_value()) {
            return std::unexpected{"setSocketToNonBlock() failed. Error: " + result.error()};
        }
        constexpr int32_t yes { 1 };
        if (SOCKET_ERROR == ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
            return std::unexpected{"setsockopt(SOL_SOCKET, SO_REUSEADDR) failed. Error: " + err2String(errno)};
        }

        sockaddr_un serverAddr { .sun_family = AF_UNIX };
        // std::copy(filePath.begin(), filePath.end(), serverAddr.sun_path);
        std::copy_n(filePath.c_str(), filePath.string().size(), serverAddr.sun_path);

        constexpr uint32_t len = sizeof(serverAddr);
        if (RESULT_SUCCESS != ::bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddr), len)) {
            return std::unexpected {"bind() failed. Error: " + err2String(errno)};
        }

        if (RESULT_SUCCESS != ::listen(serverSocket, 32)) {
            return std::unexpected {"listen() failed. Error: " + err2String(errno)};
        }

        return true;
    }

    void UDSServer::removeClosedHandles()
    {
        uint32_t pos = 1;
        for (uint32_t idx = pos; idx < handlesCount; ++idx ) {
            if (fds[idx].fd != -1)
                std::swap(fds[idx], fds[pos++]);
        }
        handlesCount = pos;
    }

    void UDSServer::closeEvent(pollfd& pollEvent)
    {
        ::close(pollEvent.fd);
        pollEvent.fd = -1;
    }

    bool UDSServer::start()
    {
        Buffer buffer;
        uint32_t currentSize { 0 };
        while (true)
        {
            if (const int32_t result = ::poll(fds.data(), handlesCount, TIMEOUT); SOCKET_ERROR == result) {
                return error("poll() failed ");
            } else if (0 == result) {
                /** Timeout **/
            }

            currentSize = handlesCount;
            for (uint32_t idx = 0; idx < currentSize; idx++)
            {
                pollfd& pollEvent { fds[idx] };
                if (0 == pollEvent.revents)
                    continue;
                if (pollEvent.revents != POLLIN)
                {
                    if (pollEvent.revents & POLLHUP)
                    {
                        closeEvent(pollEvent);
                        break;
                    }
                }
                else if (pollEvent.fd == serverSocket) /** Listening descriptor is readable. **/
                {
                    Socket clientSocket { INVALID_HANDLE };
                    while (true)
                    {
                        clientSocket = ::accept(serverSocket, nullptr, nullptr);
                        if (SOCKET_ERROR == clientSocket)
                        {
                            if (errno != EWOULDBLOCK || EAGAIN != errno) {
                                return error("accept() failed = " + std::to_string(pollEvent.revents));
                            }
                            break;
                        }
                        if (const auto result = setSocketToNonBlock(clientSocket); !result) {
                            std::cerr << result.error() << std::endl;
                        }

                        fds[handlesCount].fd = clientSocket;
                        fds[handlesCount].events = POLLIN | POLLHUP;
                        ++handlesCount;
                        break;
                    }
                }
                else
                {
                    const Socket clientSocket = pollEvent.fd;
                    while (true)
                    {
                        // TODO:
                        //  - Refactor this part:
                        //  - add func to return pointer() to data to write into Buffer
                        //  - how to not update Buffer::size manually ????
                        buffer.validateAvailableSize(2 * 1024);
                        const int64_t bytesRead = ::recv(clientSocket,
                            buffer.buffer.data() + buffer.size, maxReceiveSize, 0);
                        buffer.size += bytesRead;


                        if (SOCKET_ERROR == bytesRead)
                        {
                            if (errno != EWOULDBLOCK || EAGAIN != errno) {
                                return error("recv() failed = " + std::to_string(pollEvent.revents));
                            }
                            break;
                        }
                        if (0 == bytesRead)
                        {
                            closeEvent(pollEvent);
                            break;
                        }
                        // TODO:
                        //  - May we can do something around the copy ???
                        //  - Need to keep BUFFER <---> Session
                        //  -
                        //  - Not use Receive_Buffer --> read to Buffer for ClientID
                        //  - Once 'read data' closed and buffer passed to handle()
                        //    Buffer may be stored back to some POOL

                        // data.insert(data.end(), buffer.begin(), buffer.begin() + bytesRead);
                        if (maxReceiveSize > bytesRead)
                        {
                            std::cout << std::string_view(reinterpret_cast<const char *>(
                                buffer.buffer.data()), buffer.size)<< std::endl;

                            buffer.size = 0;
                        }
                    }
                }
            }
        }
    }
}

void UDSAsynchServer::TestAll()
{
    UDSServer server { "/tmp/unix_socket" };
    const std::expected<bool, std::string> ok = server.init();
    if (!ok.has_value()) {
        std::cerr << "Failed to initialize server. Error: " << ok.error() << std::endl;
        return;
    }

    std::cout << "Server started." << std::endl;
    server.start();
}