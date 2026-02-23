/**============================================================================
Name        : ServerTcpAsynchEPoll.cpp
Created on  : 20.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ServerTcpAsynchEPoll.cpp
============================================================================**/

#include "ServerTcpAsynchEPoll.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <stdexcept>

namespace
{
    using Handle = int;
    constexpr Handle invalidHandle { -1 };

    struct alignas(64) UltraRingBuffer
    {
        explicit UltraRingBuffer(const size_t initCapacity):
                capacity { initCapacity },
                mask { initCapacity - 1 }
        {
            if (initCapacity == 0 || (initCapacity & (initCapacity - 1)) != 0) {
                throw std::runtime_error("Capacity must be power of two");
            }

            void* ptr = nullptr;
            if (posix_memalign(&ptr, 64, capacity) != 0) {
                throw std::bad_alloc();
            }
            buffer_ = static_cast<char*>(ptr);
        }

        ~UltraRingBuffer() {
            std::free(buffer_);
        }

        UltraRingBuffer(const UltraRingBuffer&) = delete;
        UltraRingBuffer& operator=(const UltraRingBuffer&) = delete;

        [[nodiscard]]
        size_t size() const noexcept {
            return tail - head;
        }

        [[nodiscard]]
        size_t freeSpace() const noexcept {
            return capacity - size();
        }

        [[nodiscard]]
        bool empty() const noexcept {
            return head == tail;
        }

        ssize_t recvFromFd(const Handle fd) noexcept
        {
            const size_t free = freeSpace();
            if (free == 0)
                return 0;

            const size_t tailIndex = tail & mask;
            const size_t firstChunk = std::min(free, capacity - tailIndex);

            iovec iov[2];
            iov[0].iov_base = buffer_ + tailIndex;
            iov[0].iov_len = firstChunk;

            int iovCount = 1;
            if (firstChunk < free) {
                iov[1].iov_base = buffer_;
                iov[1].iov_len = free - firstChunk;
                iovCount = 2;
            }

            const ssize_t received = readv(fd, iov, iovCount);
            if (received > 0) {
                tail += static_cast<size_t>(received);
            }
            return received;
        }

        ssize_t sendToFd(const Handle fd) noexcept
        {
            if (empty())
                return 0;

            const size_t headIndex = head & mask;
            const size_t total = size();
            const size_t firstChunk = std::min(total, capacity - headIndex);

            iovec iov[2];
            iov[0].iov_base = buffer_ + headIndex;
            iov[0].iov_len = firstChunk;

            int iovCount = 1;
            if (firstChunk < total) {
                iov[1].iov_base = buffer_;
                iov[1].iov_len = total - firstChunk;
                iovCount = 2;
            }

            const ssize_t sent = writev(fd, iov, iovCount);
            if (sent > 0) {
                head += static_cast<size_t>(sent);
            }
            return sent;
        }

        [[nodiscard]]
        const char* data() const noexcept {
            return buffer_ + (head & mask);
        }

        [[nodiscard]]
        size_t contiguousSize() const noexcept
        {
            const size_t headIndex = head & mask;
            return std::min(size(), capacity - headIndex);
        }

        void consume(const size_t len) noexcept {
            head += len;
        }

        // ================= Write copy =================

        void write(const char* data, size_t len) noexcept
        {
            if (const size_t free = freeSpace(); len > free)
                len = free;

            const size_t tailIndex = tail & mask;
            const size_t firstChunk = std::min(len, capacity - tailIndex);

            std::memcpy(buffer_ + tailIndex, data, firstChunk);
            if (const size_t secondChunk = len - firstChunk; secondChunk) {
                std::memcpy(buffer_, data + firstChunk, secondChunk);
            }
            tail += len;
        }

    private:
        const size_t capacity;
        const size_t mask;
        alignas(64) uint64_t head { 0 };
        alignas(64) uint64_t tail { 0 };
        char* buffer_ = nullptr;
    };

    struct Connection
    {
        Handle fd { invalidHandle };
        UltraRingBuffer rx {1 << 16 };
        UltraRingBuffer tx {1 << 16 };

        explicit Connection(const Handle handle): fd { handle } {
        }
    };

