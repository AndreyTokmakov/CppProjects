/**============================================================================
Name        : AOS_VS_SOA_H.cpp
Created on  : 21.03.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AOS_VS_SOA_H.h
============================================================================**/

#include "AoS_vs_SoA.h"
#include "../Utilities.h"

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <string_view>
#include <mutex>
#include <atomic>
#include <numeric>
#include <chrono>
#include <algorithm>

namespace
{
    struct Entity
    {
        int a { 0 };
        int b { 0 };
        int c { 0 };
        int d { 0 };
        int e { 0 };
    };

    using SoAEntity = std::vector<Entity>;

    struct AoSEntity
    {
        std::vector<int> aValues {  };
        std::vector<int> bValues {  };
        std::vector<int> cValues {  };
        std::vector<int> dValues {  };
        std::vector<int> eValues {  };
    };


    constexpr uint64_t iterCount { 1'000'000 };
    constexpr uint64_t size { 10000 };

    void test_SoA(SoAEntity& entities)
    {
        Utilities::ScopedTimer timer { "SoA" };
        for (uint64_t n = 0; n < iterCount; ++n) {
            for (Entity& entity : entities)
            {
                entity.a += 1;
                entity.b += 1;
                entity.c += 1;
                entity.d += 1;
                entity.e += 1;
            }
        }
    }

    void test_AoS(AoSEntity& entities)
    {
        Utilities::ScopedTimer timer { "AoS" };
        for (uint64_t n = 0; n < iterCount; ++n)
        {
            for (int& val : entities.aValues) {
                val += 1;
            }
            for (int& val : entities.bValues) {
                val += 1;
            }
            for (int& val : entities.cValues) {
                val += 1;
            }
            for (int& val : entities.dValues) {
                val += 1;
            }
            for (int& val : entities.eValues) {
                val += 1;
            }
        }
    }

    void benchmark()
    {
        SoAEntity soa(size);
        AoSEntity aos {.aValues = std::vector<int>(size),
                       .bValues = std::vector<int>(size),
                       .cValues = std::vector<int>(size),
                       .dValues = std::vector<int>(size),
                       .eValues = std::vector<int>(size)
        };

        test_AoS(aos);
        test_SoA(soa);


    }
}


void AoS_vs_SoA::TestAll()
{
    benchmark();
}
