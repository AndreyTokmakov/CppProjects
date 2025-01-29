/**============================================================================
Name        : AsynchUnixSocketServer_Poll.cpp
Created on  : 29.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AsynchUnixSocketServer_Poll.cpp
============================================================================**/

#include "AsynchUnixSocketServer_Poll.h"

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

#define RESULT_SUCCESS   (0)
#define INVALID_HANDLE   (-1)
#define SOCKET_ERROR     (-1)
#define SERVER_SOCK_PATH "/tmp/unix_socket"

namespace
{
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
    
    std::string_view err2String(const int32_t errorCode)
    {
        switch (errorCode)
        {
            case EPERM: return "EPERM: Operation not permitted"sv;
            case ENOENT: return "ENOENT: No such file or directory"sv;
            case ESRCH: return "ESRCH: No such process"sv;
            case EINTR: return "EINTR: Interrupted system call"sv;
            case EIO: return "EIO: I/O error"sv;
            case ENXIO: return "ENXIO: No such device or address"sv;
            case E2BIG: return "E2BIG: Argument list too long"sv;
            case ENOEXEC: return "ENOEXEC: Exec format error"sv;
            case EBADF: return "EBADF: Bad file number"sv;
            case ECHILD: return "ECHILD: No child processes"sv;
            case EAGAIN: return "EAGAIN: Try again"sv;
            case ENOMEM: return "ENOMEM: Out of memory"sv;
            case EACCES: return "EACCES:  Permission denied"sv;
            case EFAULT: return "EFAULT: Bad address"sv;
            case ENOTBLK: return "ENOTBLK: Block device required"sv;
            case EBUSY: return "EBUSY: Device or resource busy"sv;
            case EEXIST: return "EEXIST: File exists"sv;
            case EXDEV: return "EXDEV: Cross-device link"sv;
            case ENODEV: return "ENODEV: No such device"sv;
            case ENOTDIR: return "ENOTDIR: Not a directory"sv;
            case EISDIR: return "EISDIR: Is a directory "sv;
            case EINVAL: return "EINVAL: Invalid argument"sv;
            case ENFILE: return "ENFILE: File table overflow"sv;
            case EMFILE: return "EMFILE: Too many open files"sv;
            case ENOTTY: return "ENOTTY: Not a typewriter"sv;
            case ETXTBSY: return "ETXTBSY: Text file busy "sv;
            case EFBIG: return "EFBIG: File too large"sv;
            case ENOSPC: return "ENOSPC: No space left on device"sv;
            case ESPIPE: return "ESPIPE: Illegal seek"sv;
            case EROFS: return "EROFS: Read-only file system"sv;
            case EMLINK: return "EMLINK: Too many links"sv;
            case EPIPE: return "EPIPE: Broken pipe"sv;
            case EDOM: return "EDOM: Math argument out of domain of func"sv;
            case ERANGE: return "ERANGE: Math result not representable"sv;
            default: return "Unknown"sv;
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


namespace AsynchUnixSocketServer_Poll::WorkingExample
{
    struct UDSAsynchServer
    {
        Socket serverSocket { INVALID_HANDLE };
        std::string filePath { SERVER_SOCK_PATH };

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

        // TODO: std::expected<R,E>
        bool init()
        {
            /*
            const int32_t on = 1;
            if (RESULT_SUCCESS != ::setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on))) {
                return error("setsockopt() failed");
            }

            if (RESULT_SUCCESS != ::ioctl(serverSocket, FIONBIO, (char*)&on)) {
                return error("ioctl() failed");
            }
            */

            if (SOCKET_ERROR == ::fcntl(serverSocket, F_SETFL, O_NONBLOCK)) {
                return error("fcntl() failed");
            }
            int32_t yes { 1 };
            if (SOCKET_ERROR == ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
                return error("setsockopt() failed");
            }


            const sockaddr_un serverAddr { .sun_family = AF_UNIX, .sun_path = SERVER_SOCK_PATH };
            uint32_t len = sizeof(serverAddr);
            if (RESULT_SUCCESS != ::bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddr), len)) {
                return error("bind() failed");
            }

            if (RESULT_SUCCESS != ::listen(serverSocket, 32)) {
                return error("listen() failed");
            }

            return true;
        }

        bool start()
        {
            std::array<pollfd, 200> fds {};
            std::array<char, 1024> buffer {};
            std::string message;

            fds[0].fd = serverSocket;
            fds[0].events = POLLIN;

            // FIXME
            int debugIterCount = 10;

            int32_t nfds = 1;
            const int32_t timeout = (3 * 60 * 1000);
            int32_t currentSize { 0 };
            while (true)
            {
                std::cout << "Waiting on poll()..." << std::endl;
                if (const int32_t result = ::poll(fds.data(), nfds, timeout); SOCKET_ERROR == result) {
                    return error("poll() failed ");
                } else if (0 == result) {
                    return error("poll() timeout ");
                }

                currentSize = nfds;
                for (int32_t idx = 0; idx < currentSize; idx++)
                {
                    if (0 == fds[idx].revents)
                        continue;
                    if (fds[idx].revents != POLLIN) {
                        return error("Error() revents = ", fds[idx].revents);
                    }
                    if (fds[idx].fd == serverSocket) /** Listening descriptor is readable. **/
                    {
                        std::cout << "Listening socket is readable" << std::endl;
                        Socket clientSocket { INVALID_HANDLE };
                        while (true)
                        {
                            clientSocket = ::accept(serverSocket, nullptr, nullptr);
                            if (SOCKET_ERROR == clientSocket)
                            {
                                if (errno != EWOULDBLOCK /*|| EAGAIN != errno*/) {
                                    return error("accept() failed = " + std::to_string(fds[idx].revents));
                                } else {
                                    std::cout << "Accept ==> EWOULDBLOCK" << std::endl;
                                    break;
                                }
                            }
                            else
                            {
                                std::cout << "New incoming connection. Client socket = " << clientSocket << std::endl;
                                fds[nfds].fd = clientSocket;
                                fds[nfds].events = POLLIN | POLLOUT;
                                nfds++;
                                break;
                            }
                        }
                        std::cout << "Accepted. clientSocket = " << clientSocket << std::endl;
                        if (0 == debugIterCount--) { return false; } // INFO - DEBUG
                    }
                    else
                    {
                        std::cout << "Descriptor " << fds[idx].fd << " is readable" << std::endl;
                        while (true)
                        {
                            if (0 == debugIterCount--) { return false; } // INFO - DEBUG
                            const int32_t bytesRead = ::recv(fds[idx].fd, buffer.data(), buffer.size(), 0);
                            if (SOCKET_ERROR == bytesRead)
                            {
                                if (errno != EWOULDBLOCK /*|| EAGAIN != errno*/) {
                                    return error("recv() failed = " + std::to_string(fds[idx].revents));
                                } else {
                                    std::cout << "recv ==> EWOULDBLOCK" << std::endl;
                                    break;
                                }
                            }
                            else if (0 == bytesRead)
                            {
                                std::cout << "Close connection for client = " << fds[idx].fd << std::endl;
                                // FIXME
                                ::close(fds[idx].fd);
                                fds[idx].fd = -1;

                                // FIXME
                                for (int i = 0; i < nfds; i++) {
                                    if (fds[i].fd == -1) {
                                        for(int j = i; j < nfds-1; j++) {
                                            fds[j].fd = fds[j+1].fd;
                                        }
                                        i--;
                                        nfds--;
                                    }
                                }

                                break;
                            }
                            else
                            {
                                message.assign(buffer.data(), bytesRead);
                                std::cout << message << std::endl;
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
        if (!server.init()) {
            std::cerr << "Failed to initialize server" << std::endl;
        }
        server.start();
    }
}

namespace AsynchUnixSocketServer_Poll::Debug_OK
{
    struct UDSAsynchServer
    {
        Socket serverSocket { INVALID_HANDLE };
        std::string filePath { SERVER_SOCK_PATH };

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

        // TODO: std::expected<R,E>
        bool init()
        {
            /*
            const int32_t on = 1;
            if (RESULT_SUCCESS != ::setsockopt(serverSocket,SOL_SOCKET,SO_REUSEADDR,(char*)&on,sizeof(on))) {
                return error("setsockopt() failed");
            }

            if (RESULT_SUCCESS != ::ioctl(serverSocket, FIONBIO, (char*)&on)) {
                return error("ioctl() failed");
            }
            */

            if (SOCKET_ERROR == ::fcntl(serverSocket, F_SETFL, O_NONBLOCK)) {
                return error("fcntl() failed");
            }
            int32_t yes { 1 };
            if (SOCKET_ERROR == ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
                return error("setsockopt() failed");
            }


            const sockaddr_un serverAddr { .sun_family = AF_UNIX, .sun_path = SERVER_SOCK_PATH };
            uint32_t len = sizeof(serverAddr);
            if (RESULT_SUCCESS != ::bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddr), len)) {
                return error("bind() failed");
            }

            if (RESULT_SUCCESS != ::listen(serverSocket, 32)) {
                return error("listen() failed");
            }

            return true;
        }

        bool start()
        {
            std::array<char, 1024> buffer {};
            std::string message;

            std::array<pollfd, 200> fds {};
            {
                fds[0].fd = serverSocket;
                fds[0].events = POLLIN;
            }
            int32_t nfds = 1;

            // FIXME: Move char to the end algo
            auto compact = [&]{
                auto it = std::partition(fds.begin() + 1, fds.end(), [](const auto& item) {
                    return -1 != item.fd;
                });
                nfds = std::distance(fds.begin(), it);
            };

            const int32_t timeout = (3 * 60 * 1000);
            int32_t currentSize { 0 };
            while (true)
            {
                // std::cout << "Waiting on poll()..." << std::endl;
                if (const int32_t result = ::poll(fds.data(), nfds, timeout); SOCKET_ERROR == result) {
                    return error("poll() failed ");
                } else if (0 == result) {
                    return error("poll() timeout ");
                }

                currentSize = nfds;
                for (int32_t idx = 0; idx < currentSize; idx++)
                {
                    if (0 == fds[idx].revents)
                        continue;
                    if (fds[idx].revents != POLLIN)
                    {
                        const Socket hSocket = fds[idx].fd;
                        if (fds[idx].revents & POLLHUP)
                        {
                            std::cout << "Close (POLLHUP) connection for client = " << hSocket << std::endl;
                            // FIXME
                            ::close(hSocket);
                            fds[idx].fd = -1;

                            compact();
                            break;
                        }
                        else
                        {
                            if (fds[idx].revents & POLLIN) std::cerr << "POLLIN " << hSocket<< std::endl;
                            if (fds[idx].revents & POLLPRI) std::cerr << "POLLPRI " << hSocket << std::endl;
                            if (fds[idx].revents & POLLOUT) std::cerr << "POLLOUT " << hSocket << std::endl;
                            if (fds[idx].revents & POLLRDNORM) std::cerr << "POLLRDNORM " << hSocket << std::endl;
                            if (fds[idx].revents & POLLWRNORM) std::cerr << "POLLWRNORM " << hSocket << std::endl;
                            if (fds[idx].revents & POLLWRBAND) std::cerr << "POLLWRBAND " << hSocket<< std::endl;
                            if (fds[idx].revents & POLLMSG) std::cerr << "POLLMSG " << hSocket << std::endl;
                            if (fds[idx].revents & POLLRDHUP) std::cerr << "POLLRDHUP " << hSocket << std::endl;
                            if (fds[idx].revents & POLLERR) std::cerr << "POLLERR " << hSocket << std::endl;
                            if (fds[idx].revents & POLLNVAL) std::cerr << "POLLNVAL " << hSocket<< std::endl;
                            // return error("Error() revents = ", fds[idx].revents);
                        }
                    }
                    else if (fds[idx].fd == serverSocket) /** Listening descriptor is readable. **/
                    {
                        std::cout << "Listening socket is readable" << std::endl;
                        Socket clientSocket { INVALID_HANDLE };
                        while (true)
                        {
                            clientSocket = ::accept(serverSocket, nullptr, nullptr);
                            if (SOCKET_ERROR == clientSocket)
                            {
                                if (errno != EWOULDBLOCK /*|| EAGAIN != errno*/) {
                                    return error("accept() failed = " + std::to_string(fds[idx].revents));
                                } else {
                                    // std::cout << "Accept ==> EWOULDBLOCK" << std::endl;
                                    break;
                                }
                            }
                            else
                            {
                                std::cout << "New incoming connection. Client socket = " << clientSocket << std::endl;

                                const int flags = ::fcntl(clientSocket, F_GETFL, 0);
                                if (SOCKET_ERROR == ::fcntl(clientSocket, F_SETFL, flags | O_NONBLOCK)) {
                                    return error("fcntl() failed for client = " + std::to_string(clientSocket));
                                }

                                fds[nfds].fd = clientSocket;
                                fds[nfds].events = POLLIN | POLLHUP;
                                nfds++;
                                break;
                            }
                        }
                        std::cout << "Accepted. clientSocket = " << clientSocket << std::endl;
                    }
                    else
                    {
                        const Socket clientSocket = fds[idx].fd;
                        // std::cout << "Descriptor " << clientSocket << " is readable" << std::endl;
                        while (true)
                        {
                            const int64_t bytesRead = ::recv(clientSocket, buffer.data(), buffer.size(), 0);
                            if (SOCKET_ERROR == bytesRead)
                            {
                                if (errno != EWOULDBLOCK /*|| EAGAIN != errno*/) {
                                    return error("recv() failed = " + std::to_string(fds[idx].revents));
                                } else {
                                    // std::cout << "recv ==> EWOULDBLOCK" << std::endl;
                                    break;
                                }
                            }
                            else if (0 == bytesRead)
                            {
                                std::cout << "Close connection for client = " << clientSocket << std::endl;
                                // FIXME
                                ::close(clientSocket);
                                fds[idx].fd = -1;

                                // FIXME
                                compact();
                                break;
                            }
                            else
                            {
                                message.assign(buffer.data(), bytesRead);
                                std::cout << clientSocket << " | " <<  message << std::endl;
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
        if (!server.init()) {
            std::cerr << "Failed to initialize server" << std::endl;
        }
        server.start();
    }
}

namespace AsynchUnixSocketServer_Poll::Debug
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

        bool setSocketToNonBlock(const Socket socket) const
        {
            const int flags = ::fcntl(socket, F_GETFL, 0);
            if (SOCKET_ERROR == ::fcntl(socket, F_SETFL, flags | O_NONBLOCK)) {
                return error("fcntl() failed for client = " + std::to_string(socket));
            }
            return false;
        }

        // TODO: std::expected<R,E>
        bool init()
        {
            if (setSocketToNonBlock(serverSocket)) {
                return error("setSocketToNonBlock() failed");
            }
            int32_t yes { 1 };
            if (SOCKET_ERROR == ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
                return error("setsockopt() failed");
            }

            const sockaddr_un serverAddr { .sun_family = AF_UNIX, .sun_path = SERVER_SOCK_PATH };
            uint32_t len = sizeof(serverAddr);
            if (RESULT_SUCCESS != ::bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddr), len)) {
                return error("bind() failed");
            }

            if (RESULT_SUCCESS != ::listen(serverSocket, 32)) {
                return error("listen() failed");
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
                    return error("poll() timeout ");
                }

                currentSize = handlesCount;
                for (int32_t idx = 0; idx < currentSize; idx++)
                {
                    if (0 == fds[idx].revents)
                        continue;
                    if (fds[idx].revents != POLLIN)
                    {
                        const Socket hSocket = fds[idx].fd;
                        std::cout << fds[idx] << std::endl;
                        if (fds[idx].revents & POLLHUP)
                        {
                            std::cout << "Close (POLLHUP) connection for client = " << hSocket << std::endl;
                            closeEvent(fds[idx]);
                            break;
                        }
                    }
                    else if (fds[idx].fd == serverSocket) /** Listening descriptor is readable. **/
                    {
                        Socket clientSocket { INVALID_HANDLE };
                        while (true)
                        {
                            clientSocket = ::accept(serverSocket, nullptr, nullptr);
                            if (SOCKET_ERROR == clientSocket)
                            {
                                if (errno != EWOULDBLOCK /*|| EAGAIN != errno*/) {
                                    return error("accept() failed = " + std::to_string(fds[idx].revents));
                                } else {
                                    // std::cout << "Accept ==> EWOULDBLOCK" << std::endl;
                                    break;
                                }
                            }
                            else
                            {
                                std::cout << "New incoming connection. Client socket = " << clientSocket << std::endl;
                                setSocketToNonBlock(clientSocket);

                                fds[handlesCount].fd = clientSocket;
                                fds[handlesCount].events = POLLIN | POLLHUP;
                                ++handlesCount;
                                break;
                            }
                        }
                    }
                    else
                    {
                        const Socket clientSocket = fds[idx].fd;
                        while (true)
                        {
                            const int64_t bytesRead = ::recv(clientSocket, buffer.data(), BUFFER_SIZE, 0);
                            if (SOCKET_ERROR == bytesRead)
                            {
                                if (errno != EWOULDBLOCK /*|| EAGAIN != errno*/) {
                                    return error("recv() failed = " + std::to_string(fds[idx].revents));
                                } else {
                                    // std::cout << "recv ==> EWOULDBLOCK" << std::endl;
                                    break;
                                }
                            }
                            else if (0 == bytesRead)
                            {
                                std::cout << "Close connection for client = " << clientSocket << std::endl;
                                closeEvent(fds[idx]);
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
        if (!server.init()) {
            std::cerr << "Failed to initialize server" << std::endl;
        }
        server.start();
    }
}

namespace AsynchUnixSocketServer_Poll::Perf
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

        bool setSocketToNonBlock(const Socket socket) const
        {
            const int flags = ::fcntl(socket, F_GETFL, 0);
            if (SOCKET_ERROR == ::fcntl(socket, F_SETFL, flags | O_NONBLOCK)) {
                return error("fcntl() failed for client = " + std::to_string(socket));
            }
            return false;
        }

        // TODO: std::expected<R,E>
        bool init()
        {
            if (setSocketToNonBlock(serverSocket)) {
                return error("setSocketToNonBlock() failed");
            }
            int32_t yes { 1 };
            if (SOCKET_ERROR == ::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))) {
                return error("setsockopt() failed");
            }

            const sockaddr_un serverAddr { .sun_family = AF_UNIX, .sun_path = SERVER_SOCK_PATH };
            uint32_t len = sizeof(serverAddr);
            if (RESULT_SUCCESS != ::bind(serverSocket, reinterpret_cast<const sockaddr*>(&serverAddr), len)) {
                return error("bind() failed");
            }

            if (RESULT_SUCCESS != ::listen(serverSocket, 32)) {
                return error("listen() failed");
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
            std::chrono::high_resolution_clock::time_point start;
            size_t counter { 0 }, bytesTotal { 0 };

            std::string message;
            uint32_t currentSize { 0 };
            while (true)
            {
                if (const int32_t result = ::poll(fds.data(), handlesCount, TIMEOUT); SOCKET_ERROR == result) {
                    return error("poll() failed ");
                } else if (0 == result) {
                    return error("poll() timeout ");
                }

                currentSize = handlesCount;
                for (uint32_t idx = 0; idx < currentSize; idx++)
                {
                    if (0 == fds[idx].revents)
                        continue;
                    if (fds[idx].revents != POLLIN)
                    {
                        const Socket hSocket = fds[idx].fd;
                        if (fds[idx].revents & POLLHUP)
                        {
                            // std::cout << "Close (POLLHUP) connection for client = " << hSocket << std::endl;
                            closeEvent(fds[idx]);

                            auto end = std::chrono::high_resolution_clock::now(); \
                            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \

                            std::cout << "Messages received: " << counter << ", Bytes: " << bytesTotal << std::endl;
                            std::cout << "Result: " << duration << " microseconds" << std::endl;

                            break;
                        }
                    }
                    else if (fds[idx].fd == serverSocket) /** Listening descriptor is readable. **/
                    {
                        Socket clientSocket { INVALID_HANDLE };
                        while (true)
                        {
                            clientSocket = ::accept(serverSocket, nullptr, nullptr);
                            if (SOCKET_ERROR == clientSocket)
                            {
                                if (errno != EWOULDBLOCK /*|| EAGAIN != errno*/) {
                                    return error("accept() failed = " + std::to_string(fds[idx].revents));
                                } else {
                                    // std::cout << "Accept ==> EWOULDBLOCK" << std::endl;
                                    break;
                                }
                            }
                            else
                            {
                                setSocketToNonBlock(clientSocket);
                                fds[handlesCount].fd = clientSocket;
                                fds[handlesCount].events = POLLIN | POLLHUP;
                                ++handlesCount;
                                break;
                            }
                        }
                    }
                    else
                    {
                        const Socket clientSocket = fds[idx].fd;
                        while (true)
                        {
                            const int64_t bytesRead = ::recv(clientSocket, buffer.data(), BUFFER_SIZE, 0);
                            if (0 == counter) {
                                start = std::chrono::high_resolution_clock::now();
                            }

                            ++counter;
                            ++bytesTotal += bytesRead;

                            if (SOCKET_ERROR == bytesRead)
                            {
                                if (errno != EWOULDBLOCK /*|| EAGAIN != errno*/) {
                                    return error("recv() failed = " + std::to_string(fds[idx].revents));
                                } else {
                                    // std::cout << "recv ==> EWOULDBLOCK" << std::endl;
                                    break;
                                }
                            }
                            else if (0 == bytesRead)
                            {
                                std::cout << "Close connection for client = " << clientSocket << std::endl;
                                closeEvent(fds[idx]);
                                break;
                            }
                            else
                            {
                                message.assign(buffer.data(), bytesRead);
                                // std::cout << clientSocket << " | " <<  message << std::endl;
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
        if (!server.init()) {
            std::cerr << "Failed to initialize server" << std::endl;
        }
        server.start();

        // Close (POLLHUP) connection for client = 4
        // Messages received: 1065766, Bytes: 1024606547
        // Result: 1273256 microseconds
    }
}

void AsynchUnixSocketServer_Poll::TestAll()
{
    // WorkingExample::runServer();
    // Debug_OK::runServer();

    Debug::runServer();

    // Perf::runServer();
}