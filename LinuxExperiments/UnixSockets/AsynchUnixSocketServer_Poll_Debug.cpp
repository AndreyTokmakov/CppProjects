/**============================================================================
Name        : AsynchUnixSocketServer_Poll_Debug.cpp
Created on  : 30.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AsynchUnixSocketServer_Poll_Debug.cpp
============================================================================**/

#include "AsynchUnixSocketServer_Poll_Debug.h"

#include <unistd.h>
#include <cerrno>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

#include <iostream>
#include <string_view>
#include <thread>
#include <chrono>
#include <utility>
#include <expected>

#define RESULT_SUCCESS   (0)
#define INVALID_HANDLE   (-1)
#define SOCKET_ERROR     (-1)
#define SERVER_SOCK_PATH "/tmp/unix_socket"

#include <syscall.h>

namespace
{
    using namespace std::string_literals;
    using namespace std::string_view_literals;
    using Socket = int32_t;

    struct SocketGuard
    {
        Socket socketHandle { INVALID_HANDLE };

        explicit SocketGuard(Socket sock): socketHandle { sock } {
        }

        ~SocketGuard()
        {
            if (INVALID_HANDLE != socketHandle)
            {
                // TODO: Check result
                [[maybe_unused]] const int32_t result = ::close(socketHandle);
                // std::cout << "Socket " << socketHandle << " is closed\n";
            }
        }

        SocketGuard(const SocketGuard&) = delete;
        SocketGuard(SocketGuard&&) noexcept = delete;

        SocketGuard& operator=(const SocketGuard&) = delete;
        SocketGuard& operator=(SocketGuard&&) noexcept = delete;
    };

