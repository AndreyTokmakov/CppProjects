/**============================================================================
Name        : ConstexprVirtualFunctions.cpp
Created on  : 21.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ConstexprVirtualFunctions.cpp
============================================================================**/

#include "ConstexprVirtualFunctions.h"

#include <print>
#include <memory>


namespace ConstexprVirtualFunctions
{
    struct IAircraft
    {
        constexpr virtual void fly() const = 0;
        virtual ~IAircraft() = default;
    };

    struct Boeing: public IAircraft
    {
        static std::unique_ptr<Boeing> create() {
            return std::make_unique<Boeing>();
        }

        constexpr void fly() const override {
            std::println("Boeing::fly()");
        }

        ~Boeing() override {
            std::println("~Boeing::Boeing()");
        }
    };

    struct Airbus: public IAircraft
    {
        static std::unique_ptr<Airbus> create() {
            return std::make_unique<Airbus>();
        }

        constexpr void fly() const override {
            std::println("Airbus::fly()");
        }

        ~Airbus() override {
            std::println("~Airbus::Airbus()");
        }
    };

    void call(const IAircraft* aircraft) {
        aircraft->fly();
    }

    void test()
    {
        {
            Boeing boeing;
            Airbus airbus;

            call(&boeing);
            call(&airbus);
        }

        {
            call(Boeing::create().get());
            call(Airbus::create().get());
        }
    }
}

void ConstexprVirtualFunctions::TestAll()
{
    test();
}