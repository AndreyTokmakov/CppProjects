//============================================================================
// Name        : Servers.h
// Created on  : 05.06.2022.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Servers
//============================================================================

#include "Servers.h"

#include <iostream>
#include <csignal>
#include <poll.h>
#include <cerrno>
#include <netinet/in.h> /* Internet address family, see ip(7) and socket(7)    */
#include <csignal>     /* signal, sigaction                                   */
#include <cstdio>
#include <cstdlib>
#include <string>
#include <sys/socket.h>
#include <sys/types.h> /* socket, ssize_t, etc.                                */
#include <unistd.h>

#define UNUSED_FD -1

namespace Servers
{
    void server1()
    {
        constexpr size_t RECV_BUFFER_SIZE {1024};
        constexpr size_t OBSERVED_FDS_POLL {64};

        [[maybe_unused]]
        constexpr size_t MAX_EVENTS {16};

        // struct sigaction action {.sa_handler = SIG_IGN};
        // sigaction(SIGPIPE, &action, nullptr);

        constexpr int port = 52525;
        sockaddr_in socket_address {};
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons(port);
        socket_address.sin_addr.s_addr = htonl(INADDR_ANY);

        /* Create a socket for internet communication */
        int hSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (-1 == hSocket) {
            std::cout << "socket() failed. Error = " << errno << std::endl;
            return;
        }

        int value {1};
        int result = setsockopt(hSocket, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int));
        if (-1 == result) {
            std::cout << "setsockopt() failed. Error = " << errno << std::endl;
            // TODO: Exit! Close socket!
        }

        result = ::bind(hSocket, reinterpret_cast<sockaddr*>(&socket_address), sizeof(socket_address));
        if (-1 == result) {
            std::cout << "Failed to bind socket. Error = " << errno << std::endl;
            // TODO: Exit! Close socket!
        } else {
            std::cout << "Socket is bind to 127.0.0.1:" << port << std::endl;
        }

        result = ::listen(hSocket, 128);  // TODO: 128 to const
        if (-1 == result) {
            std::cout << "Failed to listen socket. Error = " << errno << std::endl;
            // TODO: Exit! Close socket!
        } else {
            std::cout << "Listening socket" << std::endl;
        }

        /* Set server's socket as the first element */
        pollfd fds[OBSERVED_FDS_POLL] {{.fd = hSocket, .events = POLLIN, .revents = {}}};
        for (size_t i = 1; i < OBSERVED_FDS_POLL; ++i) {
            fds[i].events = POLLIN;
            fds[i].fd = UNUSED_FD;
        }


        while (poll(fds, OBSERVED_FDS_POLL, -1) > 0)
        {
            for (size_t i = 0; i < OBSERVED_FDS_POLL; ++i) {
                pollfd* polled_fd = fds + i;
                if (!(polled_fd->revents & POLLIN)) {
                    if (polled_fd->revents != 0) {
                        /*
                         * In this app we're not interested in other events, however if such occurres,
                         * let's log it to the console. What could be expected here? If for instance,
                         * we did not handle the client's socket file descriptor properly
                         * when connection is terminated (i.e. file descriptor has been closed),
                         * we might the POLLNVAL error. See poll(2) for more information.
                         */
                        printf("Event (%ld): %d on fd: %d\n", i, polled_fd->revents, polled_fd->fd);
                    }

                    /* If reading from a file descriptor is not possible, then do nothing. */
                    continue;
                }

                if (i == 0) { /* the server's socket */
                    printf("Accepting new incoming connection\n");
                    /* polled_fd.fd in this case is quivalet to sockfd */
                    int clientSocket = ::accept(polled_fd->fd, nullptr, nullptr);

                    /*
                     * Now we'd like to add client's file descriptor to the fds array.
                     * To do so we have to find the first element of fds array which has
                     * the fd property not set to an actual file descriptor (i.e. is NULL)
                     */
                    int free_fd_idx;
                    for (free_fd_idx = 0; fds[free_fd_idx].fd > UNUSED_FD; ++free_fd_idx)
                        ;
                    fds[free_fd_idx].fd = clientSocket;
                } else {
                    printf("Receiving data from the client with socket fd: %d\n", polled_fd->fd);

                    char buffer[RECV_BUFFER_SIZE];
                    ssize_t recv_buff_len = read(polled_fd->fd, buffer, sizeof(buffer));
                    if (recv_buff_len > 0) {
                        printf("Received message: ");
                        ::fwrite(buffer, recv_buff_len, 1, stdout);

                        constexpr std::string_view helloMessage { "Hello!!!" };
                        ::write(polled_fd->fd, helloMessage.data(), helloMessage.size());
                    }

                    if (recv_buff_len < 1) {
                        printf("Closing connection with client with socket fd: %d\n", polled_fd->fd);
                        ::close(polled_fd->fd);
                        polled_fd->fd = UNUSED_FD;
                    }
                }
            }
        }

        result = ::close(hSocket);
        if (-1 == result) {
            std::cout << "Failed to close socket" << std::endl;
        } else {
            std::cout << "Socket is closed" << std::endl;
        }
    }
};

void Servers::TestAll()
{
    server1();
};