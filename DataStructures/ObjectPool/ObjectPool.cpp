//============================================================================
// Name        : ObjectPools.cpp
// Created on  : 31.03.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Object pools src
//============================================================================

#include "ObjectPool.h"

#include <memory>
#include <vector>
#include <new>
#include <iostream>
#include <iterator>
#include <cassert>
#include <numeric>
#include <memory_resource>

#include "PerfUtilities.hpp"

namespace ObjectPool::TestTypes {

	template<size_t N>
	class Object  {
		char buffer[N]{ 0 };

	public:
		Object() {}

		/*
		Object(const Object& obj) {}
		Object& operator=(Object& right) {}

		Object(Object&& obj) noexcept {}
		Object& operator=(Object&& right) noexcept {}
		*/
	};

	using TypeTiny   = Object<sizeof(int)>;
	using TypeSmall  = Object<128>;
	using TypeMedium = Object<1024>;
	using TypeLarge  = Object<1024 * 64>;

}

namespace ObjectPool::MyPools1 {

	template<class T, size_t _Size = 128>
	class Pool {
	private:
		void* raw_mem_block;
		T* pool;
		T* states[_Size]{ nullptr };
		std::size_t m_size = 0;

	public:
		Pool() {
			this->raw_mem_block = operator new[](_Size * sizeof(T));
			this->pool = static_cast<T*>(this->raw_mem_block);
		}

		template<class ...Args>
		T* getObject(Args&& ... arguments) {

			// TODO: possible error handling
			if (m_size >= _Size)
				throw std::bad_alloc{};

			// TODO: Optimize??
			// TODO: Make thread safe
			size_t pos = 0;
			for (; pos < _Size; pos++)
				if (nullptr == states[pos])
					break;

			this->states[pos] = new (&this->pool[pos]) T(std::forward<Args>(arguments)...);
			m_size++;
			return this->states[pos];
		}

		void deleteObject(const T* obj) {
			for (size_t pos = 0; pos < _Size; pos++)
				if (obj == states[pos]) {
					pool[pos].~T();
					states[pos] = nullptr;
					m_size--;
				}
		}

		/*

		~Pool() {
			// Call destructors for all 'alive' objects:
			for (size_t pos = 0; pos < max_size; pos++) {
				if (nullptr != states[pos]) {
					pool[pos].~T();
					states[pos] = nullptr;
				}
			}
			// Delete memory:
			operator delete[](this->raw_mem_block);
		}
		*/

		~Pool() {
			auto ptr = std::launder(reinterpret_cast<T*>(pool));
			std::destroy(ptr, ptr + _Size);
			// Deallocate memory:
			operator delete[](this->raw_mem_block);
		}
	};


	void PerformanceTests()
	{

		using TestType = TestTypes::TypeLarge;
		constexpr size_t size = 256;
		TestType* ints[size];
		constexpr size_t MAX_COUNT = 64;

		Pool<TestType, size> pool;

		std::cout << " ======================== Using std::new(): =========================\n";

		{
			const utilities::perf::ScopedTimer timer { "Using std::new()" };;
			for (size_t i = 0; i < MAX_COUNT; i++) {
				for (size_t n = 0; n < MAX_COUNT; n++) {
					for (size_t k = 0; k < size; k++)
						ints[k] = new TestType;
					for (size_t k = 0; k < size; k++)
						delete ints[k];
				}
			}
		}

		std::cout << " ======================== Using pool: =========================\n";

		{
			const utilities::perf::ScopedTimer timer { "Using pool" };;
			for (size_t i = 0; i < MAX_COUNT; i++) {
				for (size_t n = 0; n < MAX_COUNT; n++) {
					for (size_t k = 0; k < size; k++)
						ints[k] = pool.getObject();
					for (size_t k = 0; k < size; k++)
						pool.deleteObject(ints[k]);
				}
			}
		}
	}
}

namespace ObjectPool::MyPools2 {

