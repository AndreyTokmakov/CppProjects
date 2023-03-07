//============================================================================
// Name        : TTLCache.cpp
// Created on  : 14.03.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : TT LCache src class
//============================================================================

#include "TTLCache.h"

#include <iostream>
#include <string>
#include <any>
#include <chrono>
#include <thread>
#include <optional>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>

#include "../Utilities/Integer.h"

namespace TTLCache::Cache_ANY {


	class TTLCache final {
	private:
		// Values stored along with timestamp
		struct Item {
			std::any value;
			// std::string value;

			explicit Item(std::any&& value) : value(std::move(value)) {
			}

			const time_t timestamp{
				std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
			};
		};

		// TODO: Implement 
		//       1. Add item removal
		// Fires periodically in a separate thread and erases the items from cache that are older than the ttlSeconds
		void onTimer() {
			while (true) {
				const time_t now {
					std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
				};

				for (const auto& [key, value] : cache) {
					std::cout << now - value.timestamp << std::endl;
					// REMOVE ITEM
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(5000));
			}
		}

		std::thread wathdog {};

	private:
		// Expire time (ttl) of items in seconds:
		const uint32_t ttlSeconds;

		// Items are stored against keys along with timestamp:
		std::unordered_map<std::string, Item> cache;

		// Default item TLL:
		constexpr static inline uint32_t DEFAULT_TTL = 32;

		mutable std::shared_mutex entry_mutex;

	public:
		// Initializes with a given ttl (in seconds)
		TTLCache(uint32_t ttl = DEFAULT_TTL): ttlSeconds{ttl} {
			wathdog = std::thread(&TTLCache::onTimer, this);
		}

		~TTLCache() {
			wathdog.join();
		}

		// Adds an item to the cache
		bool put(std::string&& key, std::any&& value) noexcept {
			std::lock_guard<std::shared_mutex> lock(entry_mutex);
            // TODO: Fix error
			// return cache.try_emplace(std::move(key), Item { std::move(value) }).second;
            return true;
		}

		// Gets a value from cache if exists otherwise returns empty std::any
		std::optional<std::any> get(const std::string& key) const noexcept {
			std::shared_lock<std::shared_mutex> lock_shared(entry_mutex);
			auto iter = cache.find(key);
			return cache.end() != iter ? 
				std::make_optional<std::any>(iter->second.value) : std::nullopt;
		}

		// Gets a value from cache if exists otherwise returns empty std::any
		std::optional<std::any> get(std::string&& key) const noexcept {
			std::shared_lock<std::shared_mutex> lock_shared(entry_mutex);
			auto iter = cache.find(key);
			return cache.end() != iter ? 
				std::make_optional<std::any>(iter->second.value) : std::nullopt;
		}

		// TODO: Implement 
		// Erases an item for a given key if exists
		// void erase(const std::string& key);
	};

	void Test() {

		TTLCache cache{ 10 };

		bool result1 = cache.put("Key1", std::make_any<std::string>("Value1"));
		std::cout << std::boolalpha << result1 << std::endl;

		bool result2 = cache.put("Key1", std::make_any<std::string>("Value2"));
		std::cout << std::boolalpha << result2 << std::endl;

		std::optional<std::any> result = cache.get("Key1");
		if (result.has_value()) {
			std::cout << std::any_cast<std::string>(result.value()) << std::endl;
		}
	}
}



namespace TTLCache::Cache {

	template<typename T>
	class TTLCache;

	template<typename Type>
	class Item {
	private:
		using data_type = Type;
		using data_reference = data_type&;

		static_assert(!std::is_same_v<data_type, void>,
					  "Type of the data can not be void");

		friend class TTLCache<data_type>;

		/* Data carrier: */
		data_type storage;

		/* Item creation timestamp: */
		const time_t timestamp {
			std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
		};

	public:
		template<typename ... Args>
		Item(Args&& ... params) : storage{ std::forward<Args>(params)... } {
		}

		inline data_reference get() noexcept {
			return storage;
		}

