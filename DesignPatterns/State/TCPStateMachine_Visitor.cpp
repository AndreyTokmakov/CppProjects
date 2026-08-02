/**============================================================================
Name        : TCPStateMachine_Visitor.cpp
Created on  : 02.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TCPStateMachine_Visitor.cpp
============================================================================**/

#include "state.hpp"

#include <iostream>
#include <memory>
#include <utility>
#include <optional>
#include <variant>

namespace
{
    struct EventConnect { std::string m_address; };
    struct EventConnected { };
    struct EventDisconnect { };
    struct EventTimeout { };

    struct Idle { };
    struct Connected { };
    struct Connecting
    {
        std::string                 m_address;
        uint32_t                    m_trial = 0;
        static constexpr uint8_t    m_max_trial = 3;
    };

    using Event = std::variant<EventConnect,EventConnected, EventDisconnect, EventTimeout>;
    using State = std::variant<Idle, Connecting, Connected>;


    struct TransitionsHandler
    {
        std::optional<State> operator()(Idle&, const EventConnect &event) const
        {
            std::cout << "Idle -> Connect (" << event.m_address << ")\n";
            return Connecting{event.m_address};
        }

        std::optional<State> operator()(Connecting&, const EventConnected &) const
        {
            std::cout << "Connecting -> Connected" << std::endl;
            return Connected{};
        }

        std::optional<State> operator()(Connecting &s, const EventTimeout &) const
        {
            std::cout << "Connecting -> Timeout" << std::endl;
            return ++s.m_trial < Connecting::m_max_trial ? std::nullopt : std::optional<State>(Idle{});
        }

        std::optional<State> operator()(Connected&, const EventDisconnect &) const
        {
            std::cout << "Connected -> Disconnect" << std::endl;
            return Idle{};
        }

        template <typename State_t, typename Event_t>
        std::optional<State> operator()(State_t &, const Event_t &) const
        {
            std::cout << "Unknown" << std::endl;
            return std::nullopt;
        }
    };

    template <typename StateVariant,
              typename EventVariant,
              typename EventHandler>
    struct Bluetooth
    {
        StateVariant currentState;
        static constexpr EventHandler handler {};

        void dispatch(const EventVariant &event)
        {
            if (std::optional<StateVariant> newState = std::visit(handler, currentState, event))
                currentState = std::move(*newState);
        }

        template <typename... Events>
        void dispatchEvents(Events... e) {
            (dispatch(e), ...);
        }
    };
}


void state::tcp_state_machine_visitor::TestAll()
{
    Bluetooth<State, Event, TransitionsHandler> bl;
    bl.dispatchEvents(EventConnect{"AA:BB:CC:DD"},
                      EventTimeout{},
                      EventConnected{},
                      EventDisconnect{});
}

/**
Idle -> Connect (AA:BB:CC:DD)
Connecting -> Timeout
Connecting -> Connected
Connected -> Disconnect
**/