	template<class T, size_t _Size>
	class Pool_AlignedStorage {
	private:
		typename std::aligned_storage<sizeof(T), alignof(T)>::type pool[_Size];
		T* states[_Size]{ nullptr };
		std::size_t m_size = 0;

	public:
		Pool_AlignedStorage() {
			// this->raw_mem_block = operator new[](max_size * sizeof(T));
			// this->pool = static_cast<T*>(this->raw_mem_block);
		}

		template<class ...Args>
		T* getObject(Args&& ... arguments) {

			// TODO: possible error handling
			if (m_size >= _Size)
				throw std::bad_alloc{};

			// TODO: Optimize??
			// TODO: Make thread safe
			size_t pos = 0;
			for (; pos < _Size; pos++)
				if (nullptr == states[pos])
					break;

			this->states[pos] = new (&this->pool[pos]) T(std::forward<Args>(arguments)...);
			m_size++;
			return this->states[pos];
		}

		void deleteObject(const T* obj) {
			for (size_t pos = 0; pos < _Size; pos++)
				if (obj == states[pos]) {
					reinterpret_cast<T*>(&pool[pos])->~T();
					states[pos] = nullptr;
					m_size--;
				}
		}

		~Pool_AlignedStorage() {
			// Call destructors for all 'alive' objects:
			for (size_t pos = 0; pos < _Size; pos++) {
				if (nullptr != states[pos]) {
					reinterpret_cast<T*>(&pool[pos])->~T();
					states[pos] = nullptr;
				}
			}
		}
	};


	void PerformanceTests()
	{
		using TestType = TestTypes::TypeLarge;
		constexpr size_t size = 256;
		TestType* ints[size];
		constexpr size_t MAX_COUNT = 64;

		Pool_AlignedStorage<TestType, size> pool;

		std::cout << " ======================== Using std::new(): =========================\n";

		{
			const utilities::perf::ScopedTimer timer { "Using std::new()" };
			for (size_t i = 0; i < MAX_COUNT; i++) {
				for (size_t n = 0; n < MAX_COUNT; n++) {
					for (size_t k = 0; k < size; k++)
						ints[k] = new TestType;
					for (size_t k = 0; k < size; k++)
						delete ints[k];
				}
			}
		}

		std::cout << " ======================== Using pool: =========================\n";

		{
			const utilities::perf::ScopedTimer timer { "Using pool" };
			for (size_t i = 0; i < MAX_COUNT; i++) {
				for (size_t n = 0; n < MAX_COUNT; n++) {
					for (size_t k = 0; k < size; k++)
						ints[k] = pool.getObject();
					for (size_t k = 0; k < size; k++)
						pool.deleteObject(ints[k]);
				}
			}
		}
	}
}

namespace ObjectPool::GoodPools_Basic {

	template <typename T, typename Allocator = std::allocator<T>>
	class ObjectPool
	{
	private:
		// Contains chunks of memory in which instances of T will be created.
		// For each chunk, the pointer to its first object is stored.
		std::vector<T*> m_pool;

		// Contains pointers to all free instances of T that
		// are availableWorkers in the pool.
		std::vector<T*> m_freeObjects;

		// The number of T instances that should fit in the first allocated chunk.
		static const size_t ms_initialChunkSize { 5 };

		// The number of T instances that should fit in a newly allocated chunk.
		// This value is doubled after each newly created chunk.
		size_t m_newChunkSize { ms_initialChunkSize };

		// Creates a new block of uninitialized memory, big enough to hold
		// m_newChunkSize instances of T.
		void addChunk() {
			// Allocate a new chunk of uninitialized memory big enough to hold
			// m_newChunkSize instances of T, and add the chunk to the pool.
			auto* firstNewObject { m_allocator.allocate(m_newChunkSize) };
			m_pool.push_back(firstNewObject);

			// Create pointers to each individual object in the new chunk
			// and store them in the list of free objects.
			auto oldFreeObjectsSize{ m_freeObjects.size() };
			m_freeObjects.resize(oldFreeObjectsSize + m_newChunkSize);
			std::iota(begin(m_freeObjects) + oldFreeObjectsSize, end(m_freeObjects), firstNewObject);

			// Double the chunk size for next time.
			m_newChunkSize *= 2;
		}

