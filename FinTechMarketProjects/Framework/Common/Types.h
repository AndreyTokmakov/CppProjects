/**============================================================================
Name        : Types.h
Created on  : 10.04.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Types.h
============================================================================**/

#ifndef CPPPROJECTS_TYPES_H
#define CPPPROJECTS_TYPES_H

#include <concepts>

namespace Common
{
    template <typename T>
    concept Numeric = std::is_arithmetic_v<T>;

    enum class OrderSide : char
    {
        Buy,
        Sell
    };
}

#endif //CPPPROJECTS_TYPES_H
