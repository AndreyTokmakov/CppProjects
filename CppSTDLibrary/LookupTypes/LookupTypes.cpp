/**============================================================================
Name        : LookupTypes.cpp
Created on  : 30.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : LookupTypes.cpp
============================================================================**/

#include "LookupTypes.h"

#include <iostream>
#include <print>

constexpr int var = 1001;


namespace LookupTypes::Qualified_Name_Lookup
{
    /**
     * The term qualified refers to symbols that are explicitly scoped using the :: operator.
     * In other words, these are names that appear to the right of a ::, such as x in a::b::x.
     */

    namespace c
    {
        int x = 3;
        int z = 13;
    }

    namespace a
    {
        int x = 1;
        int w = 66;

        namespace d
        {
            int y = 42;
        }

        namespace b
        {
            using namespace c;
            int x = 2;
            int y = d::y;
        }
    }

    void demo()
    {
        std::println("{}", a::b::x);  // 2
        // std::println("{}", a::b::w);       // ERROR
        std::println("{}", a::b::z);  // 13
    }
}

namespace LookupTypes::Unqualified_Name_Lookups
{

    void demo()
    {
        constexpr int var = 3;

        constexpr int y = var;    // Unqualified name lookup finds local var = 3
        static_assert(3 == y);

        constexpr int z = ::var;  // Qualified name lookup finds global var = 2
        static_assert(1001 == z);
    }


    struct Widget
    {
        [[nodiscard]]
        int func() const
        {
            // No local x; lookup finds member x
            int y = x;
            return y;
        }
        int x = 3;
    };
}

namespace LookupTypes::Argument_Dependent_Lookups_ADL
{
    namespace math_one
    {
        enum class Integer {};
        // enum class Double {};

        constexpr int normalize(Integer) {
            return 1;
        }

        constexpr bool operator==(Integer, Integer) {
            return true;
        }
    }


    void demo()
    {

        constexpr math_one::Integer intVar1 {1}, intVar2 {2};

        static_assert(true == normalize(intVar1));  // OK via ADL
        static_assert(intVar1 == intVar2);          // OK via ADL

        /**
         * Even though normalize and operator== are in the math_one namespace, they are found without qualification because
         * the arguments are of type math_one::Integer. Thanks to ADL, function within the math_one namespace are brought into scope.
         */
    }
}



void LookupTypes::TestAll()
{
    // Qualified_Name_Lookup::demo();
    // Unqualified_Name_Lookups::demo();


    Argument_Dependent_Lookups_ADL::demo();
}