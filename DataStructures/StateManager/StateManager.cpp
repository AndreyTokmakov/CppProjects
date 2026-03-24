/**============================================================================
Name        : StateManager.cpp
Created on  : 24.03.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StateManager.cpp
============================================================================**/

#include "StateManager.hpp"

#include <iostream>
#include <concepts>

namespace state_manager
{
    class StateManager
    {
        using Mask = uint8_t;
        static constexpr Mask ONE = 1;

        enum class State : uint8_t {
            Active      = ONE << 0,
            Running     = ONE << 1,
            Started     = ONE << 2,
            Configured  = ONE << 3
        };

        Mask stateMask { 0 };

        static constexpr Mask bit(State s) noexcept {
            return static_cast<Mask>(s);
        }

        template<typename... Ty>
            requires (std::same_as<Ty, State> && ...)
        [[nodiscard]]
        static constexpr Mask mask(Ty... state) noexcept {
            return (bit(state) | ...);
        }

        [[nodiscard]]
        bool get(const State s) const noexcept {
            return (stateMask & bit(s)) != 0;
        }

        void set(const State s, const bool value) noexcept
        {
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


namespace state_manager::unit_tests
{
#define ASSERT_TRUE(x)  Assert((x), #x, __LINE__)
#define ASSERT_FALSE(x) Assert(!(x), #x, __LINE__)
#define ASSERT_EQ(a,b)  Assert(((a) == (b)), #a " == " #b, __LINE__)

    void Assert(const bool cond, const std::string_view expr, const int line)
    {
        if (!cond) {
            std::cerr << "Assertion failed at line " << line << ": " << expr << "\n";
            std::terminate();
        }
    }

    void test_default_state()
    {
        constexpr StateManager sm;

        ASSERT_FALSE(sm.isActive());
        ASSERT_FALSE(sm.isRunning());
        ASSERT_FALSE(sm.isStarted());
        ASSERT_FALSE(sm.isConfigured());

        ASSERT_EQ(sm.raw(), 0);
    }

    void test_set_individual_flags()
    {
        StateManager sm;

        sm.setActive(true);
        ASSERT_TRUE(sm.isActive());
        ASSERT_FALSE(sm.isRunning());

        sm.setRunning(true);
        ASSERT_TRUE(sm.isRunning());

        sm.setStarted(true);
        ASSERT_TRUE(sm.isStarted());

        sm.setConfigured(true);
        ASSERT_TRUE(sm.isConfigured());
    }

    void test_clear_individual_flags()
    {
        StateManager sm;

        sm.setActive(true);
        sm.setActive(false);
        ASSERT_FALSE(sm.isActive());

        sm.setRunning(true);
        sm.setRunning(false);
        ASSERT_FALSE(sm.isRunning());
    }

    void test_flags_are_independent()
    {
        StateManager sm;

        sm.setActive(true);
        sm.setRunning(true);

        ASSERT_TRUE(sm.isActive());
        ASSERT_TRUE(sm.isRunning());
        ASSERT_FALSE(sm.isStarted());
        ASSERT_FALSE(sm.isConfigured());

        sm.setActive(false);

        ASSERT_FALSE(sm.isActive());
        ASSERT_TRUE(sm.isRunning());
    }

    void test_reset()
    {
        StateManager sm;

        sm.setActive(true);
        sm.setRunning(true);
        sm.setStarted(true);

        sm.reset();

        ASSERT_FALSE(sm.isActive());
        ASSERT_FALSE(sm.isRunning());
        ASSERT_FALSE(sm.isStarted());
        ASSERT_FALSE(sm.isConfigured());

        ASSERT_EQ(sm.raw(), 0);
    }


    void test_raw_overwrite()
    {
        StateManager sm;

        sm.setActive(true);
        ASSERT_TRUE(sm.isActive());

        sm.setRaw(0);

        ASSERT_FALSE(sm.isActive());
        ASSERT_EQ(sm.raw(), 0);
    }

    void test_all()
    {
        test_default_state();
        test_set_individual_flags();
        test_clear_individual_flags();
        test_flags_are_independent();
        test_reset();
        test_raw_overwrite();
        std::cout << "All tests passed\n";
    }
}

void state_manager::TestAll()
{
    unit_tests::test_all();
}
