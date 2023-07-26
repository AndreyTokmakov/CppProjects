//============================================================================
// Name        : Auto.cpp
// Created on  : 28.02.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Auto src class
//============================================================================

#include <iostream>
#include <string>
#include <thread>
#include <type_traits>

#include "Auto.h"

namespace Auto {

    class Base {

    };

    class Derived: public Base {

    };

    Base buildBase() {
        return {};
    }

    auto print_type = [](auto T) -> void {
        std::cout << typeid(T).name() << "\n" << std::endl;
    };

    void foo1(const Derived& obj) {

        //std::cout << std::is_reference_v<obj> << std::endl;
        print_type(obj);

        auto a = static_cast<const Base&>(obj);
        print_type(a);

        auto &b = static_cast<const Base&>(obj);
        print_type(b);

        decltype(auto) c = static_cast<const Base&>(obj);
        print_type(c);
    }


    void SimpleTest() {
        auto a = 1;
        print_type(a);

        auto b = "Test";
        print_type(b);

        auto c = buildBase();
        print_type(c);

        Derived d;
        foo1(d);
    }

}

namespace Auto
{
    struct Object
    {
        Object() { std::cout << "Object::Object()" << std::endl; }
        ~Object() { std::cout << "~Object::Object()" << std::endl; }

        Object(const Object&) { std::cout << "Object::Object(const Object&)" << std::endl; }
        Object(Object&&) noexcept { std::cout << "Object::Object(Object&&)" << std::endl; }

        Object& operator=(const Object&) {
            std::cout << "Object::Object(const Object&)" << std::endl;
            return *this;
        }

        Object& operator=(Object&&) noexcept {
            std::cout << "Object::Object(Object&&)" << std::endl;
            return *this;
        }
    };

    Object tmp;

    Object& getObject() {
        return tmp;
    }

    void Test_GetReference()
    {
        Object& obj = getObject();
    }

    void Test_LoseReference_Copy()
    {
        // 'auto obj' --> 'Object obj' [not a ref]
        auto obj = getObject();
    }
}


void Auto::TestAll()
{
    // Auto::SimpleTest();

    // Test_GetReference();
    Test_LoseReference_Copy();
};
