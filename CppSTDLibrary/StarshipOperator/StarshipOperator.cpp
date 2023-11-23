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

namespace StarshipOperator
{
    struct Custom
    {
        int v {0};   // Default operator==, also provides operator!=

        friend bool operator==(const Custom&, const Custom&) = default;
        // Same as: bool operator==(const Custom&) const = default;
    };

    struct Point
    {
        int x {0};
        int y {0};

        // Default three-way comparison, also defaults operator== if none is declared:
        friend constexpr std::strong_ordering operator<=>(const Point&, const Point&) = default;

        // Same as: auto operator <=>(const Point&) const = default;
    };

    void CompareCustomTypes()
    {
        Point a{3,2}, b{1,3};

        static_assert(std::equality_comparable<Point>);
        std::cout << std::boolalpha << (a > b) << "\n";

        Custom c{4}, d{5};

        static_assert(std::equality_comparable<Custom>);
        std::cout << std::boolalpha << (c != d) << "\n";
    }
}


namespace StarshipOperator::Inheritance
{
    struct Base
    {
        int id;

        std::strong_ordering operator<=>(const Base&) const = default;
    };

    struct Derived : Base {
        std::string data;

        // Default spaceship operator:
        std::strong_ordering operator<=> (const Derived&) const = default;

        /*  Manual implementation with the same semantics:
        auto operator<=>(const Derived& other) const {
            auto cmp = (const Base&)*this <=> (const Base&)other;
            if (std::is_neq(cmp)) // id != other.id
                return cmp;
            return this->data <=> other.data;
        }
        */
    };

    void test()
    {
        Base b1 {2 }, b2 {1 };
        std::cout << std::boolalpha << (b1 > b2) << "\n";


        Derived d1 {2,"hello"}, d2 {2,"bye"};
        std::cout << std::boolalpha << (d1 > d2) << "\n";
    }
}

void StarshipOperator::TestAll()
{
    // OrderingTests();

    CompareCustomTypes();

    // Inheritance::test();

};

