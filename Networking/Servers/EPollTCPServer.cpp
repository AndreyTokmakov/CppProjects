/**============================================================================
Name        : EPollTCPServer.h
Created on  : 22.03.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : EPollTCPServer
============================================================================**/

#include "EPollTCPServer.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <sstream>
#include <cstring>
#include <map>


#include <iostream>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <netdb.h>

#define BUFFERSIZE 512
#define MAXEVENTS 128
#define MAX_EVENTS 20
#define QUEUE 100
#define PORT 1012
#define READ_BUF_LEN 256

namespace EPollTCPServer::One
{
    int SetNonBlocking(int sock)
    {
        int opts;
        opts=fcntl(sock,F_GETFL);
        if(opts<0)
        {
            perror("fcntl(sock,F_GETFL)");
            return -1;
        }
        opts|=O_NONBLOCK;
        if(fcntl(sock,F_SETFL,opts)<0)
        {
            perror("fcntl(sock,F_SETFL,opts)");
            return -1;
        }
        return 0;
    }

    void InitSock(int &listenfd)
    {
        listenfd = socket(AF_INET,SOCK_STREAM,0);// Returns a SOCKFD (socket descriptor) if successful
        SetNonBlocking(listenfd);


        struct sockaddr_in server_addr; // Is generally stored in the address and port. Display and storage of information

        /* Set the relevant parameters in the SockAddr_in structure */
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);// convert an unsigned short value to network word sequence, that is, large end mode (BIG-Endian)
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);// Convert the number of unsigned long shapes of the host into network byte order.


        /* Enable address reuse */
        int on = 1;
        // Open the Socket port multiplexing to prevent Test, address Already in Use
        int result = setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
        if (-1 == result) {
            perror ("setsockopt");
            exit(EXIT_FAILURE);
        }

        if(bind(listenfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) == -1)
        {
            perror("bind");
            exit(EXIT_FAILURE);
        }

        if(listen(listenfd,QUEUE)==-1)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
    }


    int runServer()
    {
        int listenfd, epfd, nfds, conn_sock;
        struct epoll_event ev, events[MAX_EVENTS];
        struct sockaddr_in local_addr;
        socklen_t local_len=sizeof(local_addr);

        InitSock(listenfd);

        epfd=epoll_create(MAX_EVENTS);
        if(epfd==-1)
        {
            perror("epoll_create");
            exit(EXIT_FAILURE);
        }

        ev.events=EPOLLIN | EPOLLET;
        ev.data.fd=listenfd;

        if(epoll_ctl(epfd,EPOLL_CTL_ADD, listenfd,&ev)==-1)
        {
            perror("epoll_ctl");
            exit(EXIT_FAILURE);
        }
        char recvBuf[READ_BUF_LEN]={0};
        char sendBuf[READ_BUF_LEN]={0};
        char host_buf[NI_MAXHOST];// IP address cache
        char port_buf[NI_MAXSERV];// Port Cache
        int wait_count;
        int _result;
        sockaddr in_addr {0};

        socklen_t in_addr_len=sizeof(in_addr);
        ssize_t result_len;

        while(1)
        {

            // Waiting for the event
            wait_count = epoll_wait(epfd,events,MAX_EVENTS,-1);

            for(int i=0;i<wait_count;i++)
            {
                // Judgment if EPOLL has an error
                if(events[i].events & EPOLLERR || events[i].events & EPOLLHUP || !(events[i].events & EPOLLIN))
                {
                    std::cout<<"Epoll error"<<std::endl;
                    close(events[i].data.fd);
                    continue;
                }
                else if(listenfd == events[i].data.fd)// has a new connection
                {
                    std::cout<<"Prepare new connection..."<<std::endl;
                    bzero(&in_addr,sizeof(in_addr));
                    int acceptfd=accept(listenfd,&in_addr,&in_addr_len);
                    if(-1 == acceptfd)
                    {
                        std::cout<<"Accept error"<<std::endl;
                        break;

                    }
                    SetNonBlocking(acceptfd);

                    _result=getnameinfo(&in_addr,sizeof(in_addr),
                                        host_buf,sizeof(host_buf)/sizeof (host_buf[0]),
                                        port_buf, sizeof (port_buf) / sizeof (port_buf[0]),
                                        NI_NUMERICHOST | NI_NUMERICSERV);

                    if(!_result)
                    {
                        std::cout<<"New connect: host= "<<host_buf<<", port= "
                            <<port_buf<<", fd="<<acceptfd<<std::endl;
                    }

                    ev.data.fd=acceptfd;
                    ev.events=EPOLLIN|EPOLLET;

                    // Set EPOLL event for the new ACCEPT's File Describe
                    _result=epoll_ctl(epfd,EPOLL_CTL_ADD,acceptfd,&ev);
                    if(-1==_result)
                    {
                        std::cout<<"epoll_ctl"<<std::endl;
                        return 0;
                    }
                }
                else if(events[i].events & EPOLLIN)// Receive data, read Socket
                {
                    memset(recvBuf,0,sizeof(recvBuf));
                    result_len=recv(events[i].data.fd, recvBuf, sizeof(recvBuf), 0);

                    if(result_len <= 0)
                    {
                        std::cout<<"close event:"<<events[i].data.fd<<std::endl;
                        // Close Socket
                        close(events[i].data.fd);
                        // Delete the EPOLL identifier
                        epoll_ctl(epfd,EPOLL_CTL_DEL,events[i].data.fd,&events[i]);
                    }
                    else
                    {
                        std::cout<<"fd="<< events[i].data.fd <<" recv="<<recvBuf<<std::endl;

                        memset(sendBuf,0,sizeof(sendBuf));
                        memcpy(sendBuf,recvBuf,strlen(recvBuf));
                        events[i].events = EPOLLET | EPOLLOUT;
                        // Modify the EPOLL identifier, wait for the next loop to send data
                        epoll_ctl(epfd, EPOLL_CTL_MOD, events[i].data.fd, &events[i]);
                    }
                }
                else if(events[i].events & EPOLLOUT)// There are data to be sent, write socket
                {
                    send(events[i].data.fd,sendBuf,strlen(sendBuf),0);

                    events[i].events = EPOLLET | EPOLLIN;
                    // Modify the EPOLL identifier, wait for the next loop to send data
                    epoll_ctl(epfd, EPOLL_CTL_MOD, events[i].data.fd, &events[i]);
                }
            }

        }
        close(epfd);
    }
};

