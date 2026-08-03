/**============================================================================
Name        : ObjectPool_Growing.cpp
Created on  : 20.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ObjectPool_Growing
============================================================================**/

#include "ObjectPool_Growing.h"

#include <memory>
#include <vector>
#include <numeric>
#include <iostream>
#include <cassert>
#include <cmath>


namespace ObjectPool_Growing
{
	template <typename Ty, typename Allocator = std::allocator<Ty>>
	class ObjectPool final
	{
		using object_type = Ty;
		using pointer = object_type*;
		using size_type = typename std::vector<pointer>::size_type;

		static_assert(!std::is_same_v<object_type, void>,
					  "Type of the Objects in the pool can not be void");

		static constexpr size_type DEFAULT_CHUNK_SIZE { 5 };
		static constexpr size_type GROWTH_STRATEGY { 2 };

		void addChunk()
        {
			// Allocate a new chunk of uninitialized memory
			pointer newBlock { m_allocator.allocate(newBlockSize) };

			// Keep all allocated blocks in 'pool' to delete them later:
			pool.push_back(newBlock);

			available.resize(newBlockSize);
			std::iota(std::begin(available), std::end(available), newBlock);

			capacity += newBlockSize;
			newBlockSize *= GROWTH_STRATEGY;
		}

    protected:

		struct Deleter final
        {
			static inline ObjectPool* pool { nullptr };

			void operator()(pointer object) const noexcept
			{
				std::destroy_at(object);

				// Return object mem pointer back to pool
				pool->available.push_back(object);
				--pool->size;
			}
		};

    public:

        using ObjectPtr = std::unique_ptr<object_type, Deleter>;

		ObjectPool() {
			deleter.pool = this;
		}

		explicit ObjectPool(const Allocator& allocator) : m_allocator { allocator } {
			deleter.pool = this;
		}

		~ObjectPool()
        {
			assert(available.size() == DEFAULT_CHUNK_SIZE * (std::pow(2, pool.size()) - 1));

			// Deallocate all allocated memory.
			size_t chunkSize { DEFAULT_CHUNK_SIZE };
			for (pointer chunk : pool) {
				m_allocator.deallocate(chunk, chunkSize);
				chunkSize *= GROWTH_STRATEGY;
			}
		}

		/** Allow move construction and move assignment **/
		ObjectPool(ObjectPool&& src) noexcept = default;
		ObjectPool& operator=(ObjectPool&& rhs) noexcept = default;

		/** Prevent copy construction and copy assignment **/
		ObjectPool(const ObjectPool& src) = delete;
		ObjectPool& operator=(const ObjectPool& rhs) = delete;

		template<typename... Args>
		std::unique_ptr<object_type, Deleter> acquireObject(Args... args)
		{
			if (available.empty()) {
				addChunk();
			}

            pointer obj = new (available.back()) object_type { std::forward<Args>(args)... };
			available.pop_back();
			++size;

			/** Wrap the initialized object and return it **/
			return std::unique_ptr<object_type, Deleter> { obj, deleter };
		}

		[[nodiscard]]
        size_type Size() const noexcept {
			return size;
		}

		[[nodiscard]]
        size_type Capacity() const noexcept {
			return capacity;
		}

	private:
		Allocator m_allocator;
		Deleter deleter;

		std::vector<pointer> pool;
		std::vector<pointer> available;

		size_type newBlockSize { DEFAULT_CHUNK_SIZE };
		size_type size { 0 };
		size_type capacity { 0 };
	};
};


namespace ObjectPool_Growing::Tests
{
	template<size_t N>
	class Object
	{
		char buffer[N] { 0 };
	};

	using TypeTiny   = Object<sizeof(int)>;
	using TypeSmall  = Object<128>;
	using TypeMedium = Object<1024>;
	using TypeLarge  = Object<1024 * 64>;


	void test()
	{
		using Type = int;
		// using Type = TestTypes::TypeMedium;

		{
			ObjectPool<Type> pool {};
			using ObjectPtr = ObjectPool<Type>::ObjectPtr;

			std::cout << "Pool address = " << &pool << std::endl;
			std::cout << "Capacity = " << pool.Capacity() << std::endl;

			ObjectPtr object1 { pool.acquireObject() };
			ObjectPtr object2 { pool.acquireObject() };
			ObjectPtr object3 { pool.acquireObject() };

			ObjectPtr object11 { pool.acquireObject() };
			ObjectPtr object22 { pool.acquireObject() };
			ObjectPtr object33 { pool.acquireObject() };

			std::cout << "Pool address = " << &pool << std::endl;
			std::cout << "Capacity = " << pool.Capacity() << ", Size = " << pool.Size() << std::endl;
		}

		std::cout << "Done" << std::endl;
	}
}

void ObjectPool_Growing::TestAll()
{
	Tests::test();
}