		// The allocator to use for allocating and deallocating chunks.
		Allocator m_allocator;

	public:
		ObjectPool() = default;

		explicit ObjectPool(const Allocator& allocator) : m_allocator{ allocator } {
			// Trivial
		}

		virtual ~ObjectPool() {
			// Note: this implementation assumes that all objects handed out by this
			// pool have been returned to the pool before the pool is destroyed.
			// The following statement asserts if that is not the case.
			assert(m_freeObjects.size() == ms_initialChunkSize * (std::pow(2, m_pool.size()) - 1));

			// Deallocate all allocated memory.
			size_t chunkSize{ ms_initialChunkSize };
			for (auto* chunk : m_pool) {
				m_allocator.deallocate(chunk, chunkSize);
				chunkSize *= 2;
			}
		}

		// Allow move construction and move assignment.
		ObjectPool(ObjectPool&& src) noexcept = default;
		ObjectPool& operator=(ObjectPool&& rhs) noexcept = default;

		// Prevent copy construction and copy assignment.
		ObjectPool(const ObjectPool& src) = delete;
		ObjectPool& operator=(const ObjectPool& rhs) = delete;

		// Reserves and returns an object from the pool. Arguments can be
		// provided which are perfectly forwarded to a constructor of T.
		template<typename... Args>
		std::shared_ptr<T> acquireObject(Args... args)
		{
			// If there are no free objects, allocate a new chunk.
			if (m_freeObjects.empty()) {
				addChunk();
			}

			// Get a free object.
			T* object{ m_freeObjects.back() };

			// Initialize, i.e. construct, an instance of T in an uninitialized block of memory
			// using placement new, and perfectly forward any provided arguments to the constructor.
			new(object) T{ std::forward<Args>(args)... };

			// Remove the object from the list of free objects.
			m_freeObjects.pop_back();

			// Wrap the initialized object and return it.
			return std::shared_ptr<T> { object, [this](T* object) {
				std::destroy_at(object);
				// Put the object back in the list of free objects.
				m_freeObjects.push_back(object);
			} };
		}
	};

	// ---------------------------------------------------------------------------

	void SimpleTest()
	{

		using TestType = TestTypes::TypeLarge;
		// using TestType = TestTypes::TypeMedium;
		ObjectPool<TestType> pool{};

		auto object{ pool.acquireObject() };
	}

	void PerformanceTests() {
		using TestType = TestTypes::TypeLarge;
		constexpr size_t size = 256;
		TestType* ints[size];
		constexpr size_t MAX_COUNT = 64;

		ObjectPool<TestType> pool{};
		std::cout << " ======================== Using std::new(): =========================\n";
		{
			const utilities::perf::ScopedTimer timer { "new" };
			for (size_t i = 0; i < MAX_COUNT; i++) {
				for (size_t n = 0; n < MAX_COUNT; n++) {
					for (size_t k = 0; k < size; k++)
						ints[k] = new TestType;
					for (size_t k = 0; k < size; k++)
						delete ints[k];
				}
			}
		}
		std::cout << " ======================== Using pool: =========================\n";
		{
			const utilities::perf::ScopedTimer timer { "pool" };
			for (size_t i = 0; i < MAX_COUNT; i++) {
				for (size_t n = 0; n < MAX_COUNT; n++) {
					for (size_t k = 0; k < size; k++)
						auto object{ pool.acquireObject() };
				}
			}
		}
	}
}

namespace ObjectPool::GoodPools_Tests {

	template <typename Ty, typename Allocator = std::allocator<Ty>>
	class ObjectPool final
	{
	private:
		using object_type = Ty;
		using pointer = object_type*;
		using size_type = typename std::vector<pointer>::size_type;

