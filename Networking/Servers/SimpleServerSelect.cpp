/**============================================================================
Name        : SimpleServerSelect.h
Created on  : 14.03.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SimpleServerSelect
============================================================================**/

#include "SimpleServerSelect.h"

#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <iostream>
#include <string>
#include <sstream>
#include <array>

#define MAX 1024
#define BACKLOG 5
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR   (-1)


namespace SimpleServerSelect
{
    int error(std::string_view message, int handle)
    {
        std::cerr << message << ". Error = " << errno << std::endl;
        ::close(handle);
        return errno;
    }


    int server1()
    {
        const int serverSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (INVALID_SOCKET == serverSocket) {
            std::cout << "Failed to create the TCP socket. Error = " << errno << std::endl;
            return 0;
        }

        constexpr uint16_t port { 52525 };
        constexpr std::string_view host { "0.0.0.0" };

        sockaddr_in server { PF_INET, htons(port),{.s_addr = inet_addr(host.data())}, {}};
        if (SOCKET_ERROR == ::bind(serverSocket, reinterpret_cast<sockaddr*>(&server), sizeof(server))) {
            return error("Failed to bind socket", serverSocket);
        }

        constexpr uint16_t backlog { 5 };
        if (SOCKET_ERROR == ::listen(serverSocket, backlog)) {
            return error("Failed to Listen the socket", serverSocket);
        }

        std::array<int, FD_SETSIZE> clients{};
        std::fill(clients.begin(), clients.end(), -1);

        fd_set rset, allset;
        FD_ZERO(&allset);
        FD_SET(serverSocket, &allset);

        char buf[MAX];
        ssize_t n;
        sockaddr_in serveraddr{} , clientaddr{};
        socklen_t clilen;
        int nready = 0,  maxfd = 5, maxi = -1, connfd {-1}, id = 0, sockfd = -1;
        while (true)
        {
            rset = allset;
            if ((nready = select(maxfd+1, &rset, nullptr, nullptr, nullptr)) < 0) {
                return error("Error is select", serverSocket);
            }

            if (FD_ISSET(serverSocket, &rset))   /* new client has requested connection */
            {
                clilen = sizeof(clientaddr);
                // This is not blocking, because we know for sure that someone is trying to
                // connect, as serverSocket=1.
                if ((connfd = accept(serverSocket, (struct sockaddr *)&clientaddr, &clilen)) < 0)
                    return error("accept error", serverSocket);

                // find the first client[]=-1
                // TODO: Refactor
                for (id = 0 ; id < FD_SETSIZE ; ++id)
                {
                    if (clients[id] < 0)
                    {
                        clients[id] = connfd;
                        break;
                    }
                }

                // 'for loop' may break or run out of iterations. In case it runs out of iterations, that means
                // all spaces in client[] array has been filled.
                // In that case, just drop the request.
                if (FD_SETSIZE == id)
                    return error("Too many clients, dropping the request.", serverSocket);

                FD_SET(connfd, &allset); /* add the new file descriptor to set */

                // to increase the maxfdp1 value that goes in select().
                if(connfd > maxfd)
                    maxfd = connfd;

                // to update max number of clients.
                if (id > maxi)
                    maxi = id;

                /* i.e nready is the number of fds that are set(number of ready connections)
                   if its zero, then don't bother checking other fds, in set*/
                if((--nready)<=0)
                    continue;
            }

            /* handle all the clients requesting */
            for (int i = 0; i <= maxi; ++i)
            {
                if ((sockfd = clients[i]) < 0)
                    continue;

                if (FD_ISSET(sockfd, &rset))
                {
                    if( (n = recv(sockfd, buf, MAX, 0))==0 )
                    {
                        /* connection closed by client side */
                        close(sockfd);
                        FD_CLR(sockfd, &allset);
                        clients[i] = -1;
                    }
                    else
                        send(sockfd, buf, n, 0);
                    // error("error while sending the message.");

                    if(--nready < 0)
                        break;
                }
            }
        }
    }
};


void SimpleServerSelect::Tests()
{
    server1();


};
