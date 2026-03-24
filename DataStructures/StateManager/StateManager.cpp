/**============================================================================
Name        : StateManager.cpp
Created on  : 24.03.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StateManager.cpp
============================================================================**/

#include "StateManager.hpp"

#include <cstdint>

namespace state_manager
{
    class StateManager
    {
    public:
        enum class State : uint8_t {
            Active      = 0,
            Running     = 1,
            Started     = 2,
            Configured  = 3
        };

    private:

        using Mask = uint8_t;
        static constexpr Mask ONE = 1;

        Mask stateMask { 0 };

        static constexpr Mask bit(State s) noexcept {
            return static_cast<Mask>(ONE << static_cast<Mask>(s));
        }

        [[nodiscard]]
        bool get(const State s) const noexcept {
            return stateMask & bit(s);
        }

        void set(const State s, const bool value) noexcept {
            if (value) {
                stateMask |= bit(s);
            } else {
                stateMask &= ~bit(s);
            }
        }

    public:
        [[nodiscard]]
        bool isActive() const noexcept {
            return get(State::Active);
        }

        [[nodiscard]]
        bool isRunning() const noexcept {
            return get(State::Running);
        }

        [[nodiscard]]
        bool isStarted() const noexcept {
            return get(State::Started);
        }

        [[nodiscard]]
        bool isConfigured() const noexcept {
            return get(State::Configured);
        }

        void setActive(const bool v) noexcept {
            set(State::Active, v);
        }

        void setRunning(const bool v) noexcept {
            set(State::Running, v);
        }

        void setStarted(const bool v) noexcept {
            set(State::Started, v);
        }

        void setConfigured(const bool v) noexcept {
            set(State::Configured, v);
        }

        void reset() noexcept {
            stateMask = 0;
        }

        [[nodiscard]]
        Mask raw() const noexcept {
            return stateMask;
        }

        void setRaw(const Mask mask) noexcept {
            stateMask = mask;
        }
    };

}



void state_manager::TestAll()
{

}
