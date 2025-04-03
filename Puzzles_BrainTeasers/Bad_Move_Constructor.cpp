/**============================================================================
Name        : Bad_Move_Constructor.cpp
Created on  : 02.04.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Bad_Move_Constructor.h
============================================================================**/

#include "Puzzles.h"
#include <iostream>
#include <exception>

namespace
{
    struct Monster
    {
        Monster() = default;
        Monster(const Monster &other) {
            std::cout << "Monster copied\n";
        }
        Monster(Monster &&other) noexcept {
            std::cout << "Monster moved\n";
        }
    };

    struct Jormungandr : public Monster
    {
        Jormungandr() = default;
        Jormungandr(const Jormungandr &other) : Monster(other){
            std::cout << "Jormungandr copied\n";
        }
        Jormungandr(Jormungandr &&other)  noexcept : Monster(other) { // <--------------- BUG
            std::cout << "Jormungandr moved\n";
        }
    };
}

namespace
{
    void demo()
    {
        Jormungandr jormungandr1;
        Jormungandr jormungandr2 { std::move(jormungandr1) };
    }
}

void Puzzles::Bad_Move_Constructor()
{
    demo();
}

/**
Monster copied
Jormungandr moved

**/