/**============================================================================
Name        : DeducingThis.cpp
Created on  : 28.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DeducingThis.cpp
============================================================================**/

#include "DeducingThis.h"

#include <iostream>

namespace DeducingThis::Deduplicating_Function_Overloading
{
    struct Test
    {
        template <typename Self>
        void explicitCall(this Self&& self, const std::string& text)
        {
            std::cout << text << ": ";
            std::forward<Self>(self).implicitCall();
        }

        void implicitCall() & {
            std::cout << "non const lvalue" << std::endl;
        }

        void implicitCall() const& {
            std::cout << "const lvalue"<< std::endl;
        }

        void implicitCall() && {
            std::cout << "non const rvalue"<< std::endl;
        }

        void implicitCall() const&& {
            std::cout << "const rvalue"<< std::endl;
        }
    };


    void test()
    {
        Test test;
        const Test constTest;

        test.explicitCall("test");
        constTest.explicitCall("constTest");
        std::move(test).explicitCall("std::move(test)");
        std::move(constTest).explicitCall("std::move(consTest)");
    }
}

namespace CRTP_OldStyle
{
    template <typename Derived>
    struct Base
    {
        inline constexpr Derived& asDerivedClass() noexcept {
            return static_cast<Derived&>(*this);
        }

        void implementation(){
            std::cout << "Implementation Base" << '\n';
        }

        void makeCall()
        {
            asDerivedClass().implementation();
        }
    };

    struct Derived1: Base<Derived1>
    {
        void implementation() {
            std::cout << "Implementation Derived1" << '\n';
        }
    };

    struct Derived2: Base<Derived2>
    {
        void implementation() {
            std::cout << "Implementation Derived2" << '\n';
        }
    };

    struct Derived3: Base<Derived3>{};

    void demo()
    {
        Derived1{}.makeCall();
        Derived2{}.makeCall();
        Derived3{}.makeCall();

        // Output:
        //   Implementation Derived1
        //   Implementation Derived2
        //   Implementation Base
    }
}

namespace CRTP_NewStyle
{
    struct Base
    {
        template <typename Self>
        void makeCall(this Self&& self){
            self.implementation();
        }

        void implementation(){
            std::cout << "Implementation Base" << '\n';
        }
    };

    struct Derived1: Base {
        void implementation(){
            std::cout << "Implementation Derived1" << '\n';
        }
    };

    struct Derived2: Base {
        void implementation(){
            std::cout << "Implementation Derived2" << '\n';
        }
    };

    struct Derived3: Base{};

    void demo()
    {
        Derived1{}.makeCall();
        Derived2{}.makeCall();
        Derived3{}.makeCall();

        // Output:
        // Implementation Derived1
        // Implementation Derived2
        // Implementation Base
    }
}


void DeducingThis::TestAll()
{
    // Deduplicating_Function_Overloading::test();

    // CRTP_OldStyle::demo();
    // CRTP_NewStyle::demo();
}