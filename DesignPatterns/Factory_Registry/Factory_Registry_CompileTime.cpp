/**============================================================================
Name        : Factory_Registry_CompileTime.cpp
Created on  : 08.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Factory_Registry_CompileTime.cpp
============================================================================**/

#include "Factory_Registry.hpp"

#include <variant>
#include <cstdint>
#include <stdexcept>
#include <iostream>


namespace
{
    enum class HandlerId : uint16_t {
        Print = 1,
        Log   = 2
    };

    struct PrintHandler
    {
        static constexpr HandlerId id = HandlerId::Print;

        static void handle()
        {
            std::cout << "PrintHandler\n";
        }
    };

    struct LogHandler
    {
        static constexpr HandlerId id = HandlerId::Log;

        static void handle()
        {
            std::cout << "LogHandler\n";
        }
    };

    template <typename... Ts>
    struct Registry {};

    using HandlerRegistry = Registry<
        PrintHandler,
        LogHandler
    >;

    template <typename... Ts>
    std::variant<Ts...>  makeHandler(HandlerId id, Registry<Ts...>)
    {
        std::variant<Ts...> result;
        bool found = false;

        // fold-expression по всем типам
        ([&] {
            if (!found && Ts::id == id) {
                result = Ts{};
                found = true;
            }
        }(), ...);

        if (!found) {
            throw std::runtime_error("Unknown handler id");
        }

        return result;
    }

}

void factory_registry::compiletimeExample()
{
    using Handler = std::variant<PrintHandler, LogHandler>;
    Handler h1 = makeHandler(HandlerId::Print, HandlerRegistry{});
    Handler h2 = makeHandler(HandlerId::Log,   HandlerRegistry{});

    std::visit([](const auto& h) {
        h.handle();
    }, h1);

    std::visit([](const auto& h) {
        h.handle();
    }, h2);

    // PrintHandler
    // LogHandler
}
