/**============================================================================
Name        : TcpClientEpoll_Experimental.cpp
Created on  :
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "DateTimeUtilities.hpp"
#include "Clients.hpp"

#include <iostream>
#include <print>
#include <syncstream>

#include <coroutine>
#include <utility>
#include <unordered_map>
#include <vector>
#include <stdexcept>

#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


#define LOG std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << ' '

struct TcpClient
{
    using Socket = int32_t;
    using Port   = uint16_t;

    constexpr static Socket InvalidSocket { -1 };
    constexpr static uint16_t MaxEvents { 16 };


    TcpClient() = default;

    ~TcpClient()
    {
        if (socket != InvalidSocket) {
            ::close(socket);
        }
        if (epollFd != InvalidSocket) {
            ::close(epollFd);
        }
    }

    bool connectTo(const std::string& host, const Port port)
    {
        socket = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (InvalidSocket == socket) {
            std::cerr << "Failed to create socket. Error = " << errno << std::endl;
            return false;
        }

        setNonBlocking(socket);
        const sockaddr_in server { PF_INET, htons(port) ,
            {.s_addr = inet_addr(host.data())}, {}
        };

        const int res = ::connect(socket, reinterpret_cast<const sockaddr*>(&server), sizeof(server));
        if (res == 0) {
            state = State::Connected;
        } else if (errno == EINPROGRESS) {
            state = State::Connecting;
        } else {
            perror("connect");
            return false;
        }

        epollFd = ::epoll_create1(0);
        if (epollFd < 0) {
            perror("epoll_create1");
            return false;
        }

        updateEpoll(EPOLLIN | EPOLLOUT | EPOLLERR);

        return true;
    }

    void send(const std::string& data)
    {
        writeBuffer.insert(writeBuffer.end(), data.begin(), data.end());
        updateEpoll(EPOLLIN | EPOLLOUT | EPOLLERR);
    }

    void run()
    {
        epoll_event events[MaxEvents];
        // std::array<epoll_data, MaxEvents> events {};

        while (state != State::Closed) {
            int n = epoll_wait(epollFd, events, MaxEvents, -1);
            if (n < 0) {
                if (errno == EINTR) continue;
                perror("epoll_wait");
                break;
            }

            for (int i = 0; i < n; ++i) {
                handleEvent(events[i].events);
            }
        }
    }

private:

    enum class State {
        Connecting,
        Connected,
        Closed
    };

    void handleEvent(const uint32_t events)
    {
        if ((events & EPOLLERR) != 0U) {
            handleError();
            return;
        }
        if (state == State::Connecting && (events & EPOLLOUT)) {
            handleConnect();
        }
        if (state == State::Connected) {
            if (events & EPOLLIN) {
                handleRead();
            }
            if (events & EPOLLOUT) {
                handleWrite();
            }
        }
    }

    void handleConnect()
    {
        int err = 0;
        socklen_t len = sizeof(err);

        if (getsockopt(socket, SOL_SOCKET, SO_ERROR, &err, &len) < 0 || err != 0) {
            std::cerr << "connect failed: " << strerror(err) << "\n";
            state = State::Closed;
            return;
        }

        std::cout << "Connected!\n";
        state = State::Connected;
    }

    void handleRead()
    {
        std::array<char, 4096> buf {};
        while (true)
        {
            const ssize_t bytes = recv(socket, buf.data(), sizeof(buf), 0);
            if (bytes > 0) {
                readBuffer.insert(readBuffer.end(), buf.data(), buf.data() + bytes);
            } else if (bytes == 0) {
                std::cout << "Server closed connection\n";
                state = State::Closed;
                return;
            } else {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                }
                perror("recv");
                state = State::Closed;
                return;
            }
        }

        onMessage();
    }

    void handleWrite()
    {
        while (!writeBuffer.empty())
        {
            const ssize_t bytes = ::send(socket, writeBuffer.data(), writeBuffer.size(), 0);
            if (bytes > 0) {
                writeBuffer.erase(writeBuffer.begin(), writeBuffer.begin() + bytes);
            } else
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                }
                perror("send");
                state = State::Closed;
                return;
            }
        }

        // если больше нечего писать — убираем EPOLLOUT
        if (writeBuffer.empty()) {
            updateEpoll(EPOLLIN | EPOLLERR);
        }
    }

    void handleError()
    {
        std::cerr << "epoll error\n";
        state = State::Closed;
    }

    void onMessage()
    {
        const std::string msg(readBuffer.begin(), readBuffer.end());
        readBuffer.clear();
        std::cout << "Received: " << msg << "\n";
    }

    void updateEpoll(const uint32_t events)
    {
        epoll_event ev { .events = events, .data = epoll_data_t { .ptr = this } };
        if (!registered) {
            ::epoll_ctl(epollFd, EPOLL_CTL_ADD, socket, &ev);
            registered = true;
        } else {
            ::epoll_ctl(epollFd, EPOLL_CTL_MOD, socket, &ev);
        }
    }

    static void setNonBlocking(const int fd)
    {
        const int flags = ::fcntl(fd, F_GETFL, 0);
        ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
    }

private:
    Socket socket { InvalidSocket };
    Socket epollFd { InvalidSocket };

    bool registered { false };
    State state { State::Closed };

    std::vector<char> readBuffer;
    std::vector<char> writeBuffer;
};


void Clients::TcpAsyncClientEpoll::TestAll()
{
    TcpClient client;
    if (!client.connectTo("0.0.0.0", 52525)) {
        return;
    }
    client.send("Hello from client\n");
    client.run();
}
