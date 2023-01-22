/**============================================================================
Name        : HashTable.cpp
Created on  : 03.09.2022
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : HashTable
============================================================================**/

#include "HashTable.h"

#include <iostream>
#include <vector>
#include <list>
#include <forward_list>
#include <optional>

namespace HashTable::Simple
{

    template <typename K, typename V>
    class HashNode {
    public:
        V value;
        K key;

        // TODO: Static assert key != void
        // TODO: Static assert value != void

        HashNode(K key, V val): key {key}, value {val} {
        }
    };

    template <typename K, typename V>
    class HashMap {
        HashNode<K, V>** arr = new HashNode<K, V>*[capacity];
        HashNode<K, V>* dummy;

        const int capacity;
        int size = 0;

    public:
        explicit HashMap(int capacity = 20): capacity {capacity}
        {
            // Initialise all elements of array as NULL
            for (int i = 0; i < capacity; i++)
                arr[i] = nullptr;

            // dummy node with value and key -1
            dummy = new HashNode<K, V>(-1, -1);
        }

        // This implements hash function to find index for a key
        int calculateHash(const K& key) const noexcept {
            return key % capacity;
        }

        // Function to add key value pair
        void insertNode(K key, V value)
        {
            // Apply hash function to find index for given key
            int hashIndex = calculateHash(key);

            // find next free space
            while (arr[hashIndex] != nullptr && arr[hashIndex]->key != key && arr[hashIndex]->key != -1) {
                hashIndex++;
                hashIndex %= capacity;
            }

            // if new node to be inserted increase the current size
            if (arr[hashIndex] == nullptr|| arr[hashIndex]->key == -1)
                size++;

            arr[hashIndex] = new HashNode<K, V>(key, value);
        }

        // Function to delete a key value pair
        V deleteNode(int key)
        {
            // Apply hash function
            // to find index for given key
            int hashIndex = calculateHash(key);

            // finding the node with given key
            while (arr[hashIndex] != nullptr) {
                // if node found
                if (arr[hashIndex]->key == key) {
                    HashNode<K, V>* temp = arr[hashIndex];

                    // Insert dummy node here for further use
                    arr[hashIndex] = dummy;

                    // Reduce size
                    size--;
                    return temp->value;
                }
                hashIndex++;
                hashIndex %= capacity;
            }

            // If not found return null
            // return nullptr;
            throw std::runtime_error("Not found");
        }

        // Function to search the value for a given key
        V get(int key)
        {
            // Apply hash function to find index for given key
            int hashIndex = calculateHash(key);
            int counter = 0;

            // finding the node with given key
            while (arr[hashIndex] != nullptr) { // int counter =0; // BUG!

                if (counter++ > capacity) { // to avoid infinite loop
                    throw std::runtime_error("capacity error");
                    // return nullptr;
                }

                // if node found return its value
                if (arr[hashIndex]->key == key)
                    return arr[hashIndex]->value;
                hashIndex++;
                hashIndex %= capacity;
            }

            // return nullptr;
            throw std::runtime_error("Not found");
        }

        // Return current size
        inline int sizeofMap() const noexcept {
            return size;
        }

        inline int isEmpty() const noexcept {
            return size == 0;
        }

        void display()
        {
            for (int i = 0; i < capacity; i++) {
                if (arr[i] != nullptr && arr[i]->key != -1)
                    std::cout << "key = " << arr[i]->key
                         << "  value = "
                         << arr[i]->value << std::endl;
            }
        }
    };

    void Test() {
        HashMap<int, int>* h = new HashMap<int, int>;
        h->insertNode(1, 1);
        h->insertNode(2, 2);
        h->insertNode(2, 3);
        h->display();
        std::cout << h->sizeofMap() << std::endl;
        std::cout << h->deleteNode(2) << std::endl;
        std::cout << h->sizeofMap() << std::endl;
        std::cout << h->isEmpty() << std::endl;
        std::cout << h->get(2);

    }
};

namespace HashTable::Impl2 {

    // TODO: Should 'values' be an empty pointer if no value is set?

    template<typename K, typename V>
    class Entry final {
    public:
        using Pair = std::pair<K, V>;
        // using bundle_type = std::forward_list<Pair>;
        using bundle_type = std::list<Pair>;

        bundle_type values {};

