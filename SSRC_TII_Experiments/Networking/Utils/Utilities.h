/**============================================================================
Name        : Utilities.h
Created on  : 09.12.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Utilities
============================================================================**/

#ifndef CPPPROJECTS_UTILITIES_H
#define CPPPROJECTS_UTILITIES_H

#include <string>
#include <linux/if_packet.h>
#include <cstdint>


namespace Utilities
{
    [[nodiscard("Call may be expensive")]]
    uint16_t Checksum(uint16_t *ptr, uint16_t bytes) noexcept;

    [[nodiscard]]
    sockaddr_ll ResolveInterfaceAddress(std::string_view interfaceName);

    // [[nodiscard]]
    bool ResolveInterfaceAddress(std::string_view interfaceName, sockaddr_ll& out);

    // TODO: Fix perf
    void PrintMACAddress(const uint8_t* mac);

    // TODO: Fix perf
    [[nodiscard("nodiscard")]]
    std::string IpToStr(unsigned long address);

    // TODO: Fix perf
    [[nodiscard("Don't forget to use the return value somehow.")]]
    std::string HostToIp(std::string_view host) noexcept;

    // Terminates if not
    void checkRunningUnderRoot();


    struct SocketScoped
    {
        static constexpr int INVALID_HANDLE { -1 };
        static constexpr int SOCKET_ERROR { -1 };

        int handle { INVALID_HANDLE };

        SocketScoped(int s = INVALID_HANDLE) : handle { s } {
        }

        SocketScoped& operator=(int s) {
            if (handle != s)
                closeSocket(handle);

            handle = s;
            return *this;
        }

        SocketScoped& operator=(const SocketScoped& sock) {
            if (handle != sock.handle) {
                closeSocket(handle);
                handle = sock.handle;
            }
            return *this;
        }

        operator int() const { // No explicit
            return handle;
        }

        ~SocketScoped() {
            closeSocket(handle);
        }

    private:
        static void closeSocket(int s);
    };

};


#endif //CPPPROJECTS_UTILITIES_H
