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
#include <utility>
#include <spdlog/fmt/bundled/core.h>

namespace DeducingThis::Explicit_Object_Member_Functions
{
    struct SimpleObject
    {
        std::string name { "SimpleObject" };

        void foo1(this SimpleObject const& self, int i) // same as void foo(int i) const &;
        {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            std::cout << "name: " << self.name << std::endl;

            /** ERROR: Will not compile **/
            // std::cout << "name: " << this->name << std::endl;
        }

        void foo2(int i) const
        {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            std::cout << "name: " << this->name << std::endl;
        }

        void bar(this SimpleObject self, int i) // pass object by value: makes a copy of “*this”
        {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }
    };

    struct SimpleObjectAuto
    {
        std::string name { "SimpleObjectAuto" };

        void foo1(this auto& self, int i)
        {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
            std::cout << "name: " << self.name << std::endl;
        }
    };

    void simpleExample()
    {
        {
            SimpleObject obj;
            obj.foo1(1);
            obj.foo2(1);
        }
        {
            SimpleObjectAuto obj;
            obj.foo1(1);
        }
    }
}



namespace DeducingThis::Deduplicating_Function_Overloading
{
    struct Test
    {
        template <typename Self>
        void explicitCall(this Self&& self, const std::string& text)
        {
            std::forward<Self>(self).implicitCall(text);
        }

        void implicitCall(const std::string& text) & {
            std::cout << "non const lvalue. input: " << text  << std::endl;
        }

        void implicitCall(const std::string& text) const& {
            std::cout << "const lvalue. input: " << text  << std::endl;
        }

        void implicitCall(const std::string& text) && {
            std::cout << "non const rvalue. input: " << text  << std::endl;
        }

        void implicitCall(const std::string& text) const&& {
            std::cout << "const rvalue. input: " << text  << std::endl;
        }
    };


    void test()
    {
        {
            Test test;

            test.explicitCall("test");
            std::as_const(test).explicitCall("constTest");
            std::move(test).explicitCall("std::move(test)");
            std::move(std::as_const(test)).explicitCall("std::move(consTest)");

            // non const lvalue. input: test
            // const lvalue. input: constTest
            // non const rvalue. input: std::move(test)
            // const rvalue. input: std::move(consTest)
        }
        {
            Test test;

            test.implicitCall("test");
            std::as_const(test).implicitCall("constTest");
            std::move(test).implicitCall("std::move(test)");
            std::move(std::as_const(test)).implicitCall("std::move(consTest)");

            // non const lvalue. input: test
            // const lvalue. input: constTest
            // non const rvalue. input: std::move(test)
            // const rvalue. input: std::move(consTest)
        }
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



namespace DeducingThis::Policy_Based_Design
{
    struct Addition
    {
        template <typename T>
        T operation(T a, T b) {
            return a + b;
        }
    };

    struct Multiplication
    {
        template <typename T>
        T operation(T a, T b) {
            return a * b;
        }
    };

    struct Calculator
    {
        template <typename Self, typename T>
        T calculate(this Self&& self, T a, T b) {
            return self.operation(a, b);
        }
    };

    struct Adder : Addition, Calculator  {};
    struct Multiplier : Multiplication, Calculator  {};

    void test()
    {
        Adder adder;
        std::cout << "Addition: " << adder.calculate(3, 4) << std::endl;  // Outputs: Addition: 7

        Multiplier multiplier;
        std::cout << "Multiplication: " << multiplier.calculate(3, 4) << std::endl;  // Outputs: Multiplication: 12
    }
}

namespace DeducingThis::Mixin_Builder
{
    struct NameMixin
    {
        void setName(this auto& self, std::string n) {
            self.name = std::move(n);
        }
    };

    struct AgeMixin
    {
        void setAge(this auto& self, int age) {
            self.age = age;
        }
    };

    struct Person : NameMixin, AgeMixin
    {
        std::string name;
        int age{};

        void printInfo() const {
            std::cout << "Age: " << age << "\n";
            std::cout << "Name: " << name << "\n";
        }
    };

    void test()
    {
        Person p;
        p.setName("AndTokm");
        p.setAge(33);
        p.printInfo();

        // Age: 33
        // Name: AndTokm
    }
}


namespace DeducingThis::UseCases
{
    struct add_postfix_increment
    {
        template <typename Self>
        auto operator++(this Self&& self, int) -> decltype(auto) {
            auto tmp = self;
            ++self;
            return tmp;
        }
    };

    struct Int : add_postfix_increment
    {
        explicit Int(const int i = 0): value(i) {
        }

        Int& operator++(int)
        {
            std::cout << __PRETTY_FUNCTION__ << '\n';
            ++value;
            return *this;
        }

        int value { 0 };
    };

    void OperatorOverloading()
    {
        Int i1 {1}, i2 {2};
        i1++;
        i2++;
        std::cout << i1.value << " " << i2.value << '\n';

        // DeducingThis::UseCases::Int& DeducingThis::UseCases::Int::operator++(int)
        // DeducingThis::UseCases::Int& DeducingThis::UseCases::Int::operator++(int)
        // 2 3
    }
}

void DeducingThis::TestAll()
{
    // Explicit_Object_Member_Functions::simpleExample();

    // Deduplicating_Function_Overloading::test();

    // CRTP_OldStyle::demo();
    // CRTP_NewStyle::demo();
    // Policy_Based_Design::test();
    // Mixin_Builder::test();

    UseCases::OperatorOverloading();
}