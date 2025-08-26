/**============================================================================
Name        : Session.hpp
Created on  : 12.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Session.hpp
============================================================================**/

#ifndef CPPPROJECTS_SESSION_HPP
#define CPPPROJECTS_SESSION_HPP

#include "Buffer.hpp"
#include <concepts>
#include <ostream>

namespace common
{
    using Socket = int32_t;
    using PortType = uint16_t;
    using SizeType = uint32_t;

    constexpr Socket INVALID_SOCKET{-1};
    constexpr Socket SOCKET_ERROR{-1};

    enum class State : uint8_t
    {
        Idle,
        Open,
        Reading,
        ReadyToWrite,
        Closed,
        ClosedWithError
    };

    std::ostream &operator<<(std::ostream &stream, State state);

    struct Session
    {
        Socket socket { INVALID_SOCKET };
        State state { State::Closed };
        common::Buffer request {};
        std::string response;

        explicit Session(Socket socket, State state = State::Open);

        void Close(State finalState = State::Closed);
    };


    template<typename T>
    concept RequestProcessor = requires(T& proc, Session& session)
    {
        { proc.process(session) } -> std::same_as<bool>;
    };
}

#endif //CPPPROJECTS_SESSION_HPP