		static_assert(!std::is_same_v<object_type, void>, 
					  "Type of the Objects in the pool can not be void");

	private:
		std::vector<pointer> pool;
		std::vector<pointer> available;

		static constexpr size_type DEFAULT_CHUNK_SIZE { 5 };
		static constexpr size_type GROWTH_STRATEGY { 2 };

		size_type _new_block_size { DEFAULT_CHUNK_SIZE };
		size_type _size { 0 };
		size_type _capacity { 0 };

		void addChunk()
        {
			// Allocate a new chunk of uninitialized memory
			pointer newBlock { m_allocator.allocate(_new_block_size) };

			// Keep all allocated blocks in 'pool' to delete them later:
			pool.push_back(newBlock);

			available.resize(_new_block_size);
			std::iota(std::begin(available), std::end(available), newBlock);

			_capacity += _new_block_size;
			_new_block_size *= GROWTH_STRATEGY;
		}

		// The allocator to use for allocating and deallocating chunks.
		Allocator m_allocator;

    protected:

		struct Deleter final
        {
			ObjectPool* pool {nullptr};

			void operator()(pointer object) const noexcept
			{
				std::destroy_at(object);

				// Return object mem pointer back to pool
				pool->available.push_back(object);
				--pool->_size;
			}
		};

    public:
        using ObjectPtr = std::unique_ptr<object_type, Deleter>;

	public:
		ObjectPool() = default;

		explicit ObjectPool(const Allocator& allocator) : m_allocator{ allocator } {
			// Trivial
		}

		virtual ~ObjectPool()
        {   // Note: this implementation assumes that all objects handed out by this
			// pool have been returned to the pool before the pool is destroyed.
			// The following statement asserts if that is not the case.
			assert(available.size() == DEFAULT_CHUNK_SIZE * (std::pow(2, pool.size()) - 1));

			// Deallocate all allocated memory.
			size_t chunkSize{ DEFAULT_CHUNK_SIZE };
			for (auto* chunk : pool) {
				m_allocator.deallocate(chunk, chunkSize);
				chunkSize *= GROWTH_STRATEGY;
			}
		}

		// Allow move construction and move assignment.
		ObjectPool(ObjectPool&& src) noexcept = default;
		ObjectPool& operator=(ObjectPool&& rhs) noexcept = default;

		// Prevent copy construction and copy assignment.
		ObjectPool(const ObjectPool& src) = delete;
		ObjectPool& operator=(const ObjectPool& rhs) = delete;

		// Reserves and returns an object from the pool. Arguments can be
		// provided which are perfectly forwarded to a constructor of T.
		template<typename... Args>
		std::unique_ptr<object_type, Deleter> acquireObject(Args... args)
		{
			// If there are no free objects, allocate a new chunk.
			if (available.empty()) {
				addChunk();
			}

			// Get a free object.
			const pointer objectPtr { available.back() };

			// Initialize, i.e. construct, an instance of T in an uninitialized block of memory
			// using placement new, and perfectly forward any provided arguments to the constructor.
            pointer obj = new (objectPtr) object_type { std::forward<Args>(args)... };

			// Remove the object from the list of free objects.
			available.pop_back();
			++_size;

			// Wrap the initialized object and return it.
			return std::unique_ptr<object_type, Deleter> { objectPtr, Deleter{this}};
		}

		[[nodiscard]]
        size_type size() const noexcept {
			return _size;
		}

		[[nodiscard]]
        size_type capacity() const noexcept {
			return _capacity;
		}
	};

	// ---------------------------------------------------------------------------

