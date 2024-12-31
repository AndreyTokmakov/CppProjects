/**============================================================================
Name        : ChainOfHandlers.cpp
Created on  : 03.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ChainOfHandlers.cpp
============================================================================**/

#include <iostream>
#include <memory>

namespace
{
    struct IHandler
    {
        virtual ~IHandler() = default;
        virtual void request(int value) = 0;
        virtual IHandler* setNextHandler(std::unique_ptr<IHandler> nextInLine) = 0;
    };

    struct Handler: IHandler
    {
        std::unique_ptr<IHandler> next { nullptr };

        IHandler* setNextHandler(std::unique_ptr<IHandler> nextInLine) override {
            next = std::move(nextInLine);
            return next.get();
        }
    };

    class SpecialHandler final : public Handler {
        int limit;
        int id;

    public:
        SpecialHandler(int limit, int id): limit {limit}, id {id} {
        }

        void request(int value) override
        {
            if (value < limit) {
                std::cout << "Handler with ID=" << id << " handled the request " << value << " with a limit of "
                        << limit << std::endl;
            } else if (next != nullptr) {
                next->request(value);
            } else {
                std::cout << "Sorry, I am the last handler (" << id << ") and I can't handle the request." << std::endl;
            }
        }
    };
}

void ChainOfHandlers_Test()
{
    std::unique_ptr<IHandler> handler = std::make_unique<SpecialHandler>(10, 1);
    handler->setNextHandler(std::make_unique<SpecialHandler>(20, 2))
        ->setNextHandler(std::make_unique<SpecialHandler>(30, 3));

    handler->request(18);
    handler->request(25);
    handler->request(40);
}