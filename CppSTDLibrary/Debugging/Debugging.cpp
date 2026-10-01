/**============================================================================
Name        : Debugging.cpp
Created on  : 01.10.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Debugging.cpp
============================================================================**/

#include "Debugging.hpp"

#if defined(_MSC_VER)
    __debugbreak();
#elif defined(__clang__)
__builtin_debugtrap();
#elif defined(__GNUC__) && (defined(__i386__) || defined(__x86_64__))
__asm__ volatile("int3");
#else
raise(SIGTRAP);
#endif

void debugging::TestAll()
{

}
