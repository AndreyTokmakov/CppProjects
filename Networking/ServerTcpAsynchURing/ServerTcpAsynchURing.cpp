/**============================================================================
Name        : ServerTcpAsynchURing.cpp
Created on  : 19.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ServerTcpAsynchURing.cpp
============================================================================**/

#include "ServerTcpAsynchURing.hpp"

#include <iostream>
#include <cstring>
#include <print>

#include <liburing.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace minimal_example_1
{
    constexpr static uint16_t backlogSize { 512 };
    constexpr static uint16_t queueDepth { 1024 };
    constexpr static uint16_t bufferSize { 4096 };

    enum class OpType  {
        Accept,
        Read,
        Write
    };

    struct Connection
    {
        int fd;
        char buffer[bufferSize];
    };

    struct Request
    {
        OpType type;
        Connection* conn;
    };

    int createListenSocket(const uint16_t port)
    {
        const int socketFd = socket(AF_INET, SOCK_STREAM, 0);
        const sockaddr_in address { AF_INET, htons(port), {.s_addr = INADDR_ANY}, {}};

        ::bind(socketFd, reinterpret_cast<const sockaddr*>(&address),sizeof(address));
        ::listen(socketFd, backlogSize);

        return socketFd;
    }

    void run(const uint16_t port = 52525)
    {
        io_uring ring{};
        io_uring_queue_init(queueDepth, &ring, 0);

        const int serverFd = createListenSocket(port);
        auto submit_accept = [&]( )
        {
            auto* req = new Request{OpType::Accept, nullptr};

            io_uring_sqe* sqe = io_uring_get_sqe(&ring);
            io_uring_prep_accept(sqe, serverFd, nullptr, nullptr, 0);
            io_uring_sqe_set_data(sqe, req);
        };

        submit_accept();
        io_uring_submit(&ring);

        while (true)
        {
            io_uring_cqe* cqe;
            io_uring_wait_cqe(&ring, &cqe);

            auto* req = static_cast<Request *>(io_uring_cqe_get_data(cqe));
            if (req->type == OpType::Accept)
            {
                if (const int client_fd = cqe->res; client_fd >= 0)
                {
                    Connection* conn = new Connection{client_fd};

                    Request* read_req = new Request{OpType::Read, conn};
                    io_uring_sqe* sqe = io_uring_get_sqe(&ring);
                    io_uring_prep_recv(sqe, client_fd, conn->buffer, bufferSize, 0);
                    io_uring_sqe_set_data(sqe, read_req);
                }

                submit_accept();
            }
            else if (req->type == OpType::Read)
            {
                if (cqe->res <= 0)
                {
                    close(req->conn->fd);
                    delete req->conn;
                    delete req;
                }
                else
                {
                    const int bytes = cqe->res;
                    Request* write_req = new Request{OpType::Write, req->conn};
                    io_uring_sqe* sqe = io_uring_get_sqe(&ring);
                    io_uring_prep_send(sqe, req->conn->fd, req->conn->buffer, bytes, 0);
                    io_uring_sqe_set_data(sqe, write_req);
                    delete req;
                }
            }
            else if (req->type == OpType::Write)
            {
                Connection* conn = req->conn;
                auto* read_req = new Request{OpType::Read, conn};
                io_uring_sqe* sqe = io_uring_get_sqe(&ring);
                io_uring_prep_recv(
                    sqe,
                    conn->fd,
                    conn->buffer,
                    bufferSize,
                    0
                );
                io_uring_sqe_set_data(sqe, read_req);
                delete req;
            }

            io_uring_cqe_seen(&ring, cqe);
            io_uring_submit(&ring);
        }

        io_uring_queue_exit(&ring);
        ::close(serverFd);
    }
}

namespace minimal_example_2
{
    struct Server
    {
        using Handle = int;
        constexpr static Handle invalidHandle { -1 };

        constexpr static uint16_t backlogSize { 512 };
        constexpr static uint16_t queueDepth { 1024 };
        constexpr static uint16_t bufferSize { 4096 };