	void SimpleTest()
	{
		using TestType = TestTypes::TypeLarge;
		// using TestType = TestTypes::TypeMedium;

		ObjectPool<TestType> pool {};
        using ObjectPtr = ObjectPool<TestType>::ObjectPtr;

		std::cout << "Pool address = " << &pool << std::endl;
		std::cout << "Capacity = " << pool.capacity() << std::endl;

        ObjectPtr object1 { pool.acquireObject() };
        ObjectPtr object2 { pool.acquireObject() };
        ObjectPtr object3 { pool.acquireObject() };

		std::cout << "Capacity = " << pool.capacity() << ". Size = " << pool.size() << std::endl;

        ObjectPtr object4 { pool.acquireObject() };
        ObjectPtr object5 { pool.acquireObject() };
        ObjectPtr object6 { pool.acquireObject() };

		std::cout << "Capacity = " << pool.capacity() << ". Size = " << pool.size() << std::endl;

        ObjectPtr object7 { pool.acquireObject() };
        ObjectPtr object8 { pool.acquireObject() };
        ObjectPtr object9 { pool.acquireObject() };

		std::cout << "Capacity = " << pool.capacity() << ". Size = " << pool.size() << std::endl;

		{
            ObjectPtr obj1 { pool.acquireObject() };
            ObjectPtr obj2 { pool.acquireObject() };
            ObjectPtr obj3 { pool.acquireObject() };
			std::cout << "Capacity = " << pool.capacity() << ". Size = " << pool.size() << std::endl;
		}

		std::cout << "Capacity = " << pool.capacity() << ". Size = " << pool.size() << std::endl;

		std::cout << typeid(object3).name() << std::endl;
	}

	void PerformanceTests() {
		using TestType = TestTypes::TypeLarge;
		constexpr size_t size = 256;
		TestType* ints[size];
		constexpr size_t MAX_COUNT = 64;

		ObjectPool<TestType> pool{};
		std::cout << " ======================== Using std::new(): =========================\n";

		{
			const utilities::perf::ScopedTimer timer { "new" };
			for (size_t i = 0; i < MAX_COUNT; i++) {
				for (size_t n = 0; n < MAX_COUNT; n++) {
					for (size_t k = 0; k < size; k++)
						ints[k] = new TestType;
					for (size_t k = 0; k < size; k++)
						delete ints[k];
				}
			}
		}
		std::cout << " ======================== Using pool: =========================\n";
		{
			const utilities::perf::ScopedTimer timer { "pool" };
			for (size_t i = 0; i < MAX_COUNT; i++) {
				for (size_t n = 0; n < MAX_COUNT; n++) {
					for (size_t k = 0; k < size; k++)
						auto object{ pool.acquireObject() };
				}
			}
		}
	}
}

namespace ObjectPool::GoodPools_MultiThreaded
{

    template <typename Ty, typename Allocator = std::allocator<Ty>>
    class ObjectPool final
    {
    private:
        using object_type = Ty;
        using pointer = object_type*;
        using size_type = typename std::vector<pointer>::size_type;

        static_assert(!std::is_same_v<object_type, void>,
                      "Type of the Objects in the pool can not be void");

    private:
        std::vector<pointer> pool;
        std::vector<pointer> available;

        static constexpr size_type DEFAULT_CHUNK_SIZE { 5 };
        static constexpr size_type GROWTH_STRATEGY { 2 };

        size_type _new_block_size { DEFAULT_CHUNK_SIZE };
        size_type _size { 0 };
        size_type _capacity { 0 };

        std::mutex mtx;

        void addChunk()
        {
            // std::lock_guard<std::mutex> lock {mtx};

            // Allocate a new chunk of uninitialized memory
            pointer newBlock { m_allocator.allocate(_new_block_size) };

            // Keep all allocated blocks in 'pool' to delete them later:
            pool.push_back(newBlock);

            available.resize(_new_block_size);
            std::iota(std::begin(available), std::end(available), newBlock);

            _capacity += _new_block_size;
            _new_block_size *= GROWTH_STRATEGY;
        }

        // The allocator to use for allocating and deallocating chunks.
        Allocator m_allocator;

    protected:

        struct Deleter final
        {
            ObjectPool* pool {nullptr};

