/**============================================================================
Name        : CommandWrapper.cpp
Created on  : 14.08.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : CommandWrapper.cpp
============================================================================**/

#include "CommandWrapper.hpp"

#include <functional>
#include <string>
#include <iostream>
#include <print>
#include <array>
#include <concepts>

/*
 * StaticCommand
 *
 * A small, fixed-size, type-erased command wrapper designed for low-latency
 * scenarios where dynamic memory allocation is undesirable.
 *
 * The wrapper stores a command object directly inside an internal fixed-size
 * buffer and uses a function pointer for type-erased dispatch.
 *
 * Architecture:
 *
 *     StaticCommand
 *     +---------------------------+
 *     | fixed-size storage        |
 *     |                           |
 *     |   Cmd object              |
 *     |                           |
 *     +---------------------------+
 *     | execute callback          |
 *     +---------------------------+
 *                 |
 *                 v
 *         Cmd::execute() const
 *
 * The concrete command type is known when StaticCommand is constructed, but
 * does not need to be known by code using the StaticCommand interface.
 *
 * Example:
 *
 *     struct Command
 *     {
 *         explicit Command(Subject& subject) : subject { subject } {
 *         }
 *
 *         void execute() const {
 *             subject.call();
 *         }
 *
 *     private:
 *         Subject& subject;
 *     };
 *
 *     Subject subject;
 *     StaticCommand command { Command { subject } };
 *
 *     command.execute();
 *
 * Design goals:
 *
 *   - No dynamic memory allocation.
 *   - Fixed and predictable object size.
 *   - No virtual functions or vtable.
 *   - Runtime type erasure through a function pointer.
 *   - Inline storage for the concrete command object.
 *   - Compile-time validation of the command interface.
 *   - Suitable for queues, ring buffers and other latency-sensitive code.
 *
 * Command requirements:
 *
 *   - Cmd must provide:
 *
 *         void execute() const;
 *
 *   - Cmd must fit into StorageSize bytes.
 *   - Cmd must satisfy the lifetime/copyability requirements imposed by
 *     StaticCommand.
 *
 * Dispatch:
 *
 *     command.execute()
 *             |
 *             v
 *     callback(storage.data())
 *             |
 *             v
 *     static_cast<const Cmd*>(ptr)->execute()
 *
 * Unlike std::function, the storage size and allocation behavior are fully
 * controlled by StaticCommand. The trade-off is that the maximum command size
 * is fixed and the supported command types are subject to the constraints
 * imposed by the implementation.
 *
 * This class is intended for cases where predictable memory behavior and
 * bounded object size are more important than supporting arbitrary callable
 * objects.
*/

namespace
{
    struct Subject
    {
        std::string name;

        void call(){
            std::println("{}", name);
        }
    };

    struct CommandOne
    {
        void execute() const {
            sub.call();
        }

        explicit CommandOne(Subject& s): sub {s} {
        }

    private:
        Subject& sub;
    };

    template<typename Cmd>
    concept HasExecMethods = requires(const Cmd& cmd)
    {
        { cmd.execute() } -> std::same_as<void>;
    };

    struct StaticCommand
    {
        StaticCommand() = delete;

        template<HasExecMethods Cmd>
        explicit StaticCommand(Cmd cmd)
        {
            static_assert(sizeof(Cmd) <= StorageSize, "Command is to large");
            static_assert(std::is_trivially_destructible_v<Cmd>, "Command is not trivially destructible");
            static_assert(std::is_trivially_copyable_v<Cmd>, "Command must be trivially copyable");

            new (storage.data()) Cmd { std::move(cmd) };
            callback = [](const void* ptr) noexcept {
                static_cast<const Cmd*>(ptr)->execute();
            };
        }

        void execute() {
            std::invoke(callback, storage.data());
        }

    private:

        static constexpr std::size_t StorageSize { 16 };

        alignas(std::max_align_t) std::array<std::byte, StorageSize> storage {};
        void (*callback)(const void*) noexcept { nullptr };
    };

    void std_Function()
    {
        Subject sub { "FunctionTest" };
        CommandOne command { sub};

        const std::function<void(CommandOne&)> callback = [](const CommandOne& cmd) {
            cmd.execute();
        };
        callback(command);
    }

    void staticCommandWrapper()
    {
        Subject sub { "StaticCommandWrapperTest" };
        const CommandOne command { sub };

        StaticCommand cmdStat { command};
        cmdStat.execute();
    }
}

void command_wrapper::TestAll()
{
    std_Function();
    staticCommandWrapper();
}

// FunctionTest
// StaticCommandWrapperTest
