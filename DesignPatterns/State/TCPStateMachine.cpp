/**============================================================================
Name        : TCPStateMachine.cpp
Created on  : 02.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TCPStateMachine.cpp
============================================================================**/

#include "state.hpp"

#include <iostream>
#include <memory>
#include <utility>

namespace
{
    enum class Event: uint8_t {
        connect,
        connected,
        disconnect,
        timeout
    };

    std::ostream &operator<<(std::ostream &os, const Event event)
    {
        switch (event) {
            case Event::connect: os << "connect"; break;
            case Event::connected: os << "connected"; break;
            case Event::disconnect: os << "disconnect"; break;
            case Event::timeout: os << "timeout"; break;
        }
        return os;
    }

    struct IState
    {
        virtual std::unique_ptr<IState> onEvent(Event event) = 0;

        [[nodiscard]]
        virtual std::string getName() const  = 0;

        virtual ~IState() = default;
    };

    struct Idle : IState
    {
        std::unique_ptr<IState> onEvent(Event event) override;

        [[nodiscard]]
        std::string getName() const override {
            return {"Idle" };
        }
    };

    struct Connected : IState
    {
        std::unique_ptr<IState> onEvent(const Event event) override
        {
            std::cout << getName() << " ==> " << event;
            if (Event::disconnect == event)
                return std::make_unique<Idle>();
            return nullptr;
        }

        [[nodiscard]]
        std::string getName() const override {
            return {"Connected"};
        }
    };

    struct Connecting : IState
    {
        std::unique_ptr<IState> onEvent(const Event event) override
        {
            std::cout << getName() << " ==> " << event;
            switch (event) {
                case Event::connected:
                    return std::make_unique<Connected>();
                case Event::timeout:
                    return ++m_trial < m_max_trial ? nullptr : std::make_unique<Idle>();
                case Event::connect:
                    [[fallthrough]];
                case Event::disconnect:
                    return nullptr;
            }
            return nullptr;
        }

        [[nodiscard]]
        std::string getName() const override {
            return {"Connecting"};
        }

    private:
        uint32_t                    m_trial = 0;
        static constexpr uint8_t    m_max_trial = 3;
    };


    std::unique_ptr<IState> Idle::onEvent(const Event event)
    {
        std::cout << getName() << " ==> " << event;
        if (Event::connect == event) {
            return std::make_unique<Connecting>();
        }
        return nullptr;
    }


    struct Bluetooth
    {
        std::unique_ptr<IState> state = std::make_unique<Idle>();

        void dispatch(const Event e)
        {
            std::cout << "dispatching '" << e << "' event: ";
            if (std::unique_ptr<IState> stateNew = state->onEvent(e))
                state = std::move(stateNew);

            std::cout << " ==> " << state->getName() << std::endl;
        }

        template <typename... Events>
        void establish_connection(Events... e) {
            (dispatch(e), ...);
        }
    };


}

void state::tcp_state_machine::TestAll()
{
    Bluetooth bl;
    bl.establish_connection(Event::connect, Event::timeout, Event::connected, Event::disconnect);
}

/**
dispatching 'connect' event: Idle ==> connect ==> Connecting
dispatching 'timeout' event: Connecting ==> timeout ==> Connecting
dispatching 'connected' event: Connecting ==> connected ==> Connected
dispatching 'disconnect' event: Connected ==> disconnect ==> Idle
**/