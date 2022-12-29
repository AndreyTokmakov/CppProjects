//============================================================================
// Name        : AggregateInitialization.cpp
// Created on  : 2021-11-06.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : AggregateInitialization
//============================================================================

#include <iostream>
#include <string>
#include <string_view>
#include "AggregateInitialization.h"

namespace AggregateInitialization {

    class Object1 {
    public:
        int a {0};
        int b {0};
        int c {0};

    public:
        friend std::ostream& operator<<(std::ostream& stream, const Object1& obj);
    };

    std::ostream& operator<<(std::ostream& stream, const Object1& obj) {
        stream << "Object(" << obj.a << ", " << obj.b << ", " << obj.c << ")";
        return stream;
    }


    void InitObjectTest()
    {
        Object1 obj{1, 2, 3};
        std::cout << obj<< std::endl;

        Object1 obj1{ 1 };
        std::cout << obj1 << std::endl;

        Object1 obj2 {.b = 22, .c = 333 };
        std::cout << obj2 << std::endl;

    }


    class Data {
    public:
        std::string name;
        double value;
    };

    class MoreData : public Data {
    public:
        bool done;
    };

    void Complex_Object()
    {


        MoreData data{ {"test1", 6.778}, false };
        std::cout << data.name << " " << data.value << " " << std::boolalpha << data.done << std::endl;

        MoreData data2{ "test2", 1.778, true };
        std::cout << data2.name << " " << data2.value << " " << std::boolalpha << data2.done << std::endl;
    }
}

void AggregateInitialization::TestAll() {
    // InitObjectTest();
    Complex_Object();
};
