/**============================================================================
Name        : NetLink.cpp
Created on  : 30.08.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : NetLink
============================================================================**/

#include "NetLink.h"

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <linux/rtnetlink.h>

#include <iostream>
#include <string_view>

namespace {

    struct SocketGuard {
        int socket { -1 };

        explicit SocketGuard(int s): socket {s} {
        }

        ~SocketGuard() {
            if (-1 != socket)
                ::close(socket);
        }

        [[nodiscard]]
        inline bool isValid() const noexcept {
            return -1 == socket;
        }

        explicit operator bool() const noexcept {
            return (-1 != socket);
        }
    };

}

namespace NetLink
{
    void Create()
    {
        SocketGuard socketGuard {socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE)};
        if (!socketGuard) {
            std::cout << "Failed to create socket. Error = " << errno << std::endl;
            return;
        }

        std::cout << "Created\n";


    }
}

void NetLink::TestAll()
{
    Create();
};
