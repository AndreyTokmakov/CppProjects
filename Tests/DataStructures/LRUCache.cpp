//============================================================================
// Name        : LRUCache.h
// Created on  : 11.06.2022.
// Author      : Tokmakov Andrei
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : LRUCache
//============================================================================

#include "LRUCache.h"

#include <iostream>
#include <string>

#include <array>
#include <list>
#include <deque>
#include <unordered_map>

namespace LRUCache::Original
{

    template<typename K = int, typename V = int>
    class LRUCache {
    private:
        using KeyType = K;
        using ValueType = V;
        using Item = std::pair<KeyType, ValueType>;
        using ListIter = typename std::list<Item>::iterator;

    private:
        std::list<Item> items;
        std::unordered_map<KeyType, ListIter> cache;
        const size_t capacity {0};

    public:
        explicit LRUCache(size_t capacity): capacity {capacity} {
        }

        bool put(const KeyType& key, const V& value) noexcept {
            if (auto iter = cache.find(key); cache.end() != iter) {
                iter->second->second = value;
                return false;
            }

            // Delete least recently used item:
            if (items.size() == capacity) {
                cache.erase(items.back().first); // Erase the last item key from the map
                items.pop_back();                // Evict last item from the list
            }

            /* Insert the new item at front of the list: */
            items.emplace_front(key, value);
            /* Insert {key->item_iterator} in the map: */
            cache.emplace(key, items.begin());

            return true;
        }

        ValueType get(const KeyType& k) noexcept {
            const auto iter = cache.find(k);
            if (cache.end() == iter) {
                return -1; //empty std::optional
            }

            /* Use list splice to transfer this item to the first position,
               which makes the item most-recently-used. Iterators still stay valid. */
            // Just move the found elemnt to first position on the LIST
            items.splice(items.begin(), items, iter->second);
            return iter->second->second;
        }
    };


    void Test() {
        LRUCache cache {2};

        cache.put(1, 1);
        cache.put(2, 2);

        std::cout << cache.get(1) << std::endl;

        cache.put(3, 3);

        std::cout << cache.get(2) << std::endl;

        cache.put(4, 4);

        std::cout << cache.get(1) << std::endl;
        std::cout << cache.get(3) << std::endl;
        std::cout << cache.get(4) << std::endl;
    }
};


namespace LRUCache::Simple {

    class LRUCache {
    public:
        using KeyType = int;
        using ValueType = int;
        using Item = std::pair<KeyType, ValueType>;
        using ListIter = typename std::list<Item>::iterator;

        std::list<Item> items {};
        std::unordered_map<KeyType, ListIter> cache {};
        size_t size {0};

    public:
        explicit LRUCache(size_t capacity): size {capacity} {
            // Write your code here
        }

        int get(int key) {
            const auto iter = cache.find(key);
            if (cache.end() == iter)
                return -1;

            items.splice(items.begin(), items, iter->second);
            return iter->second->second;
        }

        // INFO: We may want to update element if it exists?
        void put(int key, int value) {
            if (const auto iter = cache.find(key); cache.end() == iter) {
                if (items.size() == size) {
                    /* Erase the last item key from the map */
                    cache.erase(items.back().first);
                    /* Evict last item from the list */
                    items.pop_back();
                }

                items.emplace_front(key, value);
                cache.emplace(key, items.begin());
            } else {
                // TODO: Update existing value?
                // iter->second->second = value;
            }
        }
    };

    void Test() {
        LRUCache cache {2};

        cache.put(1, 1);
        cache.put(2, 2);

        std::cout << cache.get(1) << std::endl;

        cache.put(3, 3);

        std::cout << cache.get(2) << std::endl;

        cache.put(4, 4);
        cache.put(4, 41);

        std::cout << cache.get(1) << std::endl;
        std::cout << cache.get(3) << std::endl;
        std::cout << cache.get(4) << std::endl;
    }
}

void LRUCache::TestAll()
{
    Original::Test();

    // Simple::Test();
};
