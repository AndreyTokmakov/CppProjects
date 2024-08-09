/**============================================================================
Name        : CompressedPair.cpp
Created on  : 09.08.2024
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : CompressedPair
============================================================================**/

#include "CompressedPair.h"
#include <iostream>

namespace CompressedPair
{
    // First
    template<typename T,
             typename Deleter,
             bool hasEmptyBase = std::is_empty_v<Deleter> and not std::is_final_v<Deleter>>
    struct compressed_pair
    {
        T* data {};
        Deleter* deleter {};

        compressed_pair() = default;
        compressed_pair(T* ptr, Deleter* del): data {ptr}, deleter{del} {
        }

        T*   first() { return data; }
        Deleter& second() { return *deleter; }
    };

    // Second
    template<typename T, typename Deleter>
    struct compressed_pair<T, Deleter, true> : public Deleter
    {
        T* data {};

        compressed_pair() = default;
        explicit compressed_pair(T* ptr): data { ptr } {
        }

        T* first() { return data; }
        Deleter& second() { return *this; }
    };

    struct DeleterSimple {};
    struct DeleterSimpleFinal final {};

    template<typename T>
    struct DeleterSimpleComplex final
    {
        int counter = 0;

        void operator()(const T* ptr) {
            delete ptr;
        }
    };

    void Tests()
    {
        {
            compressed_pair<int, DeleterSimple>      pair;  // Second specialization
            std::cout << sizeof(pair) << std::endl;
        }
        {
            compressed_pair<int, DeleterSimpleFinal> pair;  // First
            std::cout << sizeof(pair) << std::endl;
        }
        {
            compressed_pair<int, DeleterSimpleComplex<int>> pair;  // First
            std::cout << sizeof(pair) << std::endl;
        }
    }
}

void CompressedPair::TestAll()
{
    Tests();
}