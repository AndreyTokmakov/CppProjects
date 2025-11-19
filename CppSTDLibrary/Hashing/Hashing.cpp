/**============================================================================
Name        : Hashing.cpp
Created on  : 16.08.2022
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Hashing tests
============================================================================**/


#include <iostream>
#include <string_view>

#include "Hashing.hpp"

namespace Hashing::CustomTypeHash
{
    struct SpaceShip
    {
        int id { 0 };
        std::string name { "Millennium Falcon" };
    };
};

template<>
struct std::hash<Hashing::CustomTypeHash::SpaceShip>
{
    size_t operator()(const Hashing::CustomTypeHash::SpaceShip& ship) const noexcept
    {
        return std::hash<int>()(ship.id) ^ std::hash<std::string>()(ship.name);;
    }
};

void Hashing::TestAll()
{
    TestUtilities();

    /*
    CustomTypeHash::SpaceShip ship;
    std::cout << std::hash<decltype(ship)>{}(ship) << std::endl;
    */
};
