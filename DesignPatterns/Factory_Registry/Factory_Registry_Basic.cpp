/**============================================================================
Name        : Factory_Registry_Basic.cpp
Created on  : 08.02.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Factory_Registry_Basic.cpp
============================================================================**/

#include "Factory_Registry.hpp"

#include <iostream>
#include <memory>
#include <unordered_map>

namespace
{
    struct Handler
    {
        virtual ~Handler() = default;
        virtual void handle(const std::string& msg) = 0;
    };

    enum class HandlerId : uint16_t {
        Print = 1,
        Log   = 2
    };

    struct HandlerFactory
    {
        using Creator = std::unique_ptr<Handler>(*)();

        static HandlerFactory& instance()
        {
            static HandlerFactory factory;
            return factory;
        }

        void register_handler(HandlerId id, Creator c)
        {
            if (auto [it, ok] = creators_.emplace(id, c); !ok) {
                throw std::runtime_error("Handler already registered");
            }
        }

        std::unique_ptr<Handler> create(const HandlerId id) const
        {
            const auto it = creators_.find(id);
            if (it == creators_.end()) {
                throw std::runtime_error("Unknown handler id");
            }
            return it->second();
        }

    private:
        std::unordered_map<HandlerId, Creator> creators_;
    };


    struct PrintHandler : Handler
    {
        void handle(const std::string& msg) override {
            std::cout << "Print: " << msg << '\n';
        }
    };

    struct LogHandler : Handler
    {
        void handle(const std::string& msg) override {
            std::cout << "Log: " << msg << '\n';
        }
    };

    std::unique_ptr<Handler> create_print() {
        return std::make_unique<PrintHandler>();
    }

    std::unique_ptr<Handler> create_log() {
        return std::make_unique<LogHandler>();
    }

    struct Registrar
    {
        Registrar()
        {
            auto& factory = HandlerFactory::instance();
            factory.register_handler(HandlerId::Log, &create_print);
            factory.register_handler(HandlerId::Print, &create_log);
        }
    };
}

void factory_registry::basic()
{
    Registrar {};

    {
        const std::unique_ptr<Handler> obj = HandlerFactory::instance().create(HandlerId::Log);
        obj->handle("hello");
    }
    {
        const std::unique_ptr<Handler> obj = HandlerFactory::instance().create(HandlerId::Print);
        obj->handle("world");
    }

    // Print: hello
    // Log: world
}