    Handle setNonBlocking(const Handle fd)
    {
        const Handle flags = ::fcntl(fd, F_GETFL, 0);
        return ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

    Handle setTcpOptions(const Handle fd)
    {
        constexpr int one = 1;
        return ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one));
    }

    struct EpollServer
    {
        constexpr static uint16_t maxEvents = 1024;

        explicit EpollServer(const uint16_t port)
        {
            serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);
            if (serverSocket < 0) {
                throw std::runtime_error("socket failed");
            }

            constexpr int one = 1;
            setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
            setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one));

            const sockaddr_in address { AF_INET, htons(port), {.s_addr = INADDR_ANY}, {}};
            if (::bind(serverSocket, reinterpret_cast<const sockaddr*>(&address),sizeof(address)) < 0)
                throw std::runtime_error("bind failed");
            if (::listen(serverSocket, 1024) < 0)
                throw std::runtime_error("listen failed");

            setNonBlocking(serverSocket);

            epollFd = ::epoll_create1(0);

            epoll_event ev { .events = EPOLLIN, .data = epoll_data {.fd = serverSocket}};
            ::epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &ev);
        }

        void run()
        {
            std::array<epoll_event, maxEvents> events {};
            while (true)
            {
                const int n = ::epoll_wait(epollFd, events.data(), maxEvents, -1);
                for (int i = 0; i < n; ++i) {
                    if (events[i].data.fd == serverSocket) {
                        acceptLoop();
                    } else {
                        handleConnection(events[i]);
                    }
                }
            }
        }

    private:

        void acceptLoop()
        {
            while (true)
            {
                const Handle clientFd = ::accept4(serverSocket, nullptr, nullptr, SOCK_NONBLOCK);
                if (clientFd < 0) {
                    break;
                }
                setTcpOptions(clientFd);

                Connection* conn = new Connection { clientFd };
                connectionsTable[clientFd] = conn;

                epoll_event ev{};
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = clientFd;
                ::epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &ev);
            }
        }

        void handleConnection(const epoll_event& event)
        {
            const Handle fd = event.data.fd;
            Connection* conn = connectionsTable[fd];

            if (event.events & EPOLLIN)
            {
                while (true) {
                    if (const ssize_t r = conn->rx.recvFromFd(fd); r <= 0) {
                        break;
                    }
                }

                // Echo example (ultra fast)
                if (const size_t available = conn->rx.size(); available > 0)
                {
                    const size_t chunk = conn->rx.size();
                    conn->tx.write(conn->rx.data(), chunk);
                    conn->rx.consume(chunk);
                    enableWrite(fd);
                }
            }

            if (event.events & EPOLLOUT)
            {
                std::cout << "EPOLLOUT\n";

                while (true) {
                    if (const ssize_t s = conn->tx.sendToFd(fd); s <= 0) {
                        break;
                    }
                }
                if (conn->tx.empty()) {
                    disableWrite(fd);
                }
            }

            if (event.events & EPOLLHUP || event.events & EPOLLRDHUP)
            {
                std::cout << "EPOLLHUP/EPOLLRDHUP\n";

                Connection* conn = connectionsTable[fd];
                connectionsTable.erase(fd);
                delete conn;
                removeEvent(fd);
            }
        }

        void enableWrite(const Handle fd) const
        {
            epoll_event ev{};
            ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
            ev.data.fd = fd;
            ::epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev);
        }

        void disableWrite(const Handle fd) const
        {
            epoll_event ev{};
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = fd;
            ::epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &ev);
        }

        void removeEvent(const Handle fd) const
        {
            ::epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr);
        }

    private:

        Handle serverSocket { invalidHandle };
        Handle epollFd { invalidHandle };
        std::unordered_map<Handle, Connection*> connectionsTable;
    };

    void run(uint16_t port = 52525)
    {
        try {
            EpollServer server(port);
            std::cout << "Server started on port " << port << std::endl;
            server.run();
        } catch (const std::exception& ex) {
            std::cerr << "Fatal: " << ex.what() << "\n";
        }
    }
}

void server_tcp_asynch_epoll::TestAll()
{
    run();
}