		Item(const Item&) = delete;
		Item(Item&&) noexcept = delete;

		Item& operator=(const Item&) = delete;
		Item& operator=(Item&&) noexcept = delete;
	};


	// TODO:
	// 1.  Extend ITEM lifetime when it accesses?

	template<typename Type>
	class TTLCache final {
	private:
		using data_type = Type;
		using data_reference = std::reference_wrapper<data_type>;
		using item_type = Item<data_type>;

		static_assert(!std::is_same_v<data_type, void>,
					  "Type of the data can not be void");

	private:
		// Expire time (ttl) of items in seconds:
		const uint32_t ttlSeconds;

		// Items are stored against keys along with timestamp:
		std::unordered_map<std::string, item_type> cache;

		// Default item TLL:
		constexpr static inline uint32_t DEFAULT_TTL { 10 };

		// Default item TLL:
		constexpr static inline std::chrono::milliseconds SLEEP_TIMEOUT {
			std::chrono::milliseconds(1000)
		};

		mutable std::shared_mutex cacheMutex;

		// 
		std::thread invalidationThread {};

	private:
		// Fires periodically in a separate thread and erases the 
		// items from cache that are older than the ttlSeconds
		void onTimer() {
			while (true) {
				const time_t now {
					std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())
				};

				for (auto it = cache.begin(); cache.end() != it;) {
					if (now - it->second.timestamp > DEFAULT_TTL) {
						std::cout << "Removing record {" << it->first << ", " << it->second.get() << "}\n";
						cache.erase(it++);
					} else {
						++it;
					}
				}
				
				std::this_thread::sleep_for(SLEEP_TIMEOUT);
			}
		}

	public:
		// Initializes with a given ttl (in seconds)
		explicit TTLCache(uint32_t ttl = DEFAULT_TTL) : ttlSeconds{ ttl } {
			invalidationThread = std::thread(&TTLCache::onTimer, this);
		}

		~TTLCache() {
			invalidationThread.join();
		}

		// Adds an item to the cache
		template<typename ... Args>
		bool put(std::string&& key, Args&& ... params) noexcept {
			std::lock_guard<std::shared_mutex> lock(cacheMutex);
			return cache.try_emplace(std::move(key), std::forward<Args>(params)...).second;
		}

		std::optional<data_reference> get(const std::string& key) noexcept {
			std::shared_lock<std::shared_mutex> lock_shared(cacheMutex);
			auto iter = cache.find(key);
			return cache.end() != iter ?
				std::make_optional<data_reference>(iter->second.storage) : std::nullopt;
		}

		// Gets a value from cache if exists otherwise returns empty std::any
		std::optional<data_reference> get(std::string&& key) noexcept {
			std::shared_lock<std::shared_mutex> lock_shared(cacheMutex);
			auto iter = cache.find(key);
			return cache.end() != iter ?
				std::make_optional<data_reference>(iter->second.storage) : std::nullopt;
		}

		// TODO: Implement 
		// Erases an item for a given key if exists
		// void erase(const std::string& key);
	};

	//--------------------------------------------------------------------------------

	void Test_Item() {

		{
			Item<Integer> val(123);
			std::cout << val.get() << std::endl;

			auto& x = val.get();
			x.setValue(321);

			std::cout << val.get() << std::endl;
		}
	}

	void Delete_Record_After_Timeout() {
		TTLCache<Integer> numbersCache;

        [[maybe_unused]]
        bool result = numbersCache.put("Key1", 123);

		while (true) {
			std::optional<std::reference_wrapper<Integer>> data = numbersCache.get("Key1");
			if (data.has_value()) {
				std::cout << data.value().get().getValue() << std::endl;
			}
			else {
				std::cout << "Not found.!" << std::endl;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}
	}
}


void TTLCache::TEST_ALL() {

	// Cache_ANY::Test();


	// -------------------------------------------------------------------//

	// Cache::Test_Item();
	Cache::Delete_Record_After_Timeout();

}