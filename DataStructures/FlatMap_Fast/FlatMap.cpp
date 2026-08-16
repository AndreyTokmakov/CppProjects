/**============================================================================
Name        : FlatMap.cpp
Created on  : 13.01.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FlatMap.cpp
============================================================================**/

#include "FlatMap.hpp"
#include "Testing.hpp"

#include <iostream>
#include <vector>
#include <map>
#include <random>

namespace flat_map_fast::testing::utils
{
    using ::utilities::testing::AssertEqual;
    using ::utilities::testing::AssertTrue;
    using namespace flat_map_fast::impl;

    std::random_device rd{};
    std::mt19937 generator = std::mt19937 {rd()};

    int getRandomInRange(const int32_t start, const int32_t end) noexcept
    {
        auto distribution = std::uniform_int_distribution<>{ start, end };
        return distribution(generator);
    }

    template<typename K, typename V >
    std::ostream& operator<<(std::ostream& stream, const std::map<K, V>& map)
    {
        for (const auto & [k, v]: map)
            stream << "{" << k << " = " << v << "} ";
        return stream;
    }

    [[nodiscard]]
    std::vector<int32_t> getTestData(const size_t size = 10'000'000)
    {
        std::vector<int32_t> data(size);
        for (size_t i = 0; i < size; ++i) {
            data[i] = getRandomInRange(0, size);
        }
        return data;
    }

    template<typename K, typename V, SortOrder ordering = SortOrder::Ascending>
    void print(const FlatMap<K, V, ordering>& flatMap)
    {
        for (uint32_t idx = 0; idx < flatMap.size(); ++idx) {
            std::cout << flatMap.data()[idx].key << " ";
        }
        std::cout << std::endl;
    }

}


namespace flat_map_fast::testing
{
    using namespace utils;

    void validation()
    {
        constexpr uint32_t collectionSize { 10 }, testDataSize = 100;
        const std::vector<int32_t> data = getTestData(testDataSize);

        FlatMap<int, int> flatMap (collectionSize);
        for (uint32_t idx = 0; idx < testDataSize; ++idx)
        {
            const auto key = data[idx];
            flatMap.push(key,key * key);
        }

        for (uint32_t idx = 0; idx < collectionSize; ++idx)
        {
            const auto node = flatMap.data()[idx];
            std::cout << "[" << idx << "] = { " << node.key<< " | " << node.value << " } " << std::endl;
        }

        /**
        [0] = { 0 | 0 }
        [1] = { 1 | 1 }
        [2] = { 2 | 4 }
        [3] = { 3 | 9 }
        [4] = { 4 | 16 }
        [5] = { 7 | 49 }
        [6] = { 9 | 81 }
        [7] = { 11 | 121 }
        [8] = { 12 | 144 }
        [9] = { 13 | 169 }
        **/
    }

    void erase_ascending()
    {
        constexpr uint32_t collectionSize { 10 };

        FlatMap<int, int> flatMap (collectionSize);
        for (int i = 40; i >= 0; i -= 2) {
            flatMap.push(i,i * i);
        }

        print(flatMap);

        std::cout << std::string(120, '-') << std::endl;
        flatMap.erase(-1);
        flatMap.erase(0);
        flatMap.erase(1);
        flatMap.erase(8);
        flatMap.erase(9);
        flatMap.erase(18);
        flatMap.erase(21);
        std::cout << std::string(120, '-') << std::endl;

        print(flatMap);
    }

    void erase_complex_ascending()
    {
        using KeyType = int32_t;

        constexpr uint32_t collectionSize { 1000 }, testDataSize = 1'000'000, ratio = 4;
        const std::vector<KeyType> dataToAdd = getTestData(testDataSize);
        const std::vector<KeyType> dataToDelete = getTestData(testDataSize);

        uint32_t deletedCount = 0, insertedCount = 0;
        FlatMap<int, int> flatMap (collectionSize);

        auto isSorted = [&flatMap] {
            return std::is_sorted(flatMap.data(), flatMap.data() + flatMap.size(), [](const auto& a, const auto& b) {
                return b.key >= a.key;
            });
        };

        for (uint32_t idx = 0; idx < testDataSize; ++idx)
        {
            KeyType key = dataToAdd[idx];
            const bool isInserted = flatMap.push(key, key);
            insertedCount += isInserted;

            AssertTrue(isSorted());
            if (idx % ratio != 0)
                continue;

            const decltype(flatMap)::size_type sizePrev = flatMap.size();
            const bool isDeleted = flatMap.erase(dataToDelete[idx]);

            if (!isDeleted) {
                AssertEqual(flatMap.size(), sizePrev);
                continue;
            }
            AssertEqual(flatMap.size(), sizePrev - 1);

            deletedCount += isDeleted;
            AssertTrue(isSorted());
        }

        std::cout << "Inserted: " << insertedCount << ", Deleted: " << deletedCount << std::endl;
    }

    void erase_complex_descending()
    {
        using KeyType = int32_t;

        constexpr uint32_t collectionSize { 1000 }, testDataSize = 1'000'000, ratio = 4;
        const std::vector<KeyType> dataToAdd = getTestData(testDataSize);
        const std::vector<KeyType> dataToDelete = getTestData(testDataSize);

        uint32_t deletedCount = 0, insertedCount = 0;
        FlatMap<int, int, SortOrder::Descending> flatMap (collectionSize);

        auto isSorted = [&flatMap] {
            return std::is_sorted(flatMap.data(), flatMap.data() + flatMap.size(), [](const auto& a, const auto& b) {
                return a.key >= b.key;
            });
        };

        for (uint32_t idx = 0; idx < testDataSize; ++idx)
        {
            KeyType key = dataToAdd[idx];
            const bool isInserted = flatMap.push(key, key);
            insertedCount += isInserted;

            AssertTrue(isSorted());
            if (idx % ratio != 0)
                continue;

            const decltype(flatMap)::size_type sizePrev = flatMap.size();
            const bool isDeleted = flatMap.erase(dataToDelete[idx]);

            if (!isDeleted) {
                AssertEqual(flatMap.size(), sizePrev);
                continue;
            }
            AssertEqual(flatMap.size(), sizePrev - 1);

            deletedCount += isDeleted;
            AssertTrue(isSorted());
        }

        std::cout << "Inserted: " << insertedCount << ", Deleted: " << deletedCount << std::endl;
    }

}



void flat_map_fast::TestAll()
{
    // testing::validation();
    // testing::erase_ascending();
    // testing::erase_complex_ascending();
    testing::erase_complex_ascending();
}
