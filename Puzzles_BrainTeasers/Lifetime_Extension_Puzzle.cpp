/**============================================================================
Name        : Lifetime_Extension_Puzzle.cpp
Created on  : 03.02.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Lifetime_Extension_Puzzle.h
============================================================================**/

#include "Puzzles.h"
#include <iostream>

namespace
{
    struct MemoryArea
    {
        int number { 0 };

        MemoryArea(int num) : number {num} {
        }

        ~MemoryArea() {
          std::cout << "Freed memory area " << number << "\n";
        }
    };

    MemoryArea getMemory(int number) {
      return MemoryArea{number};
    }

    struct DataSource
    {
        DataSource(const MemoryArea &memoryArea): memoryAreaRef { memoryArea } {
        }

        const MemoryArea &memoryAreaRef;
    };
}

namespace
{
    void demo()
    {
        const auto &reference1 = getMemory(1);
        std::cout << "Bound reference 1\n";

        const auto &reference2 = getMemory(2).number;
        std::cout << "Bound reference 2\n";

        const auto &reference3 = DataSource(getMemory(3));
        std::cout << "Bound reference 3\n";
    }
}

void Puzzles::Lifetime_Extension_Puzzle()
{
    demo();
}

/**
Bound reference 1
Bound reference 2

Freed memory area 3
Bound reference 3

Freed memory area 2
Freed memory area 1
*/
