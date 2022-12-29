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

void Auto::TestAll() {

    Auto::SimpleTest();
};
