/**============================================================================
Name        : DataPrefetching.cpp
Created on  : 06.11.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : DataPrefetching.cpp
============================================================================**/

#include <iostream>
#include <memory>
#include <vector>

#include "DataPrefetching.hpp"

#include <cstring>

// __builtin_prefetch

void simpleExample()
{
    constexpr int Size { 1024 * 1024 };
    const std::unique_ptr<int[]> data { std::make_unique<int[]>(Size) };

    int64_t sum = 0;
    for (int i = 0; i < Size; ++i) {
        if ((i % 16) == 0)
            __builtin_prefetch(&data[i + 64], 0, 3);  // предзагрузка будущих данных
        sum += data[i];
    }

    std::cout << "Sum = " << sum << "\n";
}


void simpleExample2()
{
    constexpr int PREFETCH_DIST = 16;
    std::vector<int> v(1'000'000, 42);
    long long sum = 0;

    for (size_t i = 0; i < v.size(); ++i) {
        if (i + PREFETCH_DIST < v.size())
            __builtin_prefetch(&v[i + PREFETCH_DIST], 0, 1);
        sum += v[i];
    }
    std::cout << sum << "\n";
}


// Если заранее подгрузить источник, CPU может выполнять копирование почти без pipeline stalls.
void fast_copy(const char* src, char* dst, size_t n)
{
    for (size_t i = 0; i < n; i += 64)
    {
        __builtin_prefetch(src + i + 256, 0, 3);
        __builtin_prefetch(dst + i + 256, 1, 3);
        std::memcpy(dst + i, src + i, 64);
    }
}


namespace iteration_over_structures
{
    struct Order
    {
        double price {};
        int quantity {};
        char side {};
    };


    void demo()
    {
        const std::vector<Order> orders(1000000);
        double total = 0.0;
        constexpr size_t prefetchDistance = 8;

        for (size_t i = 0; i < orders.size(); ++i)
        {
            if (i + prefetchDistance < orders.size()) {
                __builtin_prefetch(&orders[i + prefetchDistance], 0, 1);
            }
            total += orders[i].price * orders[i].quantity;
        }

        std::cout << "total = " << total << "\n";
    }
}


void DataPrefetching::TestAll()
{
    // simpleExample();
    simpleExample2();
    // iteration_over_structures::demo();

    // fast_copy
}