            void operator()(pointer object) const noexcept
            {
                std::destroy_at(object);

                std::lock_guard<std::mutex> lock { pool->mtx };

                // Return object mem pointer back to pool
                pool->available.push_back(object);
                --(pool->_size);
            }
        };

    public:
        using ObjectPtr = std::unique_ptr<object_type, Deleter>;

    public:
        ObjectPool() = default;

        explicit ObjectPool(const Allocator& allocator) : m_allocator{ allocator } {
            // Trivial
        }

        virtual ~ObjectPool() {
            // Note: this implementation assumes that all objects handed out by this
            // pool have been returned to the pool before the pool is destroyed.
            // The following statement asserts if that is not the case.
            assert(available.size() == DEFAULT_CHUNK_SIZE * (std::pow(2, pool.size()) - 1));

            // Deallocate all allocated memory.
            size_t chunkSize{ DEFAULT_CHUNK_SIZE };
            for (auto* chunk : pool) {
                m_allocator.deallocate(chunk, chunkSize);
                chunkSize *= GROWTH_STRATEGY;
            }
        }

        // Allow move construction and move assignment.
        ObjectPool(ObjectPool&& src) noexcept = default;
        ObjectPool& operator=(ObjectPool&& rhs) noexcept = default;

        // Prevent copy construction and copy assignment.
        ObjectPool(const ObjectPool& src) = delete;
        ObjectPool& operator=(const ObjectPool& rhs) = delete;

        // Reserves and returns an object from the pool. Arguments can be
        // provided which are perfectly forwarded to a constructor of T.
        template<typename... Args>
        ObjectPtr acquireObject(Args... args)
        {
            pointer objectPtr {nullptr};

            {
                std::lock_guard<std::mutex> lock { mtx };
                // If there are no free objects, allocate a new chunk.
                if (available.empty()) {
                    addChunk();
                }

                // Get a free object.
                objectPtr = available.back();

                // Remove the object from the list of free objects.
                available.pop_back();
                ++(_size);
            }

            // Initialize, i.e. construct, an instance of T in an uninitialized block of memory
            // using placement new, and perfectly forward any provided arguments to the constructor.
            new (objectPtr) object_type { std::forward<Args>(args)... };

            // Wrap the initialized object and return it.
            return ObjectPtr { objectPtr, Deleter { this } };
        }

        [[nodiscard]]
        size_type size() const noexcept {
            return _size;
        }

        [[nodiscard]]
        size_type capacity() const noexcept {
            return _capacity;
        }
    };

    // ---------------------------------------------------------------------------

    void PerformanceTests()
    {
        using TestType = TestTypes::TypeLarge;
        constexpr size_t size = 256;
        TestType* ints[size];
        constexpr size_t MAX_COUNT = 64;

        ObjectPool<TestType> pool{};
        std::cout << " ======================== Using std::new(): =========================\n";

        {
            const utilities::perf::ScopedTimer timer { "new" };;
            for (size_t i = 0; i < MAX_COUNT; i++) {
                for (size_t n = 0; n < MAX_COUNT; n++) {
                    for (size_t k = 0; k < size; k++)
                        ints[k] = new TestType;
                    for (size_t k = 0; k < size; k++)
                        delete ints[k];
                }
            }
        }

        std::cout << " ======================== Using pool: =========================\n";

        {
            const utilities::perf::ScopedTimer timer { "pool" };;
            for (size_t i = 0; i < MAX_COUNT; i++) {
                for (size_t n = 0; n < MAX_COUNT; n++) {
                    for (size_t k = 0; k < size; k++)
                        auto object{ pool.acquireObject() };
                }
            }
        }
    }
}

void ObjectPool::TestAll()
{
	// MyPools1::PerformanceTests();
	// MyPools2::PerformanceTests();

	// GoodPools_Basic::SimpleTest();
	// GoodPools_Basic::PerformanceTests();

	// GoodPools_Tests::SimpleTest();
	GoodPools_Tests::PerformanceTests();

    // GoodPools_MultiThreaded::PerformanceTests();
};