/**============================================================================
Name        : main.cpp
Created on  : 29.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Undefined Behavior C++ tests
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <iomanip>
#include <fstream>
#include <charconv>
#include <optional>
#include <algorithm>
#include <numeric>


namespace Numeric
{
    void Char_Sign_Extension()
    {
        // При преобразовании знакового целого меньшей длины к беззнаковому целому большей длины
        // происходит знаковое расширение: старшие биты заполняются битом знака.

        // С/C++ специфичная ловушка: знаковость типа char не специфицирована.
        // В зависимости от платформы он может быть как знаковым, так и беззнаковым.
        // На x86 чаще всего является знаковым.
        // И из char c = 128 получается c = -128.

        // Аналогичная конструкция может быть и при преобразовании int32 -> uint64, и
        // при любых других комбинациях -- только константу правильную писать не забывайте.

        int8_t c = -5;
        uint16_t c_direct_cast = c;
        uint16_t c_two_casts = static_cast<uint8_t>(c);
        std::cout << c_direct_cast << " != " << c_two_casts;
    }

    void Integer_Promotion_Error()
    {
        std::vector<int> v = {1};
        auto idx = -1;
        if (idx < v.size()) { //
            std::cout << "less!\n";
        } else {
            std::cout << "oops!\n";
            std::cout << "idx -> " << static_cast<size_t>(idx) << std::endl;
        }
    }

    void Narrowing()
    {
        auto average = [](const std::vector<int>& v) -> int {
            if (v.empty()) {
                return 0;
            }
            return std::accumulate(v.begin(), v.end(), 0) / v.size();
        };

        std::cout << average({-1,-1,-1}) << std::endl;
    }
}


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // Numeric::Char_Sign_Extension();
    // Numeric::Integer_Promotion_Error();
    Numeric::Narrowing();

    return EXIT_SUCCESS;
}
