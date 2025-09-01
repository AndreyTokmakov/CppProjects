/**============================================================================
Name        : TcpServer.cpp
Created on  : 23.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TcpServer.cpp
============================================================================**/

#include "Session.hpp"

#include <ostream>
#include <iostream>
#include <string_view>
#include <unistd.h>    // ::close()


namespace common
{
    std::ostream& operator<<(std::ostream& stream, const State state)
    {
        using enum State;
        using namespace std::string_view_literals;
        switch (state) {
            case Idle:    return stream << "Idle"sv;
            case Open:    return stream << "Open"sv;
            case Reading: return stream << "Reading"sv;
            case ReadyToWrite: return stream << "ReadyToWrite"sv;
            case Closed:  return stream << "Closed"sv;
            case ClosedWithError: return stream << "ClosedWithError"sv;
        }
        return stream << "Unknown state(" << static_cast<int>(state) << ")";
    }

    Session::Session(const Socket socket, const State state):
            socket { socket }, state { state }
    {
        // std::cout << "Session(" << socket << ") created\n";
    }

    void Session::Close(const State finalState)
    {
        if (SOCKET_ERROR == ::close(socket)) {
            // Error("close() failed");
        }
        state = finalState;
        request.clear();
    }
}