namespace EPollTCPServer::Two
{
    int serverSock_init (char *port)
    {
        addrinfo hints, *res;

        memset (&hints, 0, sizeof(addrinfo));
        hints.ai_family = AF_INET;  // IPV4
        hints.ai_socktype = SOCK_STREAM; // TCP socket
        hints.ai_flags = AI_PASSIVE;     // needed for serversocket

        // populates res addrinfo struct ready for socket call
        int status = getaddrinfo (nullptr, port, &hints, &res);
        if (status != 0)
        {
            fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (status));
            return -1;
        }

        int sfd = socket (res->ai_family, res->ai_socktype, res->ai_protocol); // create endpoint socketFD
        if (sfd == -1) {
            fprintf (stderr, "Socket error\n");
            close (sfd);
            return -1;
        }

        int optval = 1; // set port reuse opt for debugging
        setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

        status = bind (sfd, res->ai_addr, res->ai_addrlen); // bind addr to sfd, addr in this case is INADDR_ANY
        if (status == -1)
        {
            fprintf (stderr, "Could not bind\n");
            return -1;
        }

        freeaddrinfo (res);
        return sfd;
    }

    void runServer()
    {
        epoll_event event {}, *events;
        std::map<int,int> clientMap;

        int sfd = serverSock_init ("52525");
        if (sfd == -1)
            abort ();

        int flags = fcntl (sfd, F_GETFL, 0);  // change socket fd to be non-blocking
        flags |= O_NONBLOCK;
        fcntl (sfd, F_SETFL, flags);

        int s = listen (sfd, SOMAXCONN);  // mark socket as passive socket type
        if (s == -1) {
            perror ("listen");
            abort ();
        }

        int efd = epoll_create1 (0);  // create epoll instance
        if (efd == -1) {
            perror ("epoll_create");
            abort ();
        }

        event.data.fd = sfd;
        event.events = EPOLLIN | EPOLLET;  // just interested in read's events using edge triggered mode
        s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event); // Add server socket FD to epoll's watched list
        if (s == -1)
        {
            perror ("epoll_ctl");
            abort ();
        }

        /* Events buffer used by epoll_wait to list triggered events */
        events = (epoll_event*) calloc (MAXEVENTS, sizeof(event));

        /* The event loop */
        while (1)
        {
            int n, i;

            n = epoll_wait (efd, events, MAXEVENTS, -1);  // Block until some events happen, no timeout
            for (i = 0; i < n; i++)
            {

                /* Error handling */
                if ((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN)))
                {
                    /* An error has occured on this fd, or the socket is not
                       ready for reading (why were we notified then?) */
                    fprintf (stderr, "epoll error\n");
                    close (events[i].data.fd);  // Closing the fd removes from the epoll monitored list
                    clientMap.erase(events[i].data.fd);
                    continue;
                }

                    /* serverSocket accepting new connections */
                else if (sfd == events[i].data.fd)
                {
                    /* We have a notification on the listening socket, which
                       means one or more incoming connections. */
                    while (1)
                    {
                        struct sockaddr in_addr;
                        socklen_t in_len;
                        int infd;
                        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                        in_len = sizeof in_addr;
                        infd = accept (sfd, &in_addr, &in_len); // create new socket fd from pending listening socket queue
                        if (infd == -1) // error
                        {
                            if ((errno == EAGAIN) ||
                                (errno == EWOULDBLOCK))
                            {
                                /* We have processed all incoming connections. */
                                break;
                            }
                            else
                            {
                                perror ("accept");
                                break;
                            }
                        }

                        int optval = 1;
                        setsockopt(infd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));  // set socket for port reuse

                        /* get the client's IP addr and port num */
                        s = getnameinfo (&in_addr, in_len,
                                         hbuf, sizeof hbuf,
                                         sbuf, sizeof sbuf,
                                         NI_NUMERICHOST | NI_NUMERICSERV);
                        if (s == 0)
                        {
                            printf("Accepted connection on descriptor %d "
                                   "(host=%s, port=%s)\n", infd, hbuf, sbuf);
                        }

                        /* Make the incoming socket non-blocking and add it to the
                           list of fds to monitor. */
                        int flags = fcntl (infd, F_GETFL, 0);
                        flags |= O_NONBLOCK;
                        fcntl (infd, F_SETFL, flags);

                        event.data.fd = infd;
                        event.events = EPOLLIN | EPOLLET;

                        s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);
                        if (s == -1)
                        {
                            perror ("epoll_ctl");
                            abort ();
                        }
                        clientMap[event.data.fd]=0;  // init msg counter
                    }
                    continue;
                }
                else
                {
                    /* We have data on the fd waiting to be read. Read and
                       display it. We must read whatever data is available
                       completely, as we are running in edge-triggered mode
                       and won't get a notification again for the same
                       data. */
                    int done = 0;

                    while (1)
                    {
                        ssize_t count;
                        char buf[BUFFERSIZE];

                        count = read (events[i].data.fd, buf, sizeof buf);

                        if (count == -1)
                        {
                            /* If errno == EAGAIN, that means we have read all
                               data. So go back to the main loop. */
                            if (errno != EAGAIN)
                            {
                                perror ("read");
                                done = 1;
                            }
                            break;
                        }
                        else if (count == 0)
                        {
                            /* End of file. The remote has closed the
                               connection. */
                            done = 1;
                            break;
                        }

                        buf[count]=0;
                        char wbuf[BUFFERSIZE];
                        int cx=snprintf(wbuf,BUFFERSIZE,"(fd:%d seq:%d) %s",events[i].data.fd, clientMap[events[i].data.fd],buf);

                        /* Write the buffer to standard output */
                        s = write (1, wbuf, cx);
                        if (s == -1)
                        {
                            perror ("write");
                            abort ();
                        }
                    }
                    // Increment msg counter
                    int tmp = clientMap[events[i].data.fd];
                    tmp++;
                    clientMap[events[i].data.fd]=tmp;

                    if (done)
                    {
                        printf ("Closed connection on descriptor %d\n",
                                events[i].data.fd);

                        /* Closing the descriptor will make epoll remove it
                           from the set of descriptors which are monitored. */
                        close (events[i].data.fd);
                        clientMap.erase(events[i].data.fd);
                    }
                }
            }
        }

        free (events);
        close (sfd);
    }
};

void EPollTCPServer::Tests()
{
    // One::runServer();
    Two::runServer();
};
