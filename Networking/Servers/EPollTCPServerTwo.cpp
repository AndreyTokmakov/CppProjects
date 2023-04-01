/**============================================================================
Name        : EPollTCPServerTwo.cpp
Created on  : 31.03.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : EPollTCPServerTwo
============================================================================**/

#include "EPollTCPServerTwo.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cassert>

#include <iostream>
#include <string>
#include <thread>
#include <memory>
#include <functional>
#include <utility>


namespace EPollTCPServerTwo
{
    constexpr uint32_t kEpollWaitTime { 10 }; // epoll wait timeout 10 ms
    constexpr uint32_t kMaxEvents { 100 };    // epoll wait return max size
    constexpr int32_t INVALID_SOCKET { -1 };
    constexpr int32_t SOCKET_ERROR {-1 };

    struct Packet {
    public:
        Packet() = default;
        explicit Packet(std::string msg): msg {std::move( msg )} {}
        Packet(int fd, std::string  msg) : fd(fd),msg(std::move(msg)) {}

        int fd { INVALID_SOCKET };
        std::string msg;
    };

    struct SocketGuard final
    {
        const int handle { INVALID_SOCKET };
        explicit SocketGuard(int s): handle {s} { }

        ~SocketGuard() {
            ::close(handle);
        }
    };

    int Error(std::string_view text)
    {
        std::cerr << text << ". Error = " << errno << std::endl;
        return INVALID_SOCKET;
    }

    using PacketPtr = std::shared_ptr<Packet> ;
    using callback_recv_t = std::function<void(const PacketPtr& data)>;


    class EpollTcpServer final  {
    public:
        EpollTcpServer()                                       = default;
        EpollTcpServer(const EpollTcpServer& other)            = delete;
        EpollTcpServer& operator=(const EpollTcpServer& other) = delete;
        EpollTcpServer(EpollTcpServer&& other)                 = delete;
        EpollTcpServer& operator=(EpollTcpServer&& other)      = delete;
        ~EpollTcpServer();

        EpollTcpServer(std::string local_ip,
                       uint16_t local_port);

    public:
        bool Start();
        bool Stop();
        int32_t SendData(const PacketPtr& data);
        void RegisterOnRecvCallback(callback_recv_t callback);
        void UnRegisterOnRecvCallback();

    protected:
        int32_t CreateEpoll();
        int32_t CreateSocket();
        static int32_t MakeSocketNonBlock(int32_t fd);
        static int32_t Listen(int32_t listenfd);
        static int32_t UpdateEpollEvents(int efd, int op, int fd, int events);

        void OnSocketAccept();
        void OnSocketRead(int32_t fd);
        void OnSocketWrite(int32_t fd);
        void EpollLoop();


    private:
        std::string serverHost; // tcp local ip
        uint16_t serverPort {0 }; // tcp bind local port
        int32_t handle {-1 }; // listenfd
        int32_t efd_ { -1 }; // epoll fd
        std::shared_ptr<std::thread> th_loop_ { nullptr }; // one loop per thread(call epoll_wait in loop)
        bool loop_flag_ { true }; // if loop_flag_ is false, then exit the epoll loop
        callback_recv_t recv_callback_ { nullptr }; // callback when received
    };


    EpollTcpServer::EpollTcpServer(std::string  local_ip, uint16_t local_port)
            : serverHost {std::move(local_ip )},
              serverPort {local_port } {
    }

    EpollTcpServer::~EpollTcpServer() {
        Stop();
    }

    bool EpollTcpServer::Start() {
        if (CreateEpoll() < 0)
            return false;

        const int listenfd = CreateSocket();
        if (listenfd < 0) {
            return false;
        }

        if (int mr = MakeSocketNonBlock(listenfd); mr < 0) {
            return false;
        }

        if (int lr = Listen(listenfd);lr < 0) {
            return false;
        }

        std::cout << "EpollTcpServer Init success!" << std::endl;
        handle = listenfd;

        if (int er = UpdateEpollEvents(efd_, EPOLL_CTL_ADD, handle, EPOLLIN | EPOLLET); er < 0) {
            ::close(handle);
            return false;
        }

        assert(!th_loop_);

        th_loop_ = std::make_shared<std::thread>(&EpollTcpServer::EpollLoop, this);
        if (!th_loop_) {
            return false;
        }

        th_loop_->detach();
        return true;
    }


    bool EpollTcpServer::Stop() {
        loop_flag_ = false;
        ::close(handle);
        ::close(efd_);
        std::cout << "stop epoll!" << std::endl;
        UnRegisterOnRecvCallback();
        return true;
    }

