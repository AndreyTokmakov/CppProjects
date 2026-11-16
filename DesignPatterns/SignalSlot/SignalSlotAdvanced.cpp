/**============================================================================
Name        : SignalSlotAdvanced.cpp
Created on  : 16.11.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SignalSlotAdvanced.cpp
============================================================================**/

#include "SignalSlot.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <memory>


namespace
{
    template<typename... Args>
    class Signal
    {
    private:
        struct SlotEntry
        {
            std::function<void(Args...)> callback;
            bool connected = true;
        };

    public:
        class Connection
        {
        public:
            Connection() = default;

            void disconnect() const noexcept
            {
                if (const std::shared_ptr<SlotEntry> entry = slot.lock()) {
                    entry->connected = false;
                }
            }

            [[nodiscard]]
            bool isConnected() const noexcept
            {
                if (const std::shared_ptr<SlotEntry> entry = slot.lock()) {
                    return entry->connected;
                }
                return false;
            }

        private:
            friend class Signal;

            explicit Connection(const std::shared_ptr<SlotEntry>& entry) noexcept:
                slot(entry)
            {
            }

            std::weak_ptr<SlotEntry> slot;
        };

        class ScopedConnection
        {
        public:
            explicit ScopedConnection(Connection connection) noexcept:
                connection(std::move(connection))
            {
            }

            ScopedConnection(const ScopedConnection&) = delete;
            ScopedConnection& operator=(const ScopedConnection&) = delete;

            ScopedConnection(ScopedConnection&& other) noexcept : connection { std::move(other.connection) }
            {
            }

            ScopedConnection& operator=(ScopedConnection&& other) noexcept
            {
                if (this != &other)
                {
                    disconnect();
                    connection = std::move(other.connection);
                }
                return *this;
            }

            ~ScopedConnection()
            {
                disconnect();
            }

            void disconnect() noexcept
            {
                connection.disconnect();
            }

        private:
            Connection connection;
        };

        using Slot = std::function<void(Args...)>;

        [[nodiscard]]
        Connection connect(Slot slot)
        {
            std::erase_if(slots, [](const std::shared_ptr<SlotEntry>& entry){
                return !entry->connected;
            });

            const std::shared_ptr<SlotEntry> entry = std::make_shared<SlotEntry>(std::move(slot));
            slots.push_back(entry);

            return Connection { entry };
        }

        void emit(Args... args) const
        {
            const std::vector<std::shared_ptr<SlotEntry>> snapshot = slots;
            for (const std::shared_ptr<SlotEntry>& entry : snapshot)
            {
                if (entry->connected) {
                    entry->callback(args...);
                }
            }
        }

    private:
        std::vector<std::shared_ptr<SlotEntry>> slots;
    };
}


void SignalSlot::SignalSlotAdvanced::demo()
{
    Signal<const std::string&> messageReceived;

    // Regular connection.
    const Signal<const std::string&>::Connection loggerConnection = messageReceived.connect([](const std::string& message){
        std::cout << "Logger: " << message << '\n';
    });

    // Scoped connection.
    Signal<const std::string&>::ScopedConnection monitorConnection { messageReceived.connect([](const std::string& message){
        std::cout << "Monitor: " << message << '\n';
    })};

    // Another regular connection.
    const auto handlerConnection = messageReceived.connect([](const std::string& message) {
        std::cout << "Handler: " << message << '\n';
    });

    const std::string message = "Connection established";

    std::cout << "--- First emit ---\n";
    messageReceived.emit(message);

    std::cout << "\n--- Disconnect logger ---\n";
    loggerConnection.disconnect();

    messageReceived.emit(message);

    std::cout << "\n--- Scoped connection disconnect ---\n";
    monitorConnection.disconnect();

    messageReceived.emit(message);

    std::cout << "\n--- Connection states ---\n";
    std::cout << "Logger: " << loggerConnection.isConnected() << '\n';
    std::cout << "Handler: " << handlerConnection.isConnected() << '\n';
}

#if 0
--- First emit ---
Logger: Connection established
Monitor: Connection established
Handler: Connection established

--- Disconnect logger ---
Monitor: Connection established
Handler: Connection established

--- Scoped connection disconnect ---
Handler: Connection established

--- Connection states ---
Logger: 0
Handler: 1

#endif