        enum class Operation {
            Accept,
            Read,
            Write
        };

        struct RequestContext
        {
            Operation opType { Operation::Accept };
            Handle socketFd { invalidHandle } ;
            size_t dataSize{};
            char buffer[bufferSize]{};
        };

        Server() {
            io_uring_queue_init(queueDepth, &ringContext, 0);
        }

        ~Server() {
            io_uring_queue_exit(&ringContext);
        }

        void run(const uint16_t port)
        {
            setupListenSocket(port);
            submitAccept();
            while (true)
            {
                io_uring_submit(&ringContext);
                io_uring_cqe* completion;
                if (const int result = io_uring_wait_cqe(&ringContext, &completion); result < 0)
                    continue;

                const auto* context = static_cast<RequestContext*>(io_uring_cqe_get_data(completion));
                handleCompletion(completion, context);
                io_uring_cqe_seen(&ringContext, completion);
            }
        }

    private:

        io_uring ringContext{};
        Handle listenSocketFd { invalidHandle };

        bool setupListenSocket(const uint16_t port)
        {
            listenSocketFd = ::socket(AF_INET, SOCK_STREAM, 0);
            if (listenSocketFd < 0) {
                std::print(std::cerr, "Failed to create server socker. Error = {}", errno);
                return false;
            }

            const sockaddr_in address { AF_INET, htons(port), {.s_addr = INADDR_ANY}, {}};

            if (0 != ::bind(listenSocketFd, reinterpret_cast<const sockaddr*>(&address),sizeof(address))) {
                std::print(std::cerr, "Failed to bind server socket to port {}. Error = {}", port, errno);
                return false;
            }
            if (0 != ::listen(listenSocketFd, backlogSize)) {
                std::print(std::cerr, "listen() failed. Error = {}", port, errno);
                return false;
            }

            return true;
        }

        void submitAccept()
        {
            RequestContext* context = new RequestContext {
                .opType = Operation::Accept,
                .socketFd = listenSocketFd
            };
            io_uring_sqe* submission = io_uring_get_sqe(&ringContext);
            io_uring_prep_accept(submission, listenSocketFd, nullptr, nullptr, 0);
            io_uring_sqe_set_data(submission, context);
        }

        void submitRead(const int clientFd)
        {
            RequestContext* context = new RequestContext {
                .opType = Operation::Read,
                .socketFd = clientFd
            };
            io_uring_sqe* submission = io_uring_get_sqe(&ringContext);
            io_uring_prep_recv(submission, clientFd, context->buffer, bufferSize, 0);
            io_uring_sqe_set_data(submission, context);
        }

        void submitWrite(const int clientFd,
                         const char* data,
                         const size_t size)
        {
            RequestContext* context = new RequestContext {
                .opType = Operation::Write,
                .socketFd = clientFd,
                .dataSize = size
            };
            std::memcpy(context->buffer, data, size);

            io_uring_sqe* submission = io_uring_get_sqe(&ringContext);
            io_uring_prep_send(submission, clientFd, context->buffer, size, 0);
            io_uring_sqe_set_data(submission, context);
        }

        void handleCompletion(const io_uring_cqe* completion,
                              const RequestContext* context)
        {
            if (completion->res < 0) {
                close(context->socketFd);
                delete context;
                return;
            }

            switch (context->opType)
            {
                case Operation::Accept:
                {
                    const Handle clientFd = completion->res;
                    submitAccept();
                    submitRead(clientFd);
                    break;
                }
                case Operation::Read:
                {
                    if (completion->res == 0) {
                        close(context->socketFd);
                    } else {
                        submitWrite(context->socketFd, context->buffer, completion->res);
                    }
                    break;
                }
                case Operation::Write:
                {
                    submitRead(context->socketFd);
                    break;
                }
            }
            delete context;
        }
    };

    void run()
    {
        Server server;
        server.run(52525);
    }

}

void server_tcp_asynch_uring::TestAll()
{
    // minimal_example_1::run();
    minimal_example_2::run();
}