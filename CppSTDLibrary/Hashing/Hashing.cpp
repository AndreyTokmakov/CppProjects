//============================================================================
// Name        : Hashing.h
// Created on  : 16.08.22.
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Hashing tests
//============================================================================

#include <iostream>
#include <string_view>

#include "Hashing.h"

namespace Hashing::CustomTypeHash
{
    struct SpaceShip
    {
        int id { 0 };
        std::string name { "Millennium Falcon" };
    };
};

template<>
struct std::hash<Hashing::CustomTypeHash::SpaceShip> {
    size_t operator()(const Hashing::CustomTypeHash::SpaceShip& ship) const {
        return std::hash<int>()(ship.id) ^ std::hash<std::string>()(ship.name);;
    }
};

void Hashing::TestAll()
{
    CustomTypeHash::SpaceShip ship;
    std::cout << std::hash<decltype(ship)>{}(ship) << std::endl;

};
