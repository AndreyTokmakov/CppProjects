/**============================================================================
Name        : StarshipOperator.h
Created on  : 17.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ StarshipOperator
============================================================================**/

#include "StarshipOperator.h"

#include <iostream>
#include <cmath>


namespace StarshipOperator
{
    struct Coord {
        int x;
        int y;
        friend auto operator<=>(const Coord&, const Coord&) = default;
    };

    void OrderingTests()
    {
        // Integral types are strongly ordered
        static_assert(std::is_same_v<decltype(0 <=> 1), std::strong_ordering>);

        // Aggregates formed from strongly ordered components end up also strongly ordered
        static_assert(std::is_same_v<decltype(Coord{0,0} <=> Coord{1,1}), std::strong_ordering>);

        // Notably floating-point numbers are only partially ordered:
        static_assert(std::is_same_v<decltype(-0.0<=>0.0), std::partial_ordering>);

        bool r1 = -0.0 == 0.0;
        bool r2 = std::signbit(-0.0) == std::signbit(0.0);

        // r1 == true, r2 == false
        // -0.0 and 0.0 are equivalent, but distinguishable
        std::cout << std::boolalpha << "r1 == " << r1 << ", r2 == " << r2 << "\n";

        bool r3 = NAN == 0.0 || NAN < 0.0 || NAN > 0.0;

        // r3 == false, NaN is unordered
        std::cout << std::boolalpha << "r3 == " << r3 << "\n";
    }

};

void StarshipOperator::TestAll()
{
    OrderingTests();
};

