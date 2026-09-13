/**============================================================================
Name        : StaticTable.cpp
Created on  : 05.09.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : StaticTable.cpp
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>
#include <chrono>
#include <filesystem>
#include <print>
#include <functional>
#include <cassert>


#include "StaticTable.hpp"
#include "Testing.hpp"

namespace static_table
{
    template<typename T>
    concept StaticTableKey = std::copy_constructible<T> &&
        requires(const T& lhs, const T& rhs)
    {
        { lhs == rhs } -> std::convertible_to<bool>;
        { lhs < rhs } -> std::convertible_to<bool>;
    };

    template<typename T>
    concept StaticTableValue = std::copy_constructible<T> &&
        requires(const T& lhs, const T& rhs)
    {
        { lhs == rhs } -> std::convertible_to<bool>;
    };


    template<StaticTableKey K,
             StaticTableValue V,
             std::size_t N,
             std::size_t BinarySearchThreshold = 16>
    class StaticTable
    {
    public:
        using key_type = K;
        using value_type = V;
        using Entry = std::pair<key_type, value_type>;
        using Storage = std::array<Entry, N>;

        template<typename... Entries>
            requires ( sizeof...(Entries) == N && (std::constructible_from<Entry, Entries> && ...))
        constexpr explicit StaticTable(Entries&&... entries) noexcept
            : mData{std::forward<Entries>(entries)...}
        {
            sortByKey();
        }

        constexpr explicit StaticTable(Storage data) noexcept : mData(std::move(data))
        {
            sortByKey();
        }

        constexpr StaticTable(std::initializer_list<Entry> entries) noexcept
        {
            //assert(entries.size() == N);
            std::copy(entries.begin(), entries.end(), mData.begin());
            sortByKey();
        }

        template<std::ranges::input_range Range>
            requires std::convertible_to< std::ranges::range_value_t<Range>, Entry>
        constexpr explicit StaticTable(Range&& range) noexcept
        {
            auto it = std::ranges::begin(range);
            auto const last = std::ranges::end(range);

            for (std::size_t i = 0; i < N; ++i)
            {
                assert(it != last);
                mData[i] = *it;
                ++it;
            }

            assert(it == last);
            sortByKey();
        }

        [[nodiscard]]
        constexpr std::optional<value_type> findByKey(const key_type& key) const noexcept
        {
            if constexpr (N > BinarySearchThreshold)
                return findByKeyBinary(key);
            return findByKeyLinear(key);
        }

        [[nodiscard]]
        constexpr std::optional<key_type> findByValue(const value_type& value) const noexcept {
            return findByValueLinear(value);
        }

        [[nodiscard]]
        constexpr bool containsKey(const key_type& key) const noexcept
        {
            // TODO: how to find right N
            if constexpr (N > BinarySearchThreshold)
                return containsByKeyBinary(key);
            return containsByKeyLinear(key);
        }

        [[nodiscard]]
        constexpr bool containsValue(const value_type& value) const noexcept
        {
            return containsByValueLinear(value);
        }

        [[nodiscard]]
        constexpr Storage const& data() const noexcept {
            return mData;
        }

        [[nodiscard]]
        static constexpr std::size_t size() noexcept {
            return N;
        }

        [[nodiscard]]
        constexpr auto begin() const noexcept
        {
            return mData.cbegin();
        }

        [[nodiscard]]
        constexpr auto end() const noexcept
        {
            return mData.cend();
        }

        [[nodiscard]]
        constexpr auto cbegin() const noexcept
        {
            return mData.cbegin();
        }

        [[nodiscard]]
        constexpr auto cend() const noexcept
        {
            return mData.cend();
        }

    private:

        constexpr void sortByKey() noexcept
        {
            std::sort(mData.begin(),mData.end(),[]
                    (const Entry& lhs, const Entry& rhs) constexpr noexcept{
                return lhs.first < rhs.first;
            });
        }

        [[nodiscard]]
        constexpr std::optional<value_type> findByKeyLinear(const key_type& key) const noexcept
        {
            for (auto const& [currentKey, value] : mData){
                if (currentKey == key)
                    return value;
            }
            return std::nullopt;
        }

        [[nodiscard]]
        constexpr std::optional<value_type> findByKeyBinary(const key_type& key) const noexcept
        {
            auto const it = std::lower_bound(mData.begin(), mData.end(), key,
            [](const Entry& entry, const key_type& value) constexpr noexcept {
                    return entry.first < value;
            });
            if (it != mData.end() && it->first == key)
                return it->second;

            return std::nullopt;
        }

        [[nodiscard]]
        constexpr std::optional<key_type> findByValueLinear(const value_type& value) const noexcept
        {
            for (auto const& [key, currentValue] : mData) {
                if (currentValue == value)
                    return key;
            }
            return std::nullopt;
        }

        [[nodiscard]]
        constexpr bool containsByKeyLinear(const key_type& key) const noexcept
        {
            for (auto const& [currentKey, value] : mData) {
                if (currentKey == key)
                    return true;
            }
            return false;
        }

        [[nodiscard]]
        constexpr bool containsByKeyBinary(const key_type& key) const noexcept
        {
            auto const it = std::lower_bound(mData.begin(), mData.end(), key,
                [](const Entry& entry, const key_type& value) constexpr noexcept {
                    return entry.first < value;
            });
            return it != mData.end() && it->first == key;
        }

        [[nodiscard]]
        constexpr bool containsByValueLinear(const value_type& value) const noexcept
        {
            for (auto const& [key, currentValue] : mData) {
                if (currentValue == value)
                    return true;
            }
            return false;
        }

    private:

        Storage mData;
    };

    template<typename K, typename V>
    concept StaticTableEntry =
        std::same_as<std::remove_cvref_t<K>, K> &&
        std::same_as<std::remove_cvref_t<V>, V> &&
        StaticTableKey<K> &&
        StaticTableValue<V> &&
        std::constructible_from<std::pair<K, V>, K, V>;


    template<typename First, typename... Rest>
    requires requires
    {
        typename std::remove_cvref_t<First>::first_type;
        typename std::remove_cvref_t<First>::second_type;
    }
    constexpr auto makeStaticTable(First&& first, Rest&&... rest) noexcept
    {
        using FirstEntry = std::remove_cvref_t<First>;
        using K = FirstEntry::first_type;
        using V = FirstEntry::second_type;
        using Entry = std::pair<K, V>;

        static_assert(StaticTableEntry<K, V>, "StaticTable key/value types do not satisfy StaticTable requirements");
        static_assert(std::constructible_from<Entry, First>, "First element cannot be used to construct the table entry");
        static_assert((std::constructible_from<Entry, Rest> && ...), "One or more elements cannot be used to construct the table entry");
        constexpr std::size_t N = 1 + sizeof...(Rest);

        return StaticTable<K, V, N>(std::forward<First>(first), std::forward<Rest>(rest)...);
    }
}


namespace static_table::testing
{
    using namespace utilities::testing;
    using namespace std::string_view_literals;

    constexpr void testConstruction()
    {
        constexpr StaticTable<int, int, 5> table{
            {30, 300},
            {10, 100},
            {50, 500},
            {20, 200},
            {40, 400}
        };

        AssertEqual(5UZ, table.size());

        auto const& data = table.data();

        AssertEqual(10, data[0].first);
        AssertEqual(100, data[0].second);

        AssertEqual(20, data[1].first);
        AssertEqual(200, data[1].second);

        AssertEqual(30, data[2].first);
        AssertEqual(300, data[2].second);

        AssertEqual(40, data[3].first);
        AssertEqual(400, data[3].second);

        AssertEqual(50, data[4].first);
        AssertEqual(500, data[4].second);
    }

    void testFindByKeyLinear()
    {
        constexpr StaticTable<int, int, 5> table{
            {30, 300},
            {10, 100},
            {50, 500},
            {20, 200},
            {40, 400}
        };

        AssertTrue(table.findByKey(10) == std::optional{100});
        AssertTrue(table.findByKey(20) == std::optional{200});
        AssertTrue(table.findByKey(30) == std::optional{300});
        AssertTrue(table.findByKey(40) == std::optional{400});
        AssertTrue(table.findByKey(50) == std::optional{500});

        AssertFalse(table.findByKey(0).has_value());
        AssertFalse(table.findByKey(25).has_value());
        AssertFalse(table.findByKey(100).has_value());
    }



    void testFindByKeyBinary()
    {
        constexpr StaticTable<int, int, 17> table{
            {170, 1700},
            {30, 300},
            {100, 1000},
            {50, 500},
            {160, 1600},
            {10, 100},
            {140, 1400},
            {70, 700},
            {120, 1200},
            {20, 200},
            {150, 1500},
            {80, 800},
            {40, 400},
            {130, 1300},
            {60, 600},
            {110, 1100},
            {90, 900}
        };

        AssertTrue(table.findByKey(10) == std::optional{100});
        AssertTrue(table.findByKey(50) == std::optional{500});
        AssertTrue(table.findByKey(90) == std::optional{900});
        AssertTrue(table.findByKey(170) == std::optional{1700});

        AssertFalse(table.findByKey(0).has_value());
        AssertFalse(table.findByKey(55).has_value());
        AssertFalse(table.findByKey(180).has_value());
    }

    void testBinarySearchThreshold()
    {
        constexpr std::size_t Threshold = 16;

        using LinearTable = StaticTable<int, int, Threshold>;
        using BinaryTable = StaticTable<int, int, Threshold + 1>;

        constexpr LinearTable linearTable{
            {16, 160},
            {1, 10},
            {8, 80},
            {4, 40},
            {12, 120},
            {2, 20},
            {6, 60},
            {10, 100},
            {14, 140},
            {3, 30},
            {5, 50},
            {7, 70},
            {9, 90},
            {11, 110},
            {13, 130},
            {15, 150}
        };

        constexpr BinaryTable binaryTable{
            {17, 170},
            {1, 10},
            {8, 80},
            {4, 40},
            {12, 120},
            {2, 20},
            {6, 60},
            {10, 100},
            {14, 140},
            {3, 30},
            {5, 50},
            {7, 70},
            {9, 90},
            {11, 110},
            {13, 130},
            {15, 150},
            {16, 160}
        };

        AssertTrue(linearTable.findByKey(16) == std::optional{160});
        AssertTrue(binaryTable.findByKey(17) == std::optional{170});

        AssertTrue(linearTable.containsKey(16));
        AssertTrue(binaryTable.containsKey(17));

        AssertFalse(linearTable.containsKey(100));
        AssertFalse(binaryTable.containsKey(100));
    }

    void testFindByValue()
    {
        constexpr StaticTable<int, int, 5> table{
            {30, 300},
            {10, 100},
            {50, 500},
            {20, 200},
            {40, 400}
        };

        AssertTrue(table.findByValue(100) == std::optional{10});
        AssertTrue(table.findByValue(200) == std::optional{20});
        AssertTrue(table.findByValue(300) == std::optional{30});
        AssertTrue(table.findByValue(400) == std::optional{40});
        AssertTrue(table.findByValue(500) == std::optional{50});

        AssertFalse(table.findByValue(0).has_value());
        AssertFalse(table.findByValue(350).has_value());
        AssertFalse(table.findByValue(600).has_value());
    }

    void testContainsKey()
    {
        constexpr StaticTable<int, int, 5> table{
            {30, 300},
            {10, 100},
            {50, 500},
            {20, 200},
            {40, 400}
        };

        AssertTrue(table.containsKey(10));
        AssertTrue(table.containsKey(20));
        AssertTrue(table.containsKey(30));
        AssertTrue(table.containsKey(40));
        AssertTrue(table.containsKey(50));

        AssertFalse(table.containsKey(0));
        AssertFalse(table.containsKey(15));
        AssertFalse(table.containsKey(60));
    }

    void testContainsValue()
    {
        constexpr StaticTable<int, int, 5> table{
            {30, 300},
            {10, 100},
            {50, 500},
            {20, 200},
            {40, 400}
        };

        AssertTrue(table.containsValue(100));
        AssertTrue(table.containsValue(200));
        AssertTrue(table.containsValue(300));
        AssertTrue(table.containsValue(400));
        AssertTrue(table.containsValue(500));

        AssertFalse(table.containsValue(0));
        AssertFalse(table.containsValue(350));
        AssertFalse(table.containsValue(600));
    }

    void testData()
    {
        constexpr StaticTable<int, int, 4> table{
            {40, 400},
            {10, 100},
            {30, 300},
            {20, 200}
        };

        auto const& data = table.data();

        AssertEqual(10, data[0].first);
        AssertEqual(100, data[0].second);

        AssertEqual(20, data[1].first);
        AssertEqual(200, data[1].second);

        AssertEqual(30, data[2].first);
        AssertEqual(300, data[2].second);

        AssertEqual(40, data[3].first);
        AssertEqual(400, data[3].second);
    }

    void testIteration()
    {
        constexpr StaticTable<int, int, 5> table{
            {30, 300},
            {10, 100},
            {50, 500},
            {20, 200},
            {40, 400}
        };

        constexpr std::array expectedKeys{10, 20, 30, 40, 50};
        std::size_t index = 0;
        for (auto const& [key, value] : table)
        {
            AssertEqual(expectedKeys[index], key);
            AssertEqual(key * 10, value);
            ++index;
        }

        AssertEqual(5UZ, index);
    }

    void testConstIterators()
    {
        constexpr StaticTable<int, int, 3> table {
            {30, 300},
            {10, 100},
            {20, 200}
        };

        auto it = table.cbegin();
        auto const end = table.cend();

        AssertTrue(it != end);
        AssertEqual(10, it->first);

        ++it;

        AssertTrue(it != end);
        AssertEqual(20, it->first);

        ++it;

        AssertTrue(it != end);
        AssertEqual(30, it->first);

        ++it;

        AssertTrue(it == end);
    }

    void testStorageConstructor()
    {
        constexpr StaticTable<int, int, 4>::Storage data {
            std::pair{40, 400},
            std::pair{10, 100},
            std::pair{30, 300},
            std::pair{20, 200}
        };
        constexpr StaticTable table { data };

        AssertEqual(10, table.data()[0].first);
        AssertEqual(20, table.data()[1].first);
        AssertEqual(30, table.data()[2].first);
        AssertEqual(40, table.data()[3].first);

        AssertEqual(100, table.data()[0].second);
        AssertEqual(200, table.data()[1].second);
        AssertEqual(300, table.data()[2].second);
        AssertEqual(400, table.data()[3].second);
    }

    void testArrayRangeConstructor()
    {
        constexpr std::array data {
            std::pair{40, 400},
            std::pair{10, 100},
            std::pair{30, 300},
            std::pair{20, 200}
        };
        constexpr StaticTable table{data};

        AssertEqual(10, table.data()[0].first);
        AssertEqual(20, table.data()[1].first);
        AssertEqual(30, table.data()[2].first);
        AssertEqual(40, table.data()[3].first);
    }

    void testVectorRangeConstructor()
    {
        std::vector<std::pair<int, int>> data{
            {40, 400},
            {10, 100},
            {30, 300},
            {20, 200}
        };

        const StaticTable<int, int, 4> table{data};

        AssertEqual(10, table.data()[0].first);
        AssertEqual(20, table.data()[1].first);
        AssertEqual(30, table.data()[2].first);
        AssertEqual(40, table.data()[3].first);
    }

    void testMakeStaticTable()
    {
        constexpr auto table = makeStaticTable(
            std::pair{30, 300},
            std::pair{10, 100},
            std::pair{50, 500},
            std::pair{20, 200},
            std::pair{40, 400}
        );

        AssertEqual(5UZ, table.size());
        AssertTrue(table.findByKey(10) == std::optional{100});
        AssertTrue(table.findByValue(400) == std::optional{40});
    }

    constexpr void testCompileTimeConstruction()
    {
        constexpr auto table = makeStaticTable(
            std::pair{30, 300},
            std::pair{10, 100},
            std::pair{50, 500},
            std::pair{20, 200},
            std::pair{40, 400}
        );

        static_assert(table.size() == 5);
        static_assert(table.data()[0] == std::pair{10, 100});
        static_assert(table.data()[1] == std::pair{20, 200});
        static_assert(table.data()[2] == std::pair{30, 300});
        static_assert(table.data()[3] == std::pair{40, 400});
        static_assert(table.data()[4] == std::pair{50, 500});
    }

    constexpr void testCompileTimeFindByKey()
    {
        constexpr auto table = makeStaticTable(
            std::pair{30, 300},
            std::pair{10, 100},
            std::pair{50, 500},
            std::pair{20, 200},
            std::pair{40, 400}
        );

        static_assert(table.findByKey(10) == std::optional{100});
        static_assert(table.findByKey(20) == std::optional{200});
        static_assert(table.findByKey(30) == std::optional{300});
        static_assert(table.findByKey(40) == std::optional{400});
        static_assert(table.findByKey(50) == std::optional{500});

        static_assert(!table.findByKey(0).has_value());
        static_assert(!table.findByKey(25).has_value());
        static_assert(!table.findByKey(100).has_value());
    }

     constexpr void testCompileTimeFindByKeyBinary()
    {
        constexpr auto table = makeStaticTable(
            std::pair{170, 1700},
            std::pair{30, 300},
            std::pair{100, 1000},
            std::pair{50, 500},
            std::pair{160, 1600},
            std::pair{10, 100},
            std::pair{140, 1400},
            std::pair{70, 700},
            std::pair{120, 1200},
            std::pair{20, 200},
            std::pair{150, 1500},
            std::pair{80, 800},
            std::pair{40, 400},
            std::pair{130, 1300},
            std::pair{60, 600},
            std::pair{110, 1100},
            std::pair{90, 900}
        );

        static_assert(table.findByKey(10) == std::optional{100});
        static_assert(table.findByKey(90) == std::optional{900});
        static_assert(table.findByKey(170) == std::optional{1700});

        static_assert(!table.findByKey(0).has_value());
        static_assert(!table.findByKey(55).has_value());
        static_assert(!table.findByKey(180).has_value());
    }


    constexpr void testCompileTimeFindByValue()
    {
        constexpr auto table = makeStaticTable(
            std::pair{30, 300},
            std::pair{10, 100},
            std::pair{50, 500},
            std::pair{20, 200},
            std::pair{40, 400}
        );

        static_assert(table.findByValue(100) == std::optional{10});
        static_assert(table.findByValue(200) == std::optional{20});
        static_assert(table.findByValue(300) == std::optional{30});
        static_assert(table.findByValue(400) == std::optional{40});
        static_assert(table.findByValue(500) == std::optional{50});

        static_assert(!table.findByValue(0).has_value());
        static_assert(!table.findByValue(350).has_value());
        static_assert(!table.findByValue(600).has_value());
    }


    constexpr void testCompileTimeContains()
    {
        constexpr auto table = makeStaticTable(
            std::pair{30, 300},
            std::pair{10, 100},
            std::pair{50, 500},
            std::pair{20, 200},
            std::pair{40, 400}
        );

        static_assert(table.containsKey(10));
        static_assert(table.containsKey(20));
        static_assert(table.containsKey(30));
        static_assert(table.containsKey(40));
        static_assert(table.containsKey(50));

        static_assert(table.containsValue(100));
        static_assert(table.containsValue(200));
        static_assert(table.containsValue(300));
        static_assert(table.containsValue(400));
        static_assert(table.containsValue(500));

        static_assert(!table.containsKey(0));
        static_assert(!table.containsKey(999));
        static_assert(!table.containsValue(0));
        static_assert(!table.containsValue(999));
    }

    void runAll()
    {
        testConstruction();
        testFindByKeyLinear();
        testFindByKeyBinary();
        testBinarySearchThreshold();
        testFindByValue();
        testContainsKey();
        testContainsValue();
        testData();
        testIteration();
        testConstIterators();
        testStorageConstructor();
        testArrayRangeConstructor();
        testVectorRangeConstructor();
        testMakeStaticTable();

        testCompileTimeConstruction();
        testCompileTimeFindByKey();
        testCompileTimeFindByKeyBinary();
        testCompileTimeFindByValue();
        testCompileTimeContains();
    }
}


void static_table::testAll()
{
    static_table::testing::runAll();

}