        // TODO: Static assert key != void
        // TODO: Static assert value != void
        // TODO: Static assert V is default_constructable

        // FIXME: In case if no Values assigned to some Index position
        //        should we create a list for bundle????

        Entry() = default;
    };


    template<typename K, typename V>
    class HashTable
    {
        using key_type = K;
        using value_type = V;

        // TODO: Static assert K is hashable
        // TODO: Static assert (k == k)
        // TODO: Calculate when we need to grow the table

        static inline constexpr size_t INITIAL_CAPACITY { 2 };
        static inline constexpr size_t GROWTH_FACTOR { 4 };

        // TODO: Use Templates to specify HASHING STRATEGY
        static inline constexpr std::hash<key_type> hasher {};
        static inline constexpr size_t MAX_LOAD_FACTOR { 3 };

        std::vector<Entry<key_type, value_type>> table { INITIAL_CAPACITY };
        size_t maxBundleSize { 0 };

    public:

        [[nodiscard]]
        constexpr size_t calculateHash(const key_type &key) const noexcept {
            return hasher(key);
        }

        [[nodiscard]]
        constexpr size_t getBundleSize() const noexcept {
            return maxBundleSize;
        }

        [[nodiscard]]
        constexpr size_t size() const noexcept {
            return table.size();
        }

        [[nodiscard]]
        constexpr size_t capacity() const noexcept {
            return table.capacity();
        }

        void put(key_type key, value_type value)
        {
            const size_t index { calculateHash(key) % table.size() };
            auto& bundle = table.at(index).values;
            bundle.push_back({key, value});
            maxBundleSize = std::max(maxBundleSize, bundle.size());

            // TODO: Check LoadFactor? Rebuild table?
            // TODO: Calculate when we need to grow the table

            if (maxBundleSize >= MAX_LOAD_FACTOR)
            {
                enlarge();
            }
        }

        [[nodiscard]]
        std::optional<value_type> find(const key_type& key) const
        {
            const size_t index { calculateHash(key) % table.size() };
            for (const auto& [k, v]: table[index].values) {
                if (key == k)
                    return v;
            }
            return std::nullopt;
        }

        // TODO: Check for performance
        void enlarge()
        {
            std::cout << "Resizing....\n";

            size_t maxBundleSizeLocal = 0;
            std::vector<Entry<key_type, value_type>> tableLocal { table.size() * GROWTH_FACTOR};
            for (const size_t size = tableLocal.size(); const auto& entry: table) {
                for (const auto& [k, v]: entry.values) {
                    // TODO: Remove redundant code
                    const size_t index { calculateHash(k) % size };
                    auto& bundle = tableLocal.at(index).values;
                    bundle.push_back({k, v});
                    maxBundleSizeLocal = std::max(maxBundleSizeLocal, bundle.size());
                }
            }

            maxBundleSize = maxBundleSizeLocal;
            table.swap(tableLocal);
        }


        void display() {
            for (size_t idx = 0; idx < table.size(); ++idx) {
                std::cout << "table[" << idx << "]: { ";
                const auto& bundle = table[idx].values;
                for (const auto& [k, v]: bundle) {
                    std::cout << "(" << k << ", " << v << ") ";
                }
                std::cout << "}" << std::endl;
            }
        }
    };


    void Test()
    {
        HashTable<std::string, int> tbl;
        tbl.put("One", 1);
        tbl.put("Two", 2);
        tbl.put("Three", 3);

        tbl.display();

        const auto& result = tbl.find("Two");
        std::cout << result.value_or(0) << std::endl;

        std::cout << "----------------------------" << tbl.getBundleSize() << "------------------------------" << std::endl;

        tbl.enlarge();
        tbl.display();

        std::cout << tbl.find("Two").value_or(0) << std::endl;
        std::cout << "----------------------------" << tbl.getBundleSize() << "------------------------------" << std::endl;

    }

    void Push_And_Growth_Test()
    {
        HashTable<int, int> table;
        for (int i = 200; i < 300; ++i) {
            table.put(i, i);
        }

        // table.display();
        std::cout << "Size: " << table.size() << std::endl;
        std::cout << "Capacity: " << table.capacity() << std::endl;
    }
}

void HashTable::TestAll()
{
    // Simple::Test();

    // Impl2::Test();
    Impl2::Push_And_Growth_Test();
};