    std::string err2String(const int32_t errorCode)
    {
        switch (errorCode)
        {
            case EPERM: return "EPERM: Operation not permitted"s;
            case ENOENT: return "ENOENT: No such file or directory"s;
            case ESRCH: return "ESRCH: No such process"s;
            case EINTR: return "EINTR: Interrupted system call"s;
            case EIO: return "EIO: I/O error"s;
            case ENXIO: return "ENXIO: No such device or address"s;
            case E2BIG: return "E2BIG: Argument list too long"s;
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
    ResultType error(std::string_view message)
    {
        std::cerr << message << ". Error (" << errno << "): " << err2String(errno) << std::endl;
        return value;
    }

    template<typename ResultType = bool , ResultType value = false>
    ResultType error(std::string_view message, int32_t errorCode)
    {
        std::cerr << message << errorCode << std::endl;
        return value;
    }
}

namespace AsynchUnixSocketServer_Poll_Debug
{
    std::ostream& operator<<(std::ostream& stream, const pollfd& pollFd)
    {
        stream << "Event { fd: " << pollFd.fd << ", revents: [ ";
        {
            if (pollFd.revents & POLLIN) stream << "POLLIN ";
            if (pollFd.revents & POLLPRI) stream << "POLLPRI ";
            if (pollFd.revents & POLLOUT) stream << "POLLOUT ";
            if (pollFd.revents & POLLRDNORM) stream << "POLLRDNORM ";
            if (pollFd.revents & POLLWRNORM) stream << "POLLWRNORM ";
            if (pollFd.revents & POLLWRBAND) stream << "POLLWRBAND ";
            if (pollFd.revents & POLLMSG) stream << "POLLMSG ";
            if (pollFd.revents & POLLRDHUP) stream << "POLLRDHUP ";
            if (pollFd.revents & POLLERR) stream<< "POLLERR ";
            if (pollFd.revents & POLLNVAL) stream << "POLLNVAL ";
            if (pollFd.revents & POLLHUP) stream << "POLLHUP ";
        }
        stream << "]} ";
        return stream;
    }

    struct UDSAsynchServer
    {
        constexpr static size_t BUFFER_SIZE { 10 * 1024 };
        constexpr static size_t MAX_DESCRIPTORS { 256 };
        constexpr static int32_t TIMEOUT { 3 * 60 * 1000 };

        Socket serverSocket { INVALID_HANDLE };
        std::string filePath { SERVER_SOCK_PATH };

        std::array<char, BUFFER_SIZE> buffer {};
        // TODO: Rename
        std::array<pollfd, MAX_DESCRIPTORS> fds {};
        uint32_t handlesCount { 0 };

        explicit UDSAsynchServer(std::string udmSockPath): filePath { std::move( udmSockPath ) }
        {
            serverSocket = ::socket(AF_UNIX, SOCK_STREAM, 0);
            if (INVALID_HANDLE == serverSocket) {
                throw std::runtime_error("CLIENT: Create socket failed.");
            }

            if (const int32_t result = ::unlink(filePath.data()); INVALID_HANDLE == result)
            {
                if (const int error = errno; ENOENT != error && RESULT_SUCCESS != error) {
                    throw std::runtime_error("Failed to unlink " + filePath + " file");
                }
            }

            fds[0].fd = serverSocket;
            fds[0].events = POLLIN;
            handlesCount = 1;
        }

        ~UDSAsynchServer()
        {
            if (const int32_t result = ::remove(filePath.data()); INVALID_HANDLE == result) {
                error("remove() failed. Result = ", result);
            }
            if (const int32_t result = ::close(serverSocket); INVALID_HANDLE == result) {
                error("close() failed. Result = ", result);
            }
        }

        [[nodiscard]]
        std::expected<bool, std::string> setSocketToNonBlock(const Socket socket) const
        {
            const int flags = ::fcntl(socket, F_GETFL, 0);
            if (SOCKET_ERROR == ::fcntl(socket, F_SETFL, flags | O_NONBLOCK)) {
                return std::unexpected{"fcntl(F_SETFL, O_NONBLOCK) failed. Error: " + err2String(errno)};
            }
            return true;
        }

        [[nodiscard]]
        std::expected<bool, std::string> init() const
        {
            if (const auto result = setSocketToNonBlock(serverSocket); !result.has_value()) {
                return std::unexpected{"setSocketToNonBlock() failed. Error: " + result.error()};
            }
            int32_t yes { 1 };
            if (SOCKET_ERROR == ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
                return std::unexpected{"setsockopt(SOL_SOCKET, SO_REUSEADDR) failed. Error: " + err2String(errno)};
            }

            const sockaddr_un serverAddr { .sun_family = AF_UNIX, .sun_path = SERVER_SOCK_PATH };
            uint32_t len = sizeof(serverAddr);
            if (RESULT_SUCCESS != ::bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddr), len)) {
                return std::unexpected{"bind() failed. Error: " + err2String(errno)};
            }

            if (RESULT_SUCCESS != ::listen(serverSocket, 32)) {
                return std::unexpected{"listen() failed. Error: " + err2String(errno)};
            }

            return true;
        }

        void removeClosedHandles()
        {
            uint32_t pos = 1;
            for (uint32_t idx = pos; idx < handlesCount; ++idx ) {
                if (fds[idx].fd != -1)
                    std::swap(fds[idx], fds[pos++]);
            }
            handlesCount = pos;
        }

        void closeEvent(pollfd& pollEvent)
        {
            ::close(pollEvent.fd);
            pollEvent.fd = -1;
            removeClosedHandles();
        }

        bool start()
        {
            std::string message;
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
                                } else {
                                    // std::cout << "Accept ==> EWOULDBLOCK" << std::endl;
                                    break;
                                }
                            }
                            else
                            {
                                std::cout << "New incoming connection. Client socket = " << clientSocket << std::endl;
                                if (const auto result = setSocketToNonBlock(clientSocket); !result) {
                                    std::cerr << result.error() << std::endl;
                                }

                                fds[handlesCount].fd = clientSocket;
                                fds[handlesCount].events = POLLIN | POLLHUP;
                                ++handlesCount;
                                break;
                            }
                        }
                    }
                    else
                    {
                        const Socket clientSocket = pollEvent.fd;
                        while (true)
                        {
                            const int64_t bytesRead = ::recv(clientSocket, buffer.data(), BUFFER_SIZE, 0);
                            if (SOCKET_ERROR == bytesRead)
                            {
                                if (errno != EWOULDBLOCK || EAGAIN != errno) {
                                    return error("recv() failed = " + std::to_string(pollEvent.revents));
                                } else {
                                    // std::cout << "recv ==> EWOULDBLOCK" << std::endl;
                                    break;
                                }
                            }
                            else if (0 == bytesRead)
                            {
                                std::cout << "Close connection for client = " << clientSocket << std::endl;
                                closeEvent(pollEvent);
                                break;
                            }
                            else
                            {
                                message.assign(buffer.data(), bytesRead);
                                std::cout  <<  message << std::endl;
                            }
                        }
                    }
                }
            }
        }
    };

    void runServer()
    {
        UDSAsynchServer server (SERVER_SOCK_PATH);
        const std::expected<bool, std::string> ok = server.init();
        if (!ok.has_value()) {
            std::cerr << "Failed to initialize server. Error: " << ok.error() << std::endl;
            return;
        }
        server.start();
    }
}


void AsynchUnixSocketServer_Poll_Debug::TestAll()
{
    runServer();
}