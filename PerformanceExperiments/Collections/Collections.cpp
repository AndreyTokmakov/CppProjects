/**============================================================================
Name        : Collections.h
Created on  : 24.04.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : Collections
============================================================================**/

#include <iostream>
#include <memory>
#include <string>
#include <filesystem>
#include <random>
#include <future>
#include <list>
#include <ostream>
#include <vector>
#include <algorithm>

#include "Collections.h"
#include "PerfUtilities.hpp"


namespace Collections
{
    struct SmallObject final
    {
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

        constexpr int size = 10'000'000;
        for (int i = 0; i < size; ++i)
        {
            const int id = DataGenerator::randomIntegerInRange(0, 10 * size);
            list.emplace_back(id);
            vector.emplace_back(id);
        }

        {
            const PerfUtilities::ScopedTimer timer { "Vector" };
            std::sort(vector.begin(), vector.end());
        }
        {
            const PerfUtilities::ScopedTimer timer { "List" };
            list.sort();
        }
    }
}


void Collections::TestAll()
{
    Sort::Vector_vs_List();
}
