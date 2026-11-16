/**============================================================================
Name        : SignalSlotBasic.cpp
Created on  : 16.11.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SignalSlotBasic.cpp
============================================================================**/

#include "SignalSlot.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace
{
    template<typename... Args>
    class Signal
    {
    public:
        using Slot = std::function<void(Args...)>;

        void connect(Slot slot)
        {
            slots.push_back(std::move(slot));
        }

        void emit(Args... args) const
        {
            for (const Slot& slot : slots)
                slot(args...);
        }

    private:
        std::vector<Slot> slots;
    };


}

void SignalSlot::SignalSlotBasic::demo()
{

    Signal<const std::string&> messageReceived;

    messageReceived.connect([](const std::string& message) {
        std::cout << "Logger: " << message << '\n';
    });
    messageReceived.connect([](const std::string& message) {
        std::cout << "Monitor: " << message << '\n';
    });
    messageReceived.connect([](const std::string& message) {
        std::cout << "Handler: " << message << '\n';
    });

    const std::string message = "Connection established";
    messageReceived.emit(message);
}

/*
bLogger: Connection established
Monitor: Connection established
Handler: Connection established
*/
