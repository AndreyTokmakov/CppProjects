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


namespace ConstexprVirtualFunctions::Demo_One
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

namespace ConstexprVirtualFunctions::Demo_Two
{
    struct Base
    {
        virtual constexpr ~Base() = default;
        virtual constexpr int getValue() const = 0;
    };

    struct DerivedOne: Base
    {
        constexpr int getValue() const override { return 1; }
    };

    struct DerivedTwo: Base
    {
        constexpr int getValue() const override { return 2; }
    };

    constexpr int sumValues(auto objects)
    {
        int result = 0;
        for (const auto& obj: objects)
            result += obj->getValue();
        return result;
    }

    void test()
    {

        constexpr DerivedOne d1;
        constexpr DerivedTwo d2;

        // constexpr std::array<const Base*, 2> objects {&d1, &d2};
        // static_assert(3 == sumValues(objects));
    }
}

void ConstexprVirtualFunctions::TestAll()
{
    // Demo_One::test();
    Demo_Two::test();
}