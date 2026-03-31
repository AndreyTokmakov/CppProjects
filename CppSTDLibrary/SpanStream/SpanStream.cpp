/**============================================================================
Name        : SpanStream.cpp
Created on  : 31.03.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : SpanStream.cpp
============================================================================**/

#include "SpanStream.hpp"

#include <sstream>
#include <spanstream>
#include <iostream>
#include <array>
#include <span>

#if 0

void* operator new(std::size_t sz)
{
    std::cout << "  !! Heap allocation: " << sz << " bytes\n";
    return std::malloc(sz);
}

#endif


/**
* std::stringstream is useful but has a dirty secret:
*   it allocates memory behind your back and sometimes reallocates when the data grows.
*   For performance-critical code, embedded systems, or real-time applications, this is unacceptable.
*
* stringstream is convenient but unpredictable.
*
* spanstream   is predictable but unforgiving.
*              That one-line contrast is the entire design philosophy.
*/
void span_stream::TestAll()
{
    {
        std::cout << std::string(120, '-') << std::endl;

        std::stringstream strStream;
        strStream << "one string that doesn't fit into SSO";
        strStream << " and another one for good measure";

        std::cout << strStream.str() << std::endl;

        std::cout << std::string(120, '-') << std::endl;

    }
    {
        std::cout << std::string(120, '-') << std::endl;

        std::array<char, 128> buffer{};
        std::ospanstream spanStream { std::span<char>(buffer) };
        spanStream << "one string that doesn't fit into SSO";
        spanStream << " and another one for good measure";

        std::cout << buffer.data() << std::endl;
        std::cout << std::string(120, '-') << std::endl;
    }

    /**
    ------------------------------------------------------------------------------------------------------------------------
    Heap allocation: 513 bytes
    one string that doesn't fit into SSO and another one for good measure
    ------------------------------------------------------------------------------------------------------------------------
    ------------------------------------------------------------------------------------------------------------------------
    one string that doesn't fit into SSO and another one for good measure
    ------------------------------------------------------------------------------------------------------------------------
    **/
}