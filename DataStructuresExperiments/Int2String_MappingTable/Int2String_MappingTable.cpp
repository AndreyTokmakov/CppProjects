/**============================================================================
Name        : Int2Strins_MappingTable.cpp
Created on  : 21.03.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "Int2String_MappingTable.h"
#include "../Utils/PertTools.h"

#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>


namespace
{
    template <typename K, typename V>
    struct Node
    {
        K key;
        V value;

        // TODO: Static assert key != void
        // TODO: Static assert value != void
        Node(K key, V val): key {key}, value {std::move(val)} {
        }
    };

    using NodeType = Node<int, std::string>;
    using Bundle = std::vector<NodeType>;

    struct Table
    {
        using key_type = int;
        using value_type = std::string;
        using size_type  = size_t;

        static constexpr size_type INITIAL_CAPACITY { 32 };
        // static constexpr size_type GROWTH_FACTOR { 4 };
        // static constexpr size_type MAX_LOAD_FACTOR { 3 };

        std::vector<Bundle> table { INITIAL_CAPACITY };

        Table() = default;

        explicit Table(const size_type capacity): table {capacity} {
        }

        NodeType& put(const key_type key, value_type value)
        {
            const size_t index { key % table.size() };
            Bundle& bundle = table[index];

            for (auto& node : bundle) {
                if (key == node.key)
                    return node;
            }
            return bundle.emplace_back(key, std::move(value));
        }

        [[nodiscard]]
        std::optional<value_type> get(const key_type key) const
        {
            const size_t index { key % table.size() };
            for (const Bundle& bundle = table[index]; const auto& [k, val] : bundle) {
                if (key == k)
                    return val;
            }
            return std::nullopt;
        }

        [[nodiscard]]
        bool contains(const key_type key) const noexcept
        {
            const size_t index { key % table.size() };
            for (const Bundle& bundle = table[index]; const auto& [k, val] : bundle) {
                if (key == k)
                    return true;
            }
            return false;
        }
    };
}

namespace Tests
{
    void print(const Table& table)
    {
        for (size_t idx = 0; idx < table.table.size(); ++idx)
        {
            std::cout << '[' << idx << "]: ";
            for (const auto& [k, v] : table.table[idx]) {
                std::cout << "(" << k << ',' << v << ")";
            }
            std::cout << '\n';
        }
    }

    void test1()
    {
        Table table;
        table.put(1, "1");
        table.put(2, "2");
        table.put(128, "128");
        table.put(55, "55");
        table.put(4545, "4545");

        //std::cout << table.get(2).value_or("None") << std::endl;
        //std::cout << table.get(32).value_or("None") << std::endl;

        print(table);
    }

    void test2()
    {
        Table table;

        for (int idx = 0; idx < 128; ++idx)
            table.put(idx, std::to_string(idx));


        print(table);
    }

    void benchmark()
    {

        constexpr int elementsCount = 1024 * 32;
        constexpr int iterations = 10'000;

        Table table(elementsCount);
        std::unordered_map<int, std::string> mapping;
        for (int idx = 0; idx < elementsCount; ++idx)
        {
            table.put(idx, std::to_string(idx));
            mapping.emplace(idx, std::to_string(idx));
        }


        {
            PertTools::ScopedTimer timer { "table" };
            for (int idx = 0; idx < iterations; ++idx) {
                for (int idx = 0; idx < elementsCount; ++idx) {
                    if (!table.contains(idx))
                    {
                        std::cerr << idx << '\n';
                    }
                }
            }
        }

        {
            PertTools::ScopedTimer timer { "std::unordered_map" };
            for (int idx = 0; idx < iterations; ++idx) {
                for (int idx = 0; idx < elementsCount; ++idx) {
                   if (!mapping.contains(idx))
                   {
                       std::cerr << idx << '\n';
                   }
                }
            }
        }


        /**
        Table table(1024 * 10);
           table              :  0.0875861 seconds.
           std::unordered_map :  0.22783 seconds.
        **/
    }
}

void Int2String_MappingTable::TestAll()
{
    using namespace Tests;

    // test1();
    // test2();
    benchmark();
}
