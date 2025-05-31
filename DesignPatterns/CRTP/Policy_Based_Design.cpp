/**============================================================================
Name        : Policy_Based_Design.cpp
Created on  : 11.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Policy_Based_Design.cpp
============================================================================**/

#include "CRTP.h"
#include <iostream>

namespace CRTP::Policy_Based_Design
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

    template<typename Derived>
    struct Calculator
    {
        template <typename T>
        T calculate(T a, T b) {
            return static_cast<Derived&>(*this).operation(a, b);
        }
    };

    struct Adder : Addition, Calculator<Adder>  {};
    struct Multiplier : Multiplication, Calculator<Multiplier>  {};

    void test()
    {
        Adder adder;
        std::cout << "Addition: " << adder.calculate(3, 4) << std::endl;  // Outputs: Addition: 7

        Multiplier multiplier;
        std::cout << "Multiplication: " << multiplier.calculate(3, 4) << std::endl;  // Outputs: Multiplication: 12
    }
}

void CRTP::Policy_Based_Design::TestAll()
{
    test();
}
