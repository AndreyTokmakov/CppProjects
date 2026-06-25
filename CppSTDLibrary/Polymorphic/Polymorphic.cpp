/**============================================================================
Name        : Polymorphic.cpp
Created on  : 25.06.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Polymorphic.cpp
============================================================================**/

#include <iostream>
#include <vector>
//#include <polymorphic> // C++26 header

#include "Polymorphic.hpp"

namespace
{
    struct  Enemy
    {
        virtual ~Enemy() = default;
        virtual void attack() const { std::cout << "Basic punch!\n"; }
    };

    struct Boss : Enemy
    {
        void attack() const override { std::cout << "Fire breath!\n"; }
    };

    void simpleTest()
    {
#if 0
        std::vector<std::polymorphic<Enemy>> level_enemies;

        // We tell it to build a Boss on the heap.
        // std::in_place_type is just a tag telling the compiler what class to make.
        level_enemies.push_back(std::polymorphic<Enemy>(std::in_place_type<Boss>));

        // We access it with an arrow -> just like a pointer.
        level_enemies[0]->attack(); // Outputs: Fire breath!

        // The copy operation automatically preserves the dynamic type
        // of every stored object.
        // Every single polymorphic object inside automatically deep-copies itself.
        // No clone() methods, no manual for-loops.
        std::vector<std::polymorphic<Enemy>> backup = level_enemies;

        backup[0]->attack(); // Outputs: Fire breath!
#endif

    }
}

void Polymorphic::TestAll()
{

}