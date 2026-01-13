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

#include <utility>
#include <memory>

#include <iostream>
#include <vector>
#include <map>
#include <random>

namespace
{
    enum class SortOrder
    {
        Ascending,
        Descending
    };

    // TODO: Concepts on <K>
    //  - Comparable
    template<typename K, typename V, SortOrder ordering = SortOrder::Ascending>
    struct FlatMap
    {
        static_assert(!std::is_same_v<K, void>, "ERROR: Key type can not be void");
        static_assert(!std::is_same_v<V, void>, "ERROR: Value type can not be void");

        struct Node
        {
            K key;
            V value;
        };

        using key_type       = K;
        using value_type     = V;
        using size_type      = uint32_t;
        using node_pointer   = Node*;
        using const_pointer  = const node_pointer;
        using array_type     = Node[];

        explicit FlatMap(const size_type capacity) :
                _size { 0 }, _capacity { capacity }, _data { std::make_unique<array_type>(capacity) }  {
        }

        FlatMap(const FlatMap & other):
                _size { other._size },
                _capacity { other._capacity },
                _data { std::make_unique_for_overwrite<array_type>(_capacity) }
        {
            std::copy_n(other._data.get(), _size, _data.get());
        }

        FlatMap & operator=(const FlatMap & other)
        {
            _size = other._size;
            _capacity = other._capacity;
            _data = std::make_unique_for_overwrite<array_type>(_capacity);
            std::copy_n(other._data.get(), _size, _data.get());

            return *this;
        }

        FlatMap(FlatMap && other) noexcept:
                _size { std::exchange(other._size, 0) },
                _capacity { std::exchange(other._capacity, 0) },
                _data { std::move(other._data) }
        {
        }

        FlatMap & operator=(FlatMap && other) noexcept
        {
            _size = std::exchange(other._size, 0);
            _capacity = std::exchange(other._capacity, 0);
            _data = std::move(other._data);

            return *this;
        }

        node_pointer push(const key_type& key, const value_type& value)
        {
            if (_size > 0 && !compare(key, _data[_size - 1].key))
            {
                if (_size == _capacity)
                    return nullptr;
                _data[_size] = Node {key, value};
                return &(_data[_size++]);
            }

            const size_type idxInsert = findInsertIndex(key);
            if (_capacity == idxInsert || key == _data[idxInsert].key) {
                return &(_data[idxInsert]);
            }

            _size = (_capacity == _size) ? _size : _size + 1;
            // __builtin_prefetch(elements.get() + size - 32, 0, 2);
            for (size_type i = _size - 1; i > idxInsert; --i) { /** TODO: Prefetch **/
                _data[i] = _data[i - 1];
            }
            _data[idxInsert] = Node {key, value};
            return &(_data[idxInsert]);
        }

        bool erase(const key_type& key)
        {
            if (_size > 0 && (!compare(_data[0].key, key) || !compare(key, _data[_size - 1].key)))
            {
                return false;
            }

            const size_type idx = findInsertIndex(key);
            if (key != _data[idx].key) {
                return false;
            }

            for (size_type n = idx + 1; n < _size; ++n) {
                _data[n - 1] = _data[n];
            }
            --_size;
            return true;
        }

        [[nodiscard]]
        size_type size() const noexcept {
            return _size;
        }

        [[nodiscard]]
        size_type empty() const noexcept {
            return 0 == _size;
        }

        [[nodiscard]]
        node_pointer data() const {
            return _data.get();
        }

        [[nodiscard]]
        node_pointer front() const noexcept {
            return &(_data[0]);
        }

        [[nodiscard]]
        node_pointer back() const noexcept {
            return &(_data[_size - 1]);
        }

    private:

        size_type _size { 0 };
        size_type _capacity { 0 };
        std::unique_ptr<array_type> _data { nullptr };

        [[nodiscard]]
        size_type findInsertIndex(const key_type& key) const noexcept
        {
            size_type left = 0, right = _size;
            while (left < right)
            {
                const size_type mid = (left + right) >> 1;
                if (compare(key, _data[mid].key))
                    right = mid;
                else
                    left = mid + 1;
            }
            return left;
        }

        static constexpr bool compare(const key_type& a, const key_type& b) noexcept __attribute__((always_inline))
        {
            if constexpr (SortOrder::Descending == ordering)
                return a >= b;
            else
                return a <= b;
        }
    };
}

namespace flat_map_fast::testing::utils
{
    using ::testing::AssertEqual;
    using ::testing::AssertTrue;

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
