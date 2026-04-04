/**============================================================================
Name        : FlatMap.cpp
Created on  : 12.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ FlatMap container testing
============================================================================**/

#include "FlatMap.h"

#include <algorithm>
#include <iostream>
#include <deque>
#include <vector>
#include <string>

#include <map>
#include <flat_map>
#include <unordered_map>
#include <boost/container/flat_map.hpp>


#include "Random.hpp"
#include "StringUtilities.hpp"
#include "PerfUtilities.hpp"


namespace FlatMap
{
    void Create()
    {
        std::flat_map<int, std::string> map;

        map.emplace (1, "I");
        map.emplace (2, "II");
        map.emplace (3, "III");
        map.emplace (4, "IV");
        map.emplace (5, "V");

        for (const auto& [k, v] : map) {
            std::cout << k << " = " << v << std::endl;
        }
        std::cout << std::endl;

        {
            const std::vector<int>& keys = map.keys();
            const std::vector<std::string>& values = map.values();

            for (uint32_t size = keys.size(), idx = 0; idx < size; ++idx) {
                std::cout << keys[idx] << " = " << values[idx] << std::endl;
            }
            std::cout << std::endl;
        }

        {
            uint32_t idx = 0;
            for (const auto& [keys, values, size] =
                std::make_tuple(map.keys(), map.values(), map.values().size()); idx < size; ++idx) {
                std::cout << keys[idx] << " = " << values[idx] << std::endl;
            }
            std::cout << std::endl;
        }
    }

    void Custom_Comparators()
    {
        // Use deques instead of vectors as backing storage
        std::flat_map<int, std::string,
                      std::less<>,
                      std::deque<int>,
                      std::deque<std::string>> map;

        map[2] = "Two";
        map[3] = "Three";
        map[1] = "One";

        for (const auto& [key, value] : map) {
            std::cout << key << " = " << value << std::endl;
        }
    }

    void Construct_From_Sorted_Range()
    {
        // You can also construct from a sorted range using the std::sorted_unique tag
        //
        //  it tells the constructor "trust me bro, this data is already sorted and unique,"
        //  so it skips sorting entirely. Use it when you can!

        std::vector<int> keys = {1, 3, 5, 7};
        std::vector<std::string> vals = {"a", "b", "c", "d"};

        const std::flat_map<int, std::string> pre_sorted(
            std::sorted_unique, std::move(keys), std::move(vals)
        );

        for (const auto& [key, value] : pre_sorted) {
            std::cout << key << " = " << value << std::endl;
        }

        // 1 = a
        // 3 = b
        // 5 = c
        // 7 = d
    }
}

namespace FlatMap::performance
{
    using namespace std::string_view_literals;

    template<typename MapType>
    concept Map = requires(MapType& map, typename MapType::key_type key)
    {
        typename MapType::key_type;
        typename MapType::mapped_type;
        typename MapType::value_type;

        { map.emplace() };
        { map.find(key) } -> std::input_iterator;
        { map.begin()   } -> std::input_iterator;
        { map.end()     } -> std::sentinel_for<decltype(map.begin())>;
        { map[key]      } -> std::same_as<typename MapType::mapped_type&>;
    };

    std::vector<std::pair<int, std::string>> getDataSet1(const uint32_t size)
    {
        std::vector<std::pair<int, std::string>> data;
        data.reserve(size);

        for (uint32_t idx = 0; idx < size; ++idx)
        {
            const int key = utilities::random::getRandomInRange(0, static_cast<int>(size * 5));
            std::string const value = StringUtilities::randomString(24);
            data.emplace_back(key, value);
        }

        std::ranges::sort(data);
        return data;
    }

    std::vector<std::pair<int, int>> getDataSet2(const uint32_t size)
    {
        std::vector<std::pair<int, int>> data;
        data.reserve(size);

        for (uint32_t idx = 0; idx < size; ++idx)
        {
            const int key = utilities::random::getRandomInRange(0, static_cast<int>(size * 5));
            const int value = utilities::random::getRandomInRange(0, static_cast<int>(size * 5));
            data.emplace_back(key, value);
        }

        std::ranges::sort(data);
        return data;
    }

    template<Map map_t, typename K, typename V>
    void benchmark_find(const std::vector<std::pair<K, V>>& data,
                        const std::string_view name,
                        const uint64_t iterations)
    {
        map_t map;
        std::ranges::for_each(data, [&](const auto& entry) {
            map[entry.first] = entry.second;
        });

        std::vector<int> keys = data | std::views::elements<0> | std::ranges::to<std::vector>();
        std::ranges::shuffle(keys, std::default_random_engine {});

        volatile uint64_t sink = 0;
        PerfUtilities::ScopedTimer timer { name };
        for (uint64_t i = 0; i < iterations; ++i ) {
            for (const auto& key : keys) {
                map.find(key);
                ++sink;
            }
        }
    }

    void find_int_string()
    {
        constexpr uint32_t datasetSize = 1'000;
        constexpr uint64_t testIterationsCount = 100'000;

        const std::vector<std::pair<int, std::string>> data = getDataSet1(datasetSize);

        benchmark_find<std::map<int, std::string>>(data, "map"sv, testIterationsCount);
        benchmark_find<std::flat_map<int, std::string>>(data, "flat_map"sv, testIterationsCount);
        benchmark_find<std::unordered_map<int, std::string>>(data, "unordered_map"sv, testIterationsCount);

        // map           :  0.0257794 seconds.
        // flat_map      :  0.870707 seconds.
        // unordered_map :  0.0242558 seconds.
    }

    void find_int_int()
    {
        constexpr uint32_t datasetSize = 1'000;
        constexpr uint64_t testIterationsCount = 100'000;

        const std::vector<std::pair<int, int>> data = getDataSet2(datasetSize);

        benchmark_find<std::map<int, int>>(data, "map"sv, testIterationsCount);
        benchmark_find<std::flat_map<int, int>>(data, "flat_map"sv, testIterationsCount);
        benchmark_find<std::unordered_map<int, int>>(data, "unordered_map"sv, testIterationsCount);

        // map           :  0.0257794 seconds.
        // flat_map      :  0.870707 seconds.
        // unordered_map :  0.0242558 seconds.
    }
}

void FlatMap::TestAll()
{
    // Create();
    // Custom_Comparators();
    // Construct_From_Sorted_Range();

    performance::find_int_string();
    // performance::find_int_int();
};