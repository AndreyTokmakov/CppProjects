/**============================================================================
Name        : MultiMap.cpp
Created on  : 15.04.2020
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : MultiMap
============================================================================**/

#include "MultiMap.h"

#include <iostream>
#include <map>
#include <string>

namespace
{
    template<typename K, typename V, typename Comparator>
    std::ostream& operator<<(std::ostream& stream, const std::multimap<K,V,Comparator>& map)
    {
        for (const auto& [k, v] : map)
            stream << k << ", " << v << '\n';
        return stream;
    }
}

namespace MultiMap
{
    void Add_Iterate_Order()
    {
        std::multimap<int, std::string> dict;

        dict.emplace(1, "One");
        dict.emplace(2, "Two_0");
        dict.emplace(3, "Three");

        dict.emplace(2, "Two_1");
        dict.emplace(2, "Two_2");


        std::cout << dict << std::endl;
    }
}


void MultiMap::TestAll()
{
    Add_Iterate_Order();
};
