/**============================================================================
Name        : StaticAsserts.cpp
Created on  : 14.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StaticAsserts.cpp
============================================================================**/

#include "StaticAsserts.h"

#include <iostream>
#include <string_view>


namespace StaticAsserts
{
    template<typename... T>
    constexpr auto sum(T... args)
    {
        return (args + ...);
    }

    constexpr int getValue_Leak()
    {
        int* intPtr = new int(123);
        return *intPtr;
    }

    constexpr int getValue_NoLeak()
    {
        int* intPtr = new int(123);
        int result = *intPtr;
        delete intPtr;
        return result;
    }

    void SimpleTests()
    {
        static_assert(1 == sum(1));
        static_assert(3 == sum(1, 2));
        static_assert(6 == sum(1, 2, 3));
    }

    void Assert_Lambdas()
    {

        static_assert([] { return true; }() == true);

        static_assert(([] {
            constexpr int s1 = 1;
            constexpr int s2 = 3;
            static_assert(s1 == sum(1));
            static_assert(s2 == sum(1, 2));
        }(), true));


        /** Will not compile **/
        // static_assert(123 == getValue_Leak());

        static_assert(123 == getValue_NoLeak());


        /*
        static_assert(consteval {
            constexpr std::vector<int> v(5);
            static_assert(1 == 1);
        });*/
    }


    template<typename K = std::string,
            typename V = std::string, size_t _Size = 5>
    struct LRUCache
    {
        using KeyType = K;
        using ValueType = V;

        static_assert(!std::is_same_v<KeyType, void > , "ERROR: Key type can not be void");
        static_assert(!std::is_same_v<ValueType, void > , "ERROR: Value type can not be void");
        static_assert(0 != _Size, "ERROR: Please try a little bigger buffer");
    };


    void Assert_Types()
    {
        [[maybe_unused]]
        LRUCache<int, int, 10> cache;

        /// LRUCache<int, int, 0>   cache1;   <--- Non compile
        /// LRUCache<void, int, 10> cache2;   <--- Non compile
        /// LRUCache<int, void, 10> cache3;   <--- Non compile
    }
}

void StaticAsserts::TestAll()
{
    SimpleTests();
    Assert_Lambdas();
    Assert_Types();
}
