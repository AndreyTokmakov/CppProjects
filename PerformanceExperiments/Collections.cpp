//============================================================================
// Name        : Collections.h
// Created on  : 24.04.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Collections
//============================================================================



#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <tuple>
#include <ranges>

#include <thread>
#include <random>
#include <future>

#include <mutex>
#include <atomic>

#include <cmath>
#include <list>
#include <numeric>
#include <numbers>
#include <ostream>

#include <vector>

#include "Collections.h"

namespace Collections
{
#define START_TIME_MEASURE auto start = std::chrono::high_resolution_clock::now();
#define STOP_TIME_MEASURE  { auto end = std::chrono::high_resolution_clock::now(); \
                           auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
						   std::cout << "Result: " << duration << " microseconds" << std::endl;}

    struct SmallObject final {
        size_t id {0};

        SmallObject(size_t id): id {id} {}

        bool operator==(const SmallObject& order) const noexcept {
            return this->id == order.id;
        }

        bool operator<(const SmallObject& order) const noexcept {
            return this->id < order.id;
        }
    };



    struct HeavyObject final {
        size_t id {0};

        std::string s1 {0};
        std::string s2 {0};
        std::string s3 {0};
        std::string s4 {0};
        std::string s5 {0};

        explicit HeavyObject(size_t id): id {id} {}

        bool operator==(const HeavyObject& order) const noexcept {
            return this->id == order.id;
        }

        bool operator<(const HeavyObject& order) const noexcept {
            return this->id < order.id;
        }
    };

    struct VeryHeavyObject final {
        size_t id {0};

        char buffer[1024] {};

        //  explicit VeryHeavyObject(size_t id): id {id} {}

        bool operator==(const VeryHeavyObject& order) const noexcept {
            return this->id == order.id;
        }

        bool operator<(const VeryHeavyObject& order) const noexcept {
            return this->id < order.id;
        }
    };
}

namespace Collections::DataGenerator {
    static std::random_device randomDevice{};
    static std::mt19937 generator(randomDevice());

    [[nodiscard]]
    int randomIntegerInRange(int from, int until) {
        return std::uniform_int_distribution<int>{from, until}(generator);
    }

    [[nodiscard]]
    double randomDoubleInRange(double from, double until) {
        return std::uniform_real_distribution<double>{from, until}(generator);
    }

    [[nodiscard]]
    std::string randomString(int size = 16) {
        auto ud = std::uniform_int_distribution<>{(int) 'A', (int) 'z'};

        std::string str(size, '\0');
        std::for_each(str.begin(), str.end(), [&](char &c) { c = static_cast<char>(ud(generator)); });
        return str;
    }
}

namespace Collections::Sort
{
    void Vector_vs_List()
    {
        // using Type = SmallObject;
        // using Type = HeavyObject;
        using Type = VeryHeavyObject;

        std::list<Type> list {};
        std::vector<Type> vector {};

        int size = 10'000'000;
        for (int i = 0; i < size; ++i) {
            const int id = DataGenerator::randomIntegerInRange(0, 10 * size);
            // std::cout << id << std::endl;

            list.emplace_back(id);
            vector.emplace_back(id);
        }

        {
            START_TIME_MEASURE;
            std::sort(vector.begin(), vector.end());
            STOP_TIME_MEASURE;
        }
        {
            START_TIME_MEASURE;
            list.sort();
            STOP_TIME_MEASURE;
        }

        std::cout << "Done\n";
    }
}


void Collections::TestAll()
{
    Sort::Vector_vs_List();
}