    int32_t EpollTcpServer::CreateEpoll() {
        int epollFd = epoll_create(1);
        if (epollFd < 0) {
            std::cout << "epoll_create failed!" << std::endl;
            return -1;
        }
        efd_ = epollFd;
        return epollFd;
    }

    int32_t EpollTcpServer::CreateSocket()
    {
        // TODO: User guard
        const int sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock < 0) {
            std::cerr << "create socket " << serverHost << ":" << serverPort << " failed!" << std::endl;
            return SOCKET_ERROR;
        }

        sockaddr_in server {PF_INET, htons(serverPort), {.s_addr = inet_addr(serverHost.data())}, {}};
        if (SOCKET_ERROR == ::bind(sock, reinterpret_cast<sockaddr*>(&server), sizeof(server))) {
            std::cerr << "bind socket " << serverHost << ":" << serverPort << " failed!" << std::endl;
            ::close(sock);
            return SOCKET_ERROR;
        }
        std::cout << "create and bind socket " << serverHost << ":" << serverPort << " success!" << std::endl;
        return sock;
    }

    int32_t EpollTcpServer::MakeSocketNonBlock(int32_t fd) {
        const int flags = ::fcntl(fd, F_GETFL, 0);
        if (flags < 0) {
            return Error("fcntl failed!");
        }

        if (SOCKET_ERROR == ::fcntl(fd, F_SETFL, flags | O_NONBLOCK)) {
            return Error("fcntl failed!");
        }
        return 0;
    }

    int32_t EpollTcpServer::Listen(int32_t sock) {
        if (SOCKET_ERROR == ::listen(sock, SOMAXCONN)) {
            return Error("Failed to Listen() socket");
        }
        return 0;
    }

    // add/modify/remove a item(socket/fd) in epoll instance(rbtree), for this example, just add a socket to epoll rbtree
    int32_t EpollTcpServer::UpdateEpollEvents(int efd, int op, int fd, int events) {
        epoll_event ev {};
        memset(&ev, 0, sizeof(ev));
        ev.events = events;
        ev.data.fd = fd; // ev.data is a enum
        fprintf(stdout,"%s fd %d events read %d write %d\n", op == EPOLL_CTL_MOD ? "mod" : "add", fd, ev.events & EPOLLIN, ev.events & EPOLLOUT);
        if (int r = epoll_ctl(efd, op, fd, &ev); r < 0) {
            std::cerr << "epoll_ctl failed!" << std::endl;
            return SOCKET_ERROR;
        }
        return 0;
    }

    // handle accept event
    void EpollTcpServer::OnSocketAccept() {
        // epoll working on et mode, must read all coming data, so use a while loop here
        while (true) {
            sockaddr_in clientAddr {};
            socklen_t addLen { sizeof(clientAddr) };

            // accept a new connection and get a new socket
            int cli_fd = ::accept(handle, reinterpret_cast<sockaddr*>(&clientAddr), &addLen);
            if (cli_fd == -1) {
                if ( (errno == EAGAIN) || (errno == EWOULDBLOCK) ) {
                    // read all accept finished(epoll et mode only trigger one time,so must read all data in listen socket)
                    std::cout << "accept all coming connections!" << std::endl;
                    break;
                } else {
                    std::cout << "accept error!" << std::endl;
                    continue;
                }
            }

            // TODO: Refactor!!! This is to Slow!
            /*
            sockaddr_in peer {};
            socklen_t p_len = sizeof(peer);
            if (int r = getpeername(cli_fd, reinterpret_cast<sockaddr*>(&peer), &p_len);r < 0) {
                std::cerr << "getpeername error!" << std::endl;
                continue;
            }
            */

            // std::cout << "accpet connection from " << inet_ntoa(clientAddr.sin_addr) << std::endl;


            if (int mr = MakeSocketNonBlock(cli_fd); mr < 0) {
                ::close(cli_fd);
                continue;
            }

            if (int er = UpdateEpollEvents(efd_, EPOLL_CTL_ADD, cli_fd, EPOLLIN | EPOLLRDHUP | EPOLLET);er < 0 ) {
                // if something goes wrong, close this new socket
                ::close(cli_fd);
                continue;
            }
        }
    }

    // register a callback when packet received
    void EpollTcpServer::RegisterOnRecvCallback(callback_recv_t callback) {
        assert(!recv_callback_);
        recv_callback_ = callback;
    }

    void EpollTcpServer::UnRegisterOnRecvCallback() {
        assert(recv_callback_);
        recv_callback_ = nullptr;
    }

    // handle read events on fd
    void EpollTcpServer::OnSocketRead(int32_t fd) {
        char read_buf[4096];
        bzero(read_buf, sizeof(read_buf));
        int n = -1;


        std::cout << "OnSocketRead()" << std::endl;
        // epoll working on et mode, must read all data
        while ( (n = ::read(fd, read_buf, sizeof(read_buf))) > 0) {
            // callback for recv
            std::cout << "fd: " << fd <<  " recv: " << read_buf << std::endl;
            std::string msg(read_buf, n);
            // create a recv packet
            PacketPtr data = std::make_shared<Packet>(fd, msg);
            std::cout << "Data: " << msg << std::endl;
            if (recv_callback_) {
                // handle recv packet
                recv_callback_(data);
            }
        }
        if (-1 == n) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // read all data finished
                return;
            }
            // something goes wrong for this fd, should close it
            ::close(fd);
            return;
        }
        if (0 == n) {
            // this may happen when client close socket. EPOLLRDHUP usually handle this, but just make sure; should close this fd
            ::close(fd);
            return;
        }
    }

    void EpollTcpServer::OnSocketWrite(int32_t fd) {
        std::cout << "fd: " << fd << " writeable!" << std::endl;
    }

    // send packet
    int32_t EpollTcpServer::SendData(const PacketPtr& data) {
        if (-1 == data->fd)
            return -1;

        int r = ::write(data->fd, data->msg.data(), data->msg.size());
        if (-1 == r) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return -1;
            }
            // error happend
            ::close(data->fd);
            std::cout << "fd: " << data->fd << " write error, close it!" << std::endl;
            return -1;
        }
        std::cout << "fd: " << data->fd << " write size: " << r << " ok!" << std::endl;
        return r;
    }

    void EpollTcpServer::EpollLoop() {
        epoll_event* alive_events = static_cast<epoll_event*>(calloc(kMaxEvents, sizeof(epoll_event)));
        if (!alive_events) {
            std::cout << "calloc memory failed for epoll_events!" << std::endl;
            return;
        }

        while (loop_flag_)
        {
            int num = epoll_wait(efd_, alive_events, kMaxEvents, kEpollWaitTime);
            for (int i = 0; i < num; ++i) {
                // get fd
                int fd = alive_events[i].data.fd;
                // get events(readable/writeable/error)
                int events = alive_events[i].events;

                if ( (events & EPOLLERR) || (events & EPOLLHUP) ) {
                    std::cout << "epoll_wait error!" << std::endl;
                    // An error has occured on this fd, or the socket is not ready for reading (why were we notified then?).
                    ::close(fd);
                } else if (events & EPOLLRDHUP) {
                    // Stream socket peer closed connection, or shut down writing half of connection.
                    // more inportant, We still to handle disconnection when read()/recv() return 0 or -1 just to be sure.
                    std::cout << "fd:" << fd << " closed EPOLLRDHUP!" << std::endl;
                    // close fd and epoll will remove it
                    ::close(fd);
                } else if (events & EPOLLIN){
                    std::cout << "epollin" << std::endl;
                    if (fd == handle) {
                        // listen fd coming connections
                        OnSocketAccept();
                    } else {
                        // other fd read event coming, meaning data coming
                        OnSocketRead(fd);
                    }
                } else if (events & EPOLLOUT)  {
                    std::cout << "epollout" << std::endl;
                    // write event for fd (not including listen-fd), meaning send buffer is available for big files
                    OnSocketWrite(fd);
                } else {
                    std::cerr << "unknown epoll event!" << std::endl;
                }
            } // end for (int i = 0; ...

        } // end while (loop_flag_)
        free(alive_events);
    }
};

[[noreturn]]
void EPollTCPServerTwo::Tests()
{
    const std::string local_ip {"0.0.0.0"};
    constexpr uint16_t local_port { 52525 };

    const auto epoll_server = std::make_shared<EpollTcpServer>(local_ip, local_port);
    if (!epoll_server) {
        std::cerr << "tcp_server create field" << std::endl;
        exit(-1);
    }

    auto recv_call = [&](const PacketPtr& data) -> void {
        epoll_server->SendData(data);
        return;
    };

    epoll_server->RegisterOnRecvCallback(recv_call);
    if (!epoll_server->Start()) {
        std::cerr << "tcp_server start failed!" << std::endl;
        exit(1);
    }

    std::cout << "* * * * * tcp_server started * * * * *" << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    epoll_server->Stop();
}