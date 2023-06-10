/**============================================================================
Name        : Int.cpp
Created on  : 09.06.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Int
============================================================================**/

#include "Int.h"

#include <iostream>

namespace Int
{
    void Integer_Literal_Bases()
    {
        // decimal: non-zero decimal digit (1, 2, 3, 4, 5, 6, 7, 8, 9),
        // followed by zero or more decimal digits (0, 1, 2, 3, 4, 5, 6, 7, 8, 9)

        int i = 42;

        std::cout << "i == " << i << "\n";

        // octal: digit zero (0)
        // followed by zero or more octal digits (0, 1, 2, 3, 4, 5, 6, 7)

        int j = 072; // 58 decimal

        std::cout << "j == " << j << "\n";

        // hexadecimal: character sequence 0x or the character sequence 0X
        // followed by one or more hexadecimal digits (0, 1, 2, 3, 4, 5, 6, 7, 8, 9, a, A, b, B, c, C, d, D, e, E, f, F)

        int k = 0XFF; // 255 decimal

        std::cout << "k == " << k << "\n";

        // binary: character sequence 0b or the character sequence 0B
        // followed by one or more binary digits (0, 1)
        // C++14

        int l = 0B1101; // 13 decimal

        std::cout << "l == " << l << "\n";

        // Digits can be separated using ' for improved readability C++14

        int m = 1'000'000;

        std::cout << "m == " << m << "\n";
    }
};

void Int::TestAll()
{
    Integer_Literal_Bases();
};


