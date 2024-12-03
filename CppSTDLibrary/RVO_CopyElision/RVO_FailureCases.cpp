/**============================================================================
Name        : RVO_FailureCases.cpp
Created on  : 09.08.2024
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : RVO_FailureCases
============================================================================**/

#include "RVO_FailureCases.h"

#include <iostream>
#include <array>
#include <numeric>

namespace Common
{
    struct Data
    {
        int value { 0 };

        Data() {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        explicit Data(const int v): value {v} {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        ~Data() {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        Data(const Data&) {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        Data(Data&&) noexcept {
            std::cout << __PRETTY_FUNCTION__ << std::endl;
        }

        Data& operator=(const Data&) {
            std::cout << __PRETTY_FUNCTION__ << std::endl; return *this;
        }

        Data& operator=(Data&&) noexcept {
            std::cout << __PRETTY_FUNCTION__ << std::endl; return *this;
        }

    };
}

namespace RVO_FailureCases::If_Else_Branch
{
    using namespace Common;

    Data getData(const int value, const bool option)
    {
        if (option)
        {
            Data data;
            data.value = value;
            return data;
        }
        else {
            Data data;
            data.value = value * 10;
            return data;
        }
    }

    void Test()
    {
        Data data = getData(10, true);
    }
}

// TODO:  [https://youtu.be/WyxUilrR6fU?t=1136]
//  1 - When the return type is not the same as what's being returned ( Inheritance)
//  2 - Multiple return statements returning DIFF types

void RVO_FailureCases::TestAll()
{
    If_Else_Branch::Test();
}
