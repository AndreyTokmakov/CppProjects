/**============================================================================
Name        : FlatMap.cpp
Created on  : 19.11.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : FlatMap.cpp
============================================================================**/

#include "FlatMap.hpp"

#include <algorithm>
#include <utility>
#include <memory>
#include <cstddef>
#include <cstdint>

namespace impl
{

    template<typename K, typename V, size_t MAX_SIZE = 1024>
    struct FlatMap
    {
        using key_type = K;
        using value_type = V;
        using size_type = size_t;

        struct Node
        {
            key_type key;
            value_type value;
        };

        FlatMap() : length(0) {}

        [[nodiscard]]
        bool contains(const key_type& key) const noexcept {
            return find_idx(key) != -1;
        }

        value_type& operator[](const key_type& key)
        {
            const int idx = find_idx(key);
            if (idx != -1)
                return data[idx].value;

            const size_type pos = lower_bound(key);
            for (size_type i = length; i > pos; --i)
                data[i] = data[i - 1];

            data[pos].key = key;
            data[pos].value = value_type{};
            ++length;
            return data[pos].value;
        }

        [[nodiscard]]
        Node& at(const size_type idx) {
            return data[idx];
        }

        [[nodiscard]]
        const Node& at(const size_type idx) const {
            return data[idx];
        }

        void erase(const key_type& key) noexcept
        {
            const int idx = find_idx(key);
            if (idx == -1)
                return;

            for (size_t i = idx; i + 1 < length; ++i)
                data[i] = data[i + 1];

            --length;
        }

        [[nodiscard]]
        size_t size() const noexcept
        {
            return length;
        }

        [[nodiscard]]
        const Node& front() const noexcept {
            return data[0];
        }

        [[nodiscard]]
        const Node& back()  const noexcept {
            return data[length - 1];
        }

        [[nodiscard]]
        const Node* best_bid() const noexcept {
            return length == 0 ? nullptr : &data[length - 1];
        }

        [[nodiscard]]
        const Node* best_ask() const noexcept {
            return length == 0 ? nullptr : &data[0];
        }

    private:

        std::array<Node, MAX_SIZE> data {};
        size_type length;

        [[nodiscard]]
        int find_idx(const key_type& key) const noexcept
        {
            size_type l = 0, r = length;
            while (l < r) {
                size_type m = (l + r) / 2;
                if (data[m].key == key)
                    return static_cast<int>(m);
                if (data[m].key < key)
                    l = m + 1;
                else
                    r = m;
            }
            return -1;
        }

        [[nodiscard]]
        size_type lower_bound(const key_type& key) const noexcept
        {
            size_type l = 0, r = length;
            while (l < r) {
                size_type m = (l + r) / 2;
                if (data[m].key < key)
                    l = m + 1;
                else
                    r = m;
            }
            return l;
        }
    };
}

namespace impl::tests
{
    template<typename K, typename V>
    void print(const FlatMap<K, V>& map)
    {
        for (typename FlatMap<K, V>::size_type idx = 0; idx < map.size(); ++idx)
        {
            const typename FlatMap<K, V>::Node& node = map.at(idx);
            std::cout << node.key << " -> " << node.value << std::endl;
        }
    }


    void testFlatMap()
    {
        FlatMap<int, int> map;
        map[1] = 1;
        map[2] = 2;
        map[3] = 3;
        map[4] = 4;
        map[5] = 5;

        print(map);
    }
}

void FlatMap::TestAll()
{
    impl::tests::testFlatMap();

}
