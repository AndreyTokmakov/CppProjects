/**============================================================================
Name        : DependencyInjection.h
Created on  : 28.04.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DependencyInjection
============================================================================**/

#include "DependencyInjection.h"

#include <memory>
#include <iostream>

namespace DependencyInjection
{
    struct IService {
        virtual void make_important_call() = 0;
        virtual ~IService() = default;
    };

    struct FakeService : IService {
        void make_important_call() override {
            std::cout << "Fake Service.\n";
        }
    };

    struct ProductionService : IService {
        void make_important_call() override {
            std::cout << "Production Service.\n";
        }
    };


    struct Client
    {
        // Inject dependency
        explicit Client(std::unique_ptr<IService> service) :
                service { std::move(service) } {
        }

        void operate() {
            service->make_important_call();
        }

    private:
        std::unique_ptr<IService> service;
    };
}

void DependencyInjection::TestAll()
{
    // In production code:
    Client m(std::make_unique<ProductionService>());
    m.operate();

    // In test code:
    Client n(std::make_unique<FakeService>());
    n.operate();
};
