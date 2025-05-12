/**============================================================================
Name        : FlatMap.cpp
Created on  : 12.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ FlatMap container testing
============================================================================**/

#include "FlatMap.h"

#include <iostream>
#include <flat_map>


namespace FlatMap
{
    void Create()
    {
        std::flat_map<int, std::string> map;

        map.emplace (1, "I");
        map.emplace (2, "II");
        map.emplace (3, "III");
        map.emplace (4, "IV");
        map.emplace (5, "V");

        for (const auto& [k, v] : map) {
            std::cout << k << " = " << v << std::endl;
        }


        map.values()
    }
}

void FlatMap::TestAll()
{
    Create();

};