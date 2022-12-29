//============================================================================
// Name        : Byte.cpp
// Created on  : 02.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Byte src
//============================================================================

#include "Byte.h"

#include <iostream>

namespace Byte {
    template <typename T>
    auto only_return_int_type_bar(std::byte& b) {
        return T(b);
    }

    /*
    void BaseTests()
    {
        std::byte test {64};

        std::cout << only_return_int_type_bar<int>(test) << std::endl;
        std::cout << only_return_int_type_bar<float>(test) << std::endl;

        std::byte b{ 42 };

        std::cout << atoi(static_cast<char>(b)) << std::endl;
        b <<= 1;
        std::cout << std::to_integer<int>(b) << std::endl;
    }

    void ToInt()
    {
        std::byte b { 32 };
        std::cout << std::to_integer<int>(b) << std::endl;

        // b *= 2 compilation error
        b <<= 1;
        std::cout << std::to_integer<int>(b) << std::endl;
    }
    */
};

namespace Byte::Size {

    class Class_3_Byte {
        std::byte b1;
        std::byte b2;
        std::byte b3;
    };

#pragma pack(push, 1)
    class Class1 {
        std::byte b1;
        std::byte b2;

        int var;
    };
#pragma pack(pop)


    void Test() {
        std::cout << "Class_3_Byte: " << sizeof(Class_3_Byte) << std::endl;
        std::cout << "Class1: " << sizeof(Class1) << std::endl;
    }
}

void Byte::TestAll() {
    // ToInt();
    // BaseTests();

    Size::Test();
};