//============================================================================
// Name        : LRUCache.h
// Created on  : 20.01.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : LRUCache src
//============================================================================

#include "LRUCache.h"

#include <iostream>
#include <string>

#include <array>
#include <list>
#include <deque>
#include <unordered_map>
#include <memory>

#include <cassert>
#include <optional>

namespace LRUCache::CircleIteratorArray {

	//template<typename K, typename V, size_t _Size>
	template<typename K = std::string, 
		     typename V = std::string, size_t _Size = 5>
	class LRUCache {
	private:
		using KeyType = K;
		using ValueType = V;
		using Iter = typename std::unordered_map<KeyType, ValueType>::const_iterator;

		static_assert(!std::is_same_v<KeyType, void>, "ERROR: Key type can not be void");
		static_assert(!std::is_same_v<ValueType, void>, "ERROR: Value type can not be void");
		static_assert(0 != _Size,  "ERROR: Please try a little bigger buffer");

	private:
		std::unordered_map<KeyType, ValueType> cache;
		std::array<Iter, _Size> iters;

		bool full{ false };
		size_t head{ 0 };

	public:
		LRUCache() {
			this->cache.reserve(_Size);
		}

		~LRUCache() {
			// For testing
			count = 0;
		}

		constexpr inline size_t size() const noexcept {
			return _Size;
		}

		[[nodiscard]]
		ValueType get(KeyType key) {
			if (Iter result = cache.find(key); cache.end() != result)
				return result->second;

			if (head >= _Size) {
				full = true;
			}

			head = head % _Size;

			if (full) {
				auto node = cache.extract(iters[head]);
				node.key() = key;
				node.mapped() = getFromRealSource();

				auto [a, b, c] = cache.insert(std::move(node));
				iters[head] = a;
			}
			else {
				std::tie(iters[head], std::ignore) = cache.insert({ key , getFromRealSource() });
			}
			return iters[head++]->second;
		}

	public:
		// Test helper functions:
		ValueType getFromRealSource() {
			count++;
			//std::cout << "Creating new value" << std::endl;
			return "Value_" + std::to_string(count);
		}

		inline size_t getReadsCount() const noexcept {
			return count;
		}

		void printCache() {
			for (Iter iter : iters) {
				if (cache.end() != iter) {
					std::cout << iter->first << "  =  " << iter->second << "\n";
				}
			}
		}

		inline static size_t count{ 0 };
	};
};

namespace LRUCache::CircleIteratorArray::Tests {

	void Test1() {
		LRUCache cache;

		auto val1 = cache.get("one");
		assert(0 == val1.compare("Value_1"));
		assert(1 == cache.getReadsCount());
	}

	void Test2() {
		LRUCache<int, std::string, 5> cache;

		auto val = cache.get(1);
		assert(0 == val.compare("Value_1"));

		val = cache.get(1);
		assert(0 == val.compare("Value_1"));

		val = cache.get(1);
		assert(0 == val.compare("Value_1"));
		assert(1 == cache.getReadsCount());
	}

	void Test3() {
		LRUCache<int, std::string, 5> cache;

		for (int i = 1; i <= 5; ++i) {
			auto val = cache.get(i);
			std::string expected{ std::string("Value_") + std::to_string(i) };

			assert(0 == val.compare(expected));
			assert(i == cache.getReadsCount());
		}
	}

	void Test4() {
		LRUCache<int, std::string, 5> cache;

		for (int i = 1; i <= 5; ++i) {
			auto val = cache.get(i);
			std::string expected{ std::string("Value_") + std::to_string(i) };

			assert(0 == val.compare(expected));
			assert(i == cache.getReadsCount());
		}

		// Test that there no cache misseses and [5 == cache.getReadsCount()]
		for (int i = 1; i <= 5; ++i) {
			auto val = cache.get(i);
			std::string expected{ std::string("Value_") + std::to_string(i) };

			assert(0 == val.compare(expected));
			assert(5 == cache.getReadsCount());
		}
	}

	void Test5() {
		LRUCache<int, std::string, 5> cache;

		for (int i = 1; i <= 5; ++i) {
			auto val = cache.get(i);
			val = cache.get(i);
		}
		assert(5 == cache.getReadsCount());

		auto val = cache.get(100500);
		std::string expected{ std::string("Value_") + std::to_string(6) };

		assert(0 == val.compare(expected));
		assert(6 == cache.getReadsCount());
	}

	void Test6() {
		constexpr size_t cache_size = 5;
		LRUCache<int, std::string, cache_size> cache;

		for (int i = 1; i <= cache_size * 3; ++i)
			auto val = cache.get(i);

		assert(cache_size * 3 == cache.getReadsCount());

		for (int i = (cache_size * 2 + 1); i <= cache_size * 3; ++i) {
			auto val = cache.get(i);
			std::string expected{ std::string("Value_") + std::to_string(i) };
			assert(0 == val.compare(expected));
		}

		assert(cache_size * 3 == cache.getReadsCount());
	}

	void PrintCache_Test() {
		constexpr size_t cache_size = 5;
		LRUCache<int, std::string, cache_size> cache;

		for (int i = 1; i <= cache_size * 3; ++i)
			auto val = cache.get(i);

		cache.printCache();
	}

	void RunAllTests() {
		/*
		CircleIteratorArray::Tests::Test1();
		CircleIteratorArray::Tests::Test2();
		CircleIteratorArray::Tests::Test3();
		CircleIteratorArray::Tests::Test4();
		CircleIteratorArray::Tests::Test5();
		CircleIteratorArray::Tests::Test6();
		*/

		CircleIteratorArray::Tests::PrintCache_Test();

		
	}
}

namespace LRUCache::Cache_LinkedList {
	
	template<typename K, typename V, size_t _Size>
	class LRUCache {
	private:
		using KeyType = K;
		using ValueType = V;
		using Item = std::pair<KeyType, ValueType>;
		using ListIter = typename std::list<Item>::iterator;

		static_assert(!std::is_same_v<KeyType, void>, "ERROR: Key type can not be void");
		static_assert(!std::is_same_v<ValueType, void>, "ERROR: Value type can not be void");
		static_assert(0 != _Size, "ERROR: Please try a little bigger buffer");

	private:
		std::list<Item> items;
		std::unordered_map<KeyType, ListIter> cache;

	public:
		/* Adds a key=>value item
		   Returns false if key already exists: */
		bool put(const KeyType& key, const V& value) noexcept {
            if (auto iter = cache.find(key); cache.end() != iter) {
                // iter->second->second = value;
                return false;
            }

            // Delete least recently used item:
			if (items.size() == _Size) {
				cache.erase(items.back().first); // Erase the last item key from the map-cache using iterator
				items.pop_back();                // Erase last item from the list
			}

            /* Insert the new item at front of the list: */
            items.emplace_front(key, value);
            /* Insert {key->item_iterator} in the map: */
            cache.emplace(key, items.begin());

			return true;
		}

		/* Gets the value for a key.
		   Returns empty std::optional if not found. The returned item becomes most-recently-used: */
		std::optional<V> get(const KeyType& k) noexcept {
			const auto iter = cache.find(k);
			if (cache.end() == iter) {
				return {}; //empty std::optional
			}

			/* Use list splice to transfer this item to the first position, 
			   which makes the item most-recently-used. Iterators still stay valid. */
			// Just move the found element to first position on the LIST
			items.splice(items.begin(), items, iter->second);
			return iter->second->second;
		}

		// Erases an item
		void erase(const KeyType& k) noexcept {
			auto itr = cache.find(k);
			if (itr == cache.end()) {
				return;
			}

			// Erase from the list
			items.erase(itr->second);

			// Erase from the  map
			cache.erase(itr);
		}

		/* TEST */
		template<typename C>
		void forEach(const C& callback) const noexcept
		{
			for (auto& [k, v] : items) {
				callback(k, v);
			}
		}
	};

	//=================================================================================//


	void RunTests() {
		auto printlnCache = []<typename C>(const C & cache) {
			cache.forEach([](auto& k, auto& v) {
				std::cout << k << "=>" << v << " ";
			});
			std::cout << std::endl;
		};

		LRUCache<std::string, double, 3> cache;

		//Add 3 entries 
		cache.put("London", 8.4);
		cache.put("Toronto", 2.5);
		cache.put("Sydney", 5.2);

		// Prints Below: Sydney=>5.2 Toronto=>2.5 London=>8.4 
		printlnCache(cache);

		// Make "London=>8.4" the most recently accessed
		std::cout << "London=>"
				  << cache.get("London").value_or(-1)
				  << std::endl; //London=>8.4

		// Prints Below: London=>8.4 Sydney=>5.2 Toronto=>2.5
		printlnCache(cache);

		// This would remove the LRU item (Toronto=>2.5)
		cache.put("Tokyo", 9.4);

		// Prints Below: Tokyo=>9.4 London=>8.4 Sydney=>5.2 
		printlnCache(cache);
	}
}

namespace LRUCache::CircleIterator_Deque {

	//template<typename K, typename V, size_t _Size>
	template<typename K = std::string,
		typename V = std::string, size_t _Size = 5>
		class LRUCache {
		private:
			using KeyType = K;
			using ValueType = V;
			using Iter = typename std::unordered_map<KeyType, ValueType>::const_iterator;

			static_assert(!std::is_same_v<KeyType, void>, "ERROR: Key type can not be void");
			static_assert(!std::is_same_v<ValueType, void>, "ERROR: Value type can not be void");
			static_assert(0 != _Size, "ERROR: Please try a little bigger buffer");

		private:
			std::unordered_map<KeyType, ValueType> cache;
			std::deque<Iter> iters;

			bool full{ false };
			size_t head{ 0 };

		public:
			LRUCache() {
				this->cache.reserve(_Size);
				this->iters.reserve(_Size);
			}

			~LRUCache() {
				// For testing
				count = 0;
			}

			constexpr inline size_t size() const noexcept {
				return _Size;
			}

			[[nodiscard]]
			ValueType get(KeyType key) {
				if (Iter result = cache.find(key); cache.end() != result)
					return result->second;

				if (head >= _Size) {
					full = true;
				}

				head = head % _Size;

				if (full) {
					auto node = cache.extract(iters[head]);
					node.key() = key;
					node.mapped() = getFromRealSource();

					auto [a, b, c] = cache.insert(std::move(node));
					iters[head] = a;
				}
				else {
					std::tie(iters[head], std::ignore) = cache.insert({ key , getFromRealSource() });
				}
				return iters[head++]->second;
			}

		public:
			// Test helper functions:
			ValueType getFromRealSource() {
				count++;
				//std::cout << "Creating new value" << std::endl;
				return "Value_" + std::to_string(count);
			}

			inline size_t getReadsCount() const noexcept {
				return count;
			}

			void printCache() {
				for (Iter iter : iters) {
					if (cache.end() != iter) {
						std::cout << iter->first << "  =  " << iter->second << "\n";
					}
				}
			}

			inline static size_t count{ 0 };
	};
};


void LRUCache::TEST_ALL() {
	// CircleIteratorArray::Tests::RunAllTests();
	Cache_LinkedList::RunTests();


	// Cache_LinkedList::RunTests();  // UNIMPLEMENTED
};
