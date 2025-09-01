/**============================================================================
Name        : Matcher_Visitor.cpp
Created on  : 01.09.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Matcher_Visitor.cpp
============================================================================**/

#include "Matcher_Visitor.hpp"

#include <iostream>
#include <variant>
#include <string>
#include <cstdint>


namespace
{
    struct WebEvent
    {
        struct PageLoad{};
        struct PageUnload{};
        struct KeyPress{char c;};
        struct Paste{std::string str;};
        struct Click{uint64_t x,y;};
        using webEvent = std::variant<PageLoad, PageUnload, KeyPress, Paste, Click>;
    };

    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };

    template<class T, class... Ts>
    auto match(const T& event, Ts&&... args) {
        return std::visit(overloaded{std::forward<Ts>(args)...}, event);
    }

}

void Matcher_Visitor::TestAll()
{
    const WebEvent::webEvent event(WebEvent::Click { .x=32, .y=64 } );
    match(event,
        [](const WebEvent::Click& click) {
            std::cout << "Clicked at x=" << click.x << ", y=" << click.y << "\n";
        },
        [](const auto&) { /* Ignore other events */ });
}