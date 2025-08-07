/**============================================================================
Name        : Decay.cpp
Created on  : 07.08.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Decay.cpp
============================================================================**/

#include <type_traits>

void Decay_Tests()
{
    static_assert(std::is_same_v<std::decay<int[5]>::type, int*>);              // int[5] -> int*
    static_assert(std::is_same_v<std::decay<void(int)>::type, void(*)(int)>);   // void(int) -> void(*)(int)
    static_assert(std::is_same_v<std::decay<const int>::type, int>);            // const int -> int
    static_assert(std::is_same_v<std::decay<const int&>::type, int>);           // const int& -> int
}