//============================================================================
// Name        : StrongTypes.cpp
// Created on  : 2021-11-09
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Math src
//============================================================================

#include <iostream>
#include "StrongTypes.h"

// TODO: Using 'enum class' instread of bool
namespace StrongTypes::Example1 {

    enum class FirstBooleanType : bool {
    };

    enum class SecondBooleanType : bool {
    };

    std::ostream& operator<<(std::ostream& stream, FirstBooleanType b) {
        stream << std::boolalpha << static_cast<bool>(b);
        return stream;
    }

    std::ostream& operator<<(std::ostream& stream, SecondBooleanType b) {
        stream << std::boolalpha << static_cast<bool>(b);
        return stream;
    }

    void do_something(FirstBooleanType b1, SecondBooleanType b2) {

    }

    void Test()
    {
        FirstBooleanType f1 {true};
        std::cout << f1 << std::endl;

        FirstBooleanType f2 {false};
        std::cout << f2 << std::endl;
    }

    void Test1()
    {
        do_something(FirstBooleanType {true}, SecondBooleanType {true});
#if 0
        // TODO: Error - shall not compile !!!
        do_something(FirstBooleanType {true}, FirstBooleanType {true});
#endif
    }
};

void StrongTypes::TestAll()
{
    Example1::Test();
};