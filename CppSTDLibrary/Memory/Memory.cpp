//============================================================================
// Name        : Memory.cpp
// Created on  : 14.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Memory src class
//============================================================================


#include "Alignment.h"
#include "AlignedStackAllocator.h"
#include "CustomStackAllocator.h"
#include "MemoryUsageMonitor.h"
#include "MemoryPool.h"
#include "Launder.hpp"
#include "ObjectPool_Stack_FixedSize.h"

#include "Memory.h"
#include <memory>
#include <vector>
#include <new>
#include <cstring>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <string>
#include <vector>
#include <array>
#include <memory_resource>
#include <cstdlib> // for std::byte
#include <iomanip>
#include <functional>
#include <memory_resource>

#include "../Helpers/Helpers.h"

namespace {
	using Helpers::Integer;
}


#define DEBUG_OUTPUT
#define FOR(var_name, range) for (int var_name  = 0; var_name < range; ++var_name)

namespace Memory::Allocators {
	
	template <class T>
	class SimpleAllocator {
	private:
		static_assert(!std::is_same_v<T, void>, "Type of the allocator can not be void");
		size_t alloc_count{0};

	public:
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T* ;
		using reference = T&;
		using const_reference = const T&;

	public:
		SimpleAllocator() noexcept {
			std::cout << "Allocator createed" << std::endl;
		}
	
		[[nodiscard]]
		pointer allocate(const std::size_t _Count) {
			std::cout << "Allocate " << _Count << " objects of type " << typeid(value_type).name() 
				      << ". Alloc count: " << ++alloc_count << std::endl;
			return static_cast<pointer>(::operator new(_Count * sizeof(value_type)));
		}

		template <typename ObjectType, typename... Types>
		void construct(const ObjectType* ptr, Types&&... params) {
			::new (const_cast<void*>(static_cast<const volatile void*>(ptr)))
				ObjectType(std::forward<Types>(params)...);
		}

		void deallocate(pointer ptr, const std::size_t _Count) {
			std::cout << "Deallocate " << _Count << " objects of type " << typeid(value_type).name() << std::endl;
			::delete(ptr);
		}

		void destroy(pointer ptr) {
			std::cout << "destroy(" << *ptr << ")" << std::endl;
			ptr->~T();
		}
	};


	// template <class T, std::size_t _MaxSize>
	template <class T>
	class StackPoolAllocator {
	private:
		static_assert(!std::is_same_v<T, void>, "Type of the allocator can not be void");

		static inline constexpr size_t _MaxSize { 100 };
		typename std::aligned_storage<sizeof(T), alignof(T)>::type pool[_MaxSize] {0};

	public:
		using value_type = T;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;

	public:
		StackPoolAllocator() noexcept {
			std::cout << "Allocator created" << std::endl;
		}

		// ~StackPoolAllocator();

		[[nodiscard]]
		pointer allocate(const std::size_t _Count) {
			std::cout << "Allocate " << _Count << " objects of type " << typeid(value_type).name() << std::endl;
			return static_cast<pointer>(::operator new(_Count * sizeof(value_type)));
		}

		template <typename ObjectType, typename... Types>
		void construct(const ObjectType* ptr, Types&&... params) {
			std::cout << "Construct (" << ptr << ")" << std::endl;
			::new (const_cast<void*>(static_cast<const volatile void*>(ptr)))
				ObjectType(std::forward<Types>(params)...);
		}

		void deallocate(pointer ptr, const std::size_t _Count) {
			std::cout << "Deallocate " << _Count << " objects of type " << typeid(value_type).name() << std::endl;
			::delete(ptr);
		}

		void destroy(pointer ptr) {
			std::cout << "destroy(" << *ptr << ")" << std::endl;
			ptr->~T();
		}
	};


	// ----------------------------------------------------------------------------------------//

	void Basic_STD_Allocator()
	{
		std::allocator<Integer> alloc;
		Integer* memory{ alloc.allocate(1) };

		// Use placement new operator to construct a MyClass in place.
		new(memory) Integer {123};

		// Destroy MyClass instance.
		std::destroy_at(memory);

		// Deallocate memory block.
		alloc.deallocate(memory, 1);
		memory = nullptr;
	}


	void SimpleTest()
	{
		std::vector<int, SimpleAllocator<int>> numbers;
		for (int i : {1, 2, 3, 4, 5, 6, 7, 8, 9})
			numbers.push_back(i);

		for (auto i : numbers)
			std::cout << i << std::endl;
	}

	void StackAllocatorTests()
	{

		std::vector<int, StackPoolAllocator<int>> numbers;
		numbers.reserve(5);


		for (int i : {1, 2, 3, 4, 5})
			numbers.push_back(i);

		for (auto i : numbers)
			std::cout << i << std::endl;
	}


    template<class T>
    struct TracingAllocator
    {
        using value_type = T;
        using pointer = value_type*;

        TracingAllocator() = default;

        template<class U>
        constexpr explicit TracingAllocator(const TracingAllocator<U>&) noexcept {
        }

        [[nodiscard]]
        pointer allocate(std::size_t n)
        {
            if (n > std::numeric_limits<std::size_t>::max() / sizeof(value_type))
                throw std::bad_array_new_length();

            if (pointer ptr = static_cast<pointer>(std::malloc(n * sizeof(value_type))))
            {
                trace(ptr, n);
                return ptr;
            }

            throw std::bad_alloc();
        }

        void deallocate(pointer ptr, std::size_t n) noexcept
        {
            trace(ptr, n, 0);
            std::free(ptr);
        }

    private:

        void trace(pointer ptr, std::size_t n, bool alloc = true) const
        {
            std::cout << (alloc ? "Alloc: " : "Dealloc: ") << sizeof(value_type) * n
                      << " bytes at " << std::hex << std::showbase
                      << reinterpret_cast<void*>(ptr) << std::dec << '\n';
        }
    };


    void AllocateShared_And_Trace()
    {
        TracingAllocator<int> traceAllocator;
        auto deleter = [&traceAllocator](int* ptr){
            traceAllocator.deallocate(ptr, 1);
        };

        std::shared_ptr<int> sharedInt (traceAllocator.allocate(1), deleter, traceAllocator);
    }
}

namespace Memory::Allocators_Vector {

	template<typename _Ty>
	struct MyAllocator {
	public:
		using value_type = _Ty;
		using pointer = _Ty*;

		static inline constexpr std::align_val_t alignment =
			static_cast<std::align_val_t>(alignof(value_type));

	public:
		[[nodiscard]]
		constexpr pointer allocate(size_t _Count) {
			std::cout << "allocate(size=" << _Count << ", alignment = "
					  << static_cast<std::size_t>(alignment) << ")\n";
			return static_cast<pointer>(::operator new(_Count, alignment));
		}

		constexpr void deallocate(pointer ptr, size_t _Count) {
			std::cout << "deallocate(size=" << _Count << ", alignment = "
					  << static_cast<std::size_t>(alignment) << ")\n";
			::operator delete(static_cast<void*>(ptr), _Count, alignment);
		}

		template<typename U, typename... Args>
		static void construct(U* ptr, Args&& ... params) {
			std::cout << "construct()\n";
			::new (static_cast<void*>(ptr))U{ std::forward<Args>(params)... };
		}
	};

	//-----------------------------------------------------------//

	struct Long {
		long value;
	};


	class LongEx {
		long value;

	public:
		LongEx(long v) : value{ v } {
		}

		inline long getValue() const noexcept {
			return value;
		}
	};

	//-----------------------------------------------------------//


	void Emplace_CustomType_1() {
		std::vector<Long> values;
		values.emplace_back(123);

		std::cout << values.back().value << std::endl;
	}


	void Emplace_CustomType_2() {
		std::vector<LongEx> values;
		values.emplace_back(123);

		std::cout << values.back().getValue() << std::endl;
	}


	void Emplace_CustomType_Allocator() {
		std::vector<LongEx, MyAllocator<LongEx>> values;
		values.emplace_back(123);

		std::cout << values.back().getValue() << std::endl;
	}
}


namespace Memory::Utilities {

	class Long {
	protected:
		int value;

	public:
		Long() : value(0) {
			std::cout << "Long::Long(" << value << ")" << std::endl;
		}

		explicit Long(int val) : value(val) {
			std::cout << "Long::Long(" << value << ")" << std::endl;
		}

		Long(const Long &obj) {
			this->value = obj.value;
			std::cout << "Copy constructor: Long::Long(" << value << ")" << std::endl;
		}

		Long& operator=(const Long& right) {
			if (&right != this) {
				value = right.value;
			}
			std::cout << "Copy assignment: Long::Long(" << value << ")" << std::endl;
			return *this;
		}

		Long(Long && obj) noexcept : value(std::exchange(obj.value, 0)) {
			std::cout << "Move constructor: Long::Long(" << value << ")" << std::endl;
		}

		Long& operator=(Long&& right) noexcept {
			if (this != &right) {
				this->value = std::exchange(right.value, 0);
			}
			std::cout << "Move assignment: Long::Long(" << value << ")" << std::endl;
			return *this;
		}

		virtual ~Long() {
			std::cout << "Long::~Long(" << value << ")" << std::endl;
		}

		friend std::ostream& operator<<(std::ostream& stream, const Long& right) {
			stream << right.value;
			return stream;
		}

		inline operator long() const noexcept {
			return this->value;
		}
	};
}

namespace Memory {
	void Delete_Array() {
		Integer* data = new Integer[3];

		std::cout << data[1] << std::endl;

		delete[] data;
	}

	void NoThrowTest() {
		int* idx = new(std::nothrow) int;
		delete idx;
	}
};

namespace Memory::New_Placement {

	class Long {
	protected:
		long value;

	public:
		explicit Long(long v) : value(v) {
			std::cout << "Long::Long(" << this->value << ")" << std::endl;
		}

		Long() = delete;
		virtual ~Long() {
			std::cout << "Long::~Long(" << this->value << ")" << std::endl;
		}

		friend std::ostream& operator<<(std::ostream& stream, const Long& right) {
			stream << right.value;
			return stream;
		}
	};

	void Test() {

		void *rawMemory = operator new[](10 * sizeof(Long));
		Long *data = static_cast<Long*>(rawMemory);
		for (int i = 0; i < 10; ++i) {
			new (data + i) Long(i);
		}

		for (int i = 0; i < 10; i++) {
			std::cout << data[i] << std::endl;
		}

		// CleanUP: Call destructors.
		for (int i = 9; i >= 0; --i) {
			data[i].~Long();
		}
		operator delete[](rawMemory);
	}

	void Test2() {

		const int N = 10;
		const int BUFF_SIZE = N * sizeof(double);

		char buffer[BUFF_SIZE];     
		double *ptr = nullptr;    
		ptr = new (buffer) double[N];     
		double value = 0.3;
		for (int i = 0; i < N; i++) {
			ptr[i] = value;         
			value = value + 0.3;    
		}

		for (int i = 0; i < N; i++) {        
			std::cout << ptr[i] << '\n';
		}
	}

	void Test3() {

		void *rawMemory = operator new[](10 * sizeof(Integer));
		Integer *data = static_cast<Integer*>(rawMemory);

		Integer* integer = new (data + 0) Integer(123);
		integer->printInfo();

		data[0].~Integer();


		/*
		for (int i = 9; i >= 0; --i) {
			data[0].~Integer();
		}
		*/
		operator delete[](rawMemory);
	}

	void Good_Example() {
		constexpr size_t size = 5;
		using T = Integer;

		void *raw_memory = operator new[](size * sizeof(T));
		T *pool = static_cast<T*>(raw_memory);
		FOR (pos, size) new (&pool[pos]) T(pos); // Create 5 Integer()'s 


		auto ptr = std::launder(reinterpret_cast<T*>(pool));
		std::destroy(ptr, ptr + size);

		operator delete[](raw_memory);
	}
};


namespace Memory::ObjectPool {

	template<class T, size_t max_size>
	class Pool {
	private:
		void *raw_mem_block;
		T *pool;
		T* states[max_size] { nullptr };
		std::size_t m_size = 0;

	public:
		Pool() {
			this->raw_mem_block = operator new[](max_size * sizeof(T));
			this->pool = static_cast<T*>(this->raw_mem_block);
		}

		template<class ...Args>
		T* getObject(Args&& ... arguments) {

			// TODO: possible error handling
			if (m_size >= max_size) 
				throw std::bad_alloc{};

			// TODO: Optimize??
			// TODO: Make thread safe
			size_t pos = 0;
			for (; pos < max_size; pos++)
				if (nullptr == states[pos])
					break;

			this->states[pos] = new (&this->pool[pos]) T(std::forward<Args>(arguments)...);
			m_size++;
			return this->states[pos];
		}

		void deleteObject(const T* obj) {
			for (size_t pos = 0; pos < max_size; pos++)
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
			std::destroy(ptr, ptr + max_size);
			// Deallocate memory:
			operator delete[](this->raw_mem_block);
		}
	};

	template<class T, size_t max_size>
	class Pool_AlignedStorage {
	private:
		typename std::aligned_storage<sizeof(T), alignof(T)>::type pool[max_size];
		T* states[max_size]{ nullptr };
		std::size_t m_size = 0;

	public:
		Pool_AlignedStorage() {
			//this->raw_mem_block = operator new[](max_size * sizeof(T));
			//this->pool = static_cast<T*>(this->raw_mem_block);
		}

		template<class ...Args>
		T* getObject(Args&& ... arguments) {

			// TODO: possible error handling
			if (m_size >= max_size)
				throw std::bad_alloc{};

			// TODO: Optimize??
			// TODO: Make thread safe
			size_t pos = 0;
			for (; pos < max_size; pos++)
				if (nullptr == states[pos])
					break;

			this->states[pos] = new (&this->pool[pos]) T(std::forward<Args>(arguments)...);
			m_size++;
			return this->states[pos];
		}

		void deleteObject(const T* obj) {
			for (size_t pos = 0; pos < max_size; pos++)
				if (obj == states[pos]) {
					reinterpret_cast<T*>(&pool[pos])->~T();
					states[pos] = nullptr;
					m_size--;
				}
		}

		~Pool_AlignedStorage() {
			// Call destructors for all 'alive' objects:
			for (size_t pos = 0; pos < max_size; pos++) {
				if (nullptr != states[pos]) {
					reinterpret_cast<T*>(&pool[pos])->~T();
					states[pos] = nullptr;
				}
			}
		}
	};


	void Test1() {

		constexpr size_t size = 256;
		Pool<Integer, size> pool;
		//Pool_AlignedStorage<Integer, size> pool;


		/*
		Integer* integer = pool.getObject(11);
		Integer* integer1 = pool.getObject(22);
		Integer* integer2 = pool.getObject(33);

		pool.deleteObject(integer1);
		*/

		for (size_t i = 0; i < size; i++) {
			Integer* integer = pool.getObject(i);
			integer->printInfo();
		}
	

		/*
		void *rawMemory = operator new[](10 * sizeof(Integer));
		Integer *data = static_cast<Integer*>(rawMemory);

		Integer* integer = new (data + 0) Integer(123);
		integer->printInfo();

		data[0].~Integer();
		operator delete[](rawMemory);
		*/
	}
}


namespace Memory::Pointers {

	void Memcpy_Test1() {

		void* raw = operator new[](sizeof(Integer));
		Integer val(123);

		memmove(raw, &val, sizeof(Integer));

		val.printInfo();
		Integer* ii = static_cast<Integer*>(raw);
		ii->printInfo();

		operator delete[](raw);
	}

	void Pointer_to_array() {
		{
			constexpr size_t length = 5;
			Integer *ints = new Integer[length];
			for (int i = 0; i < length; i++)
				ints[i].setValue(i);

			delete[] ints;
		}
		std::cout << "------- TEST2 -------" << std::endl;
		{
			constexpr size_t length = 5;
			Integer *ints[length];
			for (int i = 0; i < length; i++)
				ints[i] = new Integer(i);
			for (int i = length; i > 0; i--)
				delete ints[i-1];
		}
	}

	void Pointer_to_array_TTEEESSSTT() {
		constexpr size_t length = 5;
		Integer *ints = new Integer[length];
		for (int i = 0; i < length; i++)
			ints[i].setValue(i);
	}
	

	void Arrays_Of_Pointers() {
		constexpr size_t length = 5;
		Integer* ints[length];

		for (int pos = 0; pos < length; pos++)
			ints[pos] = new Integer(pos);
		for (size_t pos = length ; pos > 0; pos--)
			delete ints[pos - 1];
	}

	void Arrays_Of_Pointers2() {

		constexpr size_t length = 5;
		void *ints1 = new int[length];
		Integer *ints = static_cast<Integer*>(ints1);
		for (int i = 0; i < length; ++i)
		{
			memmove(ints + i, new Integer(i), sizeof(Integer*));
		}

		for (int i = length; i > 0; --i)
			ints[i - 1].~Integer();
		operator delete[](ints1);

	}

	void Pointer_to_Arrays_NewPlacement() {
		constexpr size_t length = 5;
		void *rawMemory = operator new[](length * sizeof(Integer));
		Integer *ints = static_cast<Integer*>(rawMemory);

		for (int i = 0; i < length; ++i)
			new (ints + i) Integer(i);

		for (int i = 0; i < length; i++) 
			std::cout << ints[i] << std::endl;
		
		// Call Dtor for each object in memory
		for (int i = length; i > 0; --i) 
			ints[i-1].~Integer();
		operator delete[](rawMemory);
	}

	/*
	void Pointer_to_Arrays_ConstructAt() {
		constexpr size_t length = 5;
		void *rawMemory = operator new[](length * sizeof(Integer));
		Integer *ints = static_cast<Integer*>(rawMemory);

		for (size_t i = 0; i < length; ++i)
			new (ints + i) Integer(i);

		for (int i = 0; i < length; i++)
			construct_at<Integer>(ints[i], i);
	
		// Call Dtor for each object in memory
		for (int i = length; i > 0; --i)
			ints[i - 1].~Integer();
		operator delete[](rawMemory);
	}
	*/
}


namespace Memory::Destroy {

	void Call_Destructor_Manualy() {
		constexpr size_t length = 5;
		Integer *ints = new Integer[length];
		for (int i = 0; i < length; i++)
			ints[i].~Integer();
	}

	void Destroy_Simple() {
		constexpr size_t length = 5;
		alignas(Integer) unsigned char pool[sizeof(Integer) * 8];
		FOR(i, length) new(pool + sizeof(Integer) * i) Integer{ i }; // manually construct objects
		auto ptr = std::launder(reinterpret_cast<Integer*>(pool));   // Get memory address as Integer's block pointer
		std::destroy(ptr, ptr + length);
	}

	void Destroy_Test1() {
		constexpr size_t length = 10;
		alignas(Integer) unsigned char rawMemory[sizeof(Integer) * length];

		for (int i = 0; i < length; ++i)
			new(rawMemory + sizeof(Integer) * i) Integer(i);

		auto ptr = std::launder(reinterpret_cast<Integer*>(rawMemory));

		std::cout << "-------------------------- std::destroy(ptr, ptr + 5):" << std::endl;
		std::destroy(ptr, ptr + 5);
		std::cout << "-------------------------- std::destroy(ptr + 5, ptr + length):" << std::endl;
		std::destroy(ptr + 5, ptr + length);
	}

	void Destroy_Test2() {
		constexpr size_t length = 10;
		void *rawMemory = operator new[](length * sizeof(Integer));
		Integer *ints = static_cast<Integer*>(rawMemory);

		for (int i = 0; i < length; ++i)
			new (ints + i) Integer(i);

		std::cout << "-------------------------- std::destroy(ptr, ptr + 5):" << std::endl;
		std::destroy(ints, ints + 5);
		std::cout << "-------------------------- std::destroy(ptr + 5, ptr + length):" << std::endl;
		std::destroy(ints + 5, ints + length);
		// delete[] rawMemory;
		operator delete[](rawMemory);
	}

	void Destroy_N_Test() {
		constexpr size_t length = 10;
		void *rawMemory = operator new[](length * sizeof(Integer));
		Integer *ints = static_cast<Integer*>(rawMemory);

		for (int i = 0; i < length; ++i)
			new (ints + i) Integer(i);

		std::cout << "-------------------------- std::destroy_n(ptr, ptr + 5):" << std::endl;
		std::destroy_n(ints, 5);
		std::cout << "-------------------------- std::destroy(ptr + 5, ptr + length):" << std::endl;
		std::destroy_n(ints+ 5, length - 5);
		// delete[] rawMemory;
		operator delete[](rawMemory);
	}

#define FOR(var_name, range) for (int var_name  = 0; var_name < range; ++var_name)

	void Destroy_AT_Simple() {
		struct Tracer {
			int value;
			~Tracer() { std::cout << value << " destructed\n"; }
		};

		alignas(Tracer) unsigned char buffer[sizeof(Tracer) * 8];
		FOR(i, 8) new(buffer + sizeof(Tracer) * i) Tracer{ i }; //manually construct objects

		auto ptr = std::launder(reinterpret_cast<Tracer*>(buffer));
		FOR(i, 8) std::destroy_at(ptr + i);
	}

	void Destroy_At_PlacmentNew() {
		constexpr size_t length = 10;
		void *rawMemory = operator new[](length * sizeof(Integer));
		Integer *ints = static_cast<Integer*>(rawMemory);

		FOR(i, length)
			new (ints + i) Integer(i);

		for (int i = length; i > 0; i--)
			std::destroy_at(ints + (i - 1));
		operator delete[](rawMemory);
	}
}

////////////////////////////////////////////////////////////////////////

namespace Memory::CustomAllocator {

	template<class T>
	class TrackingAllocator
	{
	public:
		using value_type = T;
		using pointer = T * ;
		using const_pointer = const T *;
		using size_type = size_t;

		TrackingAllocator() = default;

		template<class U>
		TrackingAllocator(const TrackingAllocator<U> &other) {
			// TODO:
		}

		~TrackingAllocator() = default;

		pointer allocate(size_type numObjects) {
			mAllocations += numObjects;
			std::cout << "Custom allocate <" << typeid(T).name() << "> object" << std::endl;
			return static_cast<pointer>(operator new(sizeof(T) * numObjects));
		}

		void deallocate(pointer p, size_type numObjects) {
			std::cout << "Custom deallocate <" << typeid(T).name() << "> object" << std::endl;
			operator delete(p);
		}

		size_type get_allocations() const {
			return mAllocations;
		}

	private:
		static size_type mAllocations;
	};

	template<class T>
	typename TrackingAllocator<T>::size_type TrackingAllocator<T>::mAllocations = 0;

	void Test()
	{
		std::vector<int, TrackingAllocator<int>> v(1);
		std::cout << v.get_allocator().get_allocations() << std::endl;
	}

	void Test_Integer()
	{
		std::vector<Integer, TrackingAllocator<Integer>> v;
		v.emplace_back(1);
		v.emplace_back(2);

		std::cout << v.get_allocator().get_allocations() << std::endl;
	}
}

namespace Memory::OperatorNew {

	class Object {
	public:
		int value;
	
	public:
		Object(int v = 0) : value(v) {
			std::cout << "Object::Object(" << value << ") constructed at [" << this << "]" << std::endl;
		}
		~Object() {
			std::cout << "~Object::Object(" << value << ")" << std::endl;
		}
	};

	class IntegerObject {
	public:
		Integer value;

	public:
		IntegerObject(int v = 0) : value(v) {
			std::cout << "Object::Object(" << value << ") constructed at [" << this << "]" << std::endl;
		}
		~IntegerObject() {
			std::cout << "~Object::Object(" << value << ")" << std::endl;
		}

	};

	// Allocates memory by calling: operator new (sizeof(Object))
	// and then constructs an object at the newly allocated space
	void Classic_Allocation() {
		std::unique_ptr<Object> obj = std::make_unique<Object>(12345);
	}

	// Allocates memory by calling: operator new (sizeof(Object),std::nothrow)
	void Allocate_NoThrow() {
		Object *objPtr = new (std::nothrow) Object(222);
		delete objPtr;
	}

	// Allocates memory by calling: operator new (sizeof(Object),std::nothrow)
	// and then constructs an object at the newly allocated space
	void Allocate_Placement()
    {
		Object *objPtr1 = new (std::nothrow) Object(333);

		std::cout << "objPtr1->value = " << objPtr1->value << "\n\n";
	
		// Does NOT allocate NEW memory -- calls: operator new (sizeof(Object), objPtr) but constructs an object at objPtr address
        objPtr1->~Object();
		Object *objPtr2 = new (objPtr1) Object(444);

		std::cout << "objPtr1->value = " << objPtr1->value << std::endl;
		std::cout << "objPtr2->value = " << objPtr2->value << std::endl;

		std::cout << "\nobjPtr1 = " << objPtr1 << std::endl;
		std::cout << "objPtr2 =  " << objPtr2 << std::endl;
		
		delete objPtr1;

		std::cout << "\nobjPtr1 = " << objPtr1 << std::endl;
		std::cout << "objPtr2 =  " << objPtr2 << std::endl;

		// delete objPtr2; // ERROR
	}

	// Allocates memory by calling: operator new (sizeof(Object),std::nothrow)
	// and then constructs an object at the newly allocated space
	void Allocate_Placement_DestroyAt()
    {
		{
			Object* objPtr = new Object(333);
			std::destroy_at(objPtr); // Just call DTor

			std::cout << objPtr << " --> " << objPtr->value << std::endl;
			
			new (objPtr) Object(444); // Do not deallocate

			std::cout << objPtr << " --> " << objPtr->value << std::endl;

			delete objPtr;
		}

		std::cout << "----------------------------------------\n";

		{
			Object* objPtr = new Object(333);
			std::destroy_at(objPtr); // Just call DTor

			new Object(444); // Do not deallocate
			delete objPtr;
		}
	}


	// Allocates memory by calling: operator new (sizeof(Object),std::nothrow)
	// and then constructs an object at the newly allocated space
	void Allocate_Placement2() {
		IntegerObject *objPtr = new (std::nothrow) IntegerObject(333);

		// Just call DTor
		std::destroy_at(objPtr);

		// Does NOT allocate NEW memory -- calls: operator new (sizeof(Object), objPtr) but constructs an object at objPtr address
		new (objPtr) IntegerObject(444);

		delete objPtr;
	}

	// Notice though that calling this function directly does not construct an object:
	void CreateObject_NoConstructor() {
		
		// allocates memory by calling: operator new (sizeof(MyClass))
		// but does not call MyClass's constructor
		Object *objPtr = (Object*) ::operator new (sizeof(Object));

		delete objPtr;
	}

    void Construct_AT() {

        alignas(Integer) unsigned char storage[sizeof(Integer)];

        Integer* ptr = std::construct_at(reinterpret_cast<Integer*>(storage), 12345);
        ptr->printInfo();

        std::destroy_at(ptr);
    }
}

namespace Memory::Reload_New_and_Delete
{
	class Object {
	public:
		int value;

	public:
		Object(int v = 0) : value(v) {
			std::cout << "Object::Object(" << value << ") constructed at [" << this << "]" << std::endl;
		}
		~Object() {
			std::cout << "~Object::Object(" << value << ")" << std::endl;
		}

		void *operator new(size_t size) {
			std::cout << "new operator overloaded" << std::endl;
			return malloc(size);
		}

		void operator delete(void *ptr) {
			std::cout << "delete operator overloaded" << std::endl;
			free(ptr);
		}

		friend std::ostream& operator<<(const std::ostream& stream, const Object& obj);

	};

	std::ostream& operator<<(std::ostream& stream, const Object& obj) {
		stream << obj.value;
		return stream;
	}

	//////////////////////////////////////////////////////////////////////////////////////////

	void TestOverloadedNew() {

		{
			Object* obj = new Object(222);
			std::cout << *obj << std::endl;
			delete obj;
		}

		std::cout << "===========================Test2============================\n";

		{
			auto objPtr = std::make_unique<Object>(222);
			std::cout << *objPtr << std::endl;
		}
	}


	//-------------------------------- TEST2 -------------------------------------------------//


	class MyType1 final {
	private:
		int value {0};

	public:
		MyType1(int v = 0) : value{ v } {
			std::cout << "MyType1::MyType1(" << value << ")\n";
		}

		~MyType1() {
			std::cout << "~MyType1::MyType1(" << value << ")" << std::endl;
		}

		inline int getValue() const noexcept {
			return value;
		}


		/*
		template <class... _Types>
		friend std::unique_ptr<MyType1> std::make_unique<MyType1>(_Types&&...);
		*/


		// friend std::unique_ptr<MyType1> std::make_unique<MyType1>(MyType1&&);


		template <class... _Types>
		static std::unique_ptr<MyType1> makeUnique(_Types&&... params)
		{
			return std::unique_ptr<MyType1>(new MyType1(std::forward<_Types>(params)...));
		}

		

	protected:
		void* operator new(size_t objectSize) {
			std::cout << "MyType1::new() opearator" << std::endl;

			// MyType1* ptr = (MyType1*) ::operator new (objectSize);
			void* ptr = malloc(objectSize);

			std::cout << objectSize << " bytes allocated\n";
			return ptr;
		}
	};



	void Disable_New__UseOnly_MakeUnique() {

		// auto obj = makeUnique(123);
		auto obj = MyType1::makeUnique(123);

		//MyType1* obj = new MyType1(222);
		//std::cout << obj->getValue() << std::endl;
		//delete obj;
	}
}

namespace Memory::Reload_New_and_Delete
{
    struct Deleters
    {
        void* operator new(std::size_t size) noexcept {
            decltype(auto) memBlock = malloc(size);
            return memBlock ? memBlock: throw std::bad_alloc{};
        }

        void operator delete(void* ptr, std::size_t size) noexcept {
            std::cout << "free: " << size << std::endl;
            free(ptr);
        }
    };

    struct Base: Deleters
    {
        std::array<std::byte, 8> buffer {};
        virtual ~Base() = default;
        virtual void f() {}
    };

    struct BaseBad: Deleters
    {
        std::array<std::byte, 8> buffer {};
        virtual void f() {}
    };

    struct Derived : public Base
    {
        std::array<std::byte, 8> buffer2 {};
    };

    struct DerivedBad : public BaseBad
    {
        std::array<std::byte, 8> buffer2 {};
    };

    void deleteOne(Base* base) {
        delete base;
    }

    void deleteTwo(BaseBad* base) {
        delete base;
    }

    void Delete_Object_Size()
    {
        deleteOne(new Base());
        deleteOne(new Derived());

        deleteTwo(new DerivedBad());

        /**
            free: 16
            free: 24
            free: 16
        **/
    }

}



//--------------------------- Relead global NEW and DELETE ------------------------------------------
/*
void* operator new(size_t count) {
	std::cout << "allocating " << count << " bytes\n";
	return malloc(count);
}

void operator delete(void* ptr) noexcept {
	std::cout << "global op delete called\n";
	free(ptr);
}

*/

// TODO: Uncomment code above
namespace Memory::ReleoadGlobal_NEW_DELETE 
{
	struct MyStruct
	{
		char buffer[1024]{};
	};

	void Test() {
		auto obj = std::make_unique<MyStruct>();

	}
}


//********* Tracker: ********//
namespace Memory::Tracker {

	class TrackNew {
	private:
		static inline int numMalloc  = 0; // num malloc calls
		static inline size_t sumSize = 0; // bytes allocated so far
		static inline bool doTrace   = false; // tracing enabled
		static inline bool inNew     = false; // don�t track output inside new overloads

	public:
		// reset new/memory counters
		static void reset() { 
			numMalloc = 0;
			sumSize = 0;
		}

		// enable/disable tracing
		static void trace(bool b) {
			doTrace = b;
		}

		// implementation of tracked allocation:
		static void* allocate(std::size_t size, std::size_t align, const char* call) {
			++numMalloc;
			sumSize += size;
			void* p;
			if (align == 0) {
				p = std::malloc(size);
			}
			else {
// #ifdef _MSC_VER
#if 0
				p = _aligned_malloc(size, align); // Windows API
#else
				p = std::aligned_alloc(align, size); // C++17 API
#endif
			}
			if (doTrace) {
				// DON�T use std::cout here because it might allocate memory
				// while we are allocating memory (core dump at best)
				printf("#%d %s ", numMalloc, call);
				printf("(%zu bytes, ", size);
				if (align > 0) {
					printf("%zu-bytes aligned) ", align);
				}
				else {
					printf("def-aligned) ");
				}
				printf("=> %p (total: %zu Bytes)\n", (void*)p, sumSize);
			}
			return p;
		}

		// print current state
		static void status() { 
			printf("%d allocations for %zu bytes\n", numMalloc, sumSize);
		}

		[[nodiscard]]
		void* operator new (std::size_t size) {
			return TrackNew::allocate(size, 0, "::new");
		}

		[[nodiscard]]
		void* operator new (std::size_t size, std::align_val_t align) {
			return TrackNew::allocate(size, static_cast<size_t>(align), "::new aligned");
		}

		[[nodiscard]]
		void* operator new[](std::size_t size) {
			return TrackNew::allocate(size, 0, "::new[]");
		}

		[[nodiscard]]
		void* operator new[](std::size_t size, std::align_val_t align) {
			return TrackNew::allocate(size, static_cast<size_t>(align), "::new[] aligned");
		}

		// ensure deallocations match:
		void operator delete (void* p) noexcept {
			std::free(p);
		}

		void operator delete (void* p, std::size_t) noexcept {
			::operator delete(p);
		}

		void operator delete (void* p, std::align_val_t) noexcept {
#ifdef _MSC_VER
			_aligned_free(p); // Windows API
#else
			std::free(p); // C++17 API
#endif
		}

		void operator delete (void* p, std::size_t, std::align_val_t align) noexcept {
			::operator delete(p, align);
		}
	};

	//-----------------------------------------------------------------------------//

	void Test() {
		TrackNew::reset();
		TrackNew::trace(true);

		std::string s = "string value with 26 chars";
		auto p1 = new std::string{ "an initial value with even 35 chars" };
		//auto p2 = new(std::align_val_t{ 64 }) std::string[4];
		auto p3 = new std::string[4]{ "7 chars", "x", "or 11 chars", "a string value with 28 chars" };

		TrackNew::status();
		
		delete p1;
		//delete[] p2;
		delete[] p3;
	}
}

//********* PolymorphicMemoryResources: ********//
namespace Memory::PolymorphicMemoryResources {

	class Object {
	public:
		int value;

	public:
		Object(int v = 0) : value(v) {
			std::cout << "Object::Object(" << value << ") constructed at [" << this << "]" << std::endl;
		}
		~Object() {
			std::cout << "~Object::Object(" << value << ")" << std::endl;
		}

		friend std::ostream& operator<<(const std::ostream& stream, const Object& obj);
	};

	std::ostream& operator<<(std::ostream& stream, const Object& obj) {
		stream << obj.value;
		return stream;
	}

	//---------------------------------------------------------------------------------//

	void NotAllocatingMemoryContainers() {

		Tracker::TrackNew::reset();

		// allocate some memory on the stack:
		std::array<std::byte, 200000> buffer;

		// and use it as initial memory pool for a vector:
		std::pmr::monotonic_buffer_resource pool{ buffer.data(), buffer.size() };
		std::pmr::vector<std::string> coll { &pool };
		for (int i = 0; i < 1000; ++i) {
			coll.emplace_back("just a non-SSO string");
		}

		/*
		for (const std::string& str : coll) {
			std::cout << str << std::endl;
		}
		*/

		std::cout << "Done." << std::endl;
		Tracker::TrackNew::status();
	}

	void NotAllocatingMemoryAtAll() {

		// allocate some memory on the stack:
		std::array<std::byte, 200000> buffer;

		// and use it as initial memory pool for a vector:
		std::pmr::monotonic_buffer_resource pool{ buffer.data(), buffer.size() };
		std::pmr::vector<std::pmr::string> coll{ &pool };
		for (int i = 0; i < 1000; ++i) {
			coll.emplace_back("just a non-SSO string");
		}

		for (const std::pmr::string& str : coll) {
			std::cout << str << std::endl;
		}

		std::cout << "Done." << std::endl;
	}

	void ReusingMemoryPools() {
		// allocate some memory on the stack:
		std::array<std::byte, 200000> buf;

		for (int num : {1000, 2000, 500, 2000, 3000, 50000, 1000}) {
			std::cout << "-- check with " << num << " elements:\n";
			std::pmr::monotonic_buffer_resource pool{ buf.data(), buf.size() };
			std::pmr::vector<std::pmr::string> coll{ &pool };
			for (int i = 0; i < num; ++i) {
				coll.emplace_back("just a non-SSO string");
			}
		}
	}

	void _TESTS_()
    {
		// std::pmr::unordered_map<std::pmr::string, std::pmr::string> dict;
		// std::pmr::

		// const std::string s[] = { "string", "1", "test", "..." };
		// const auto p = std::get_temporary_buffer<std::string>(4);
		// requires that p.first is passed to return_temporary_buffer
		// (beware of early exit points and exceptions)
	}
}

//********* Alligned_New_Delete: ********//
namespace Memory::Alligned_New_Delete {

	struct alignas(32) MyType32 {
		int i;
		char c;
		std::string s[4];
	};

	void Simple_OverAlligned_New() {
		MyType32 *ptr = new MyType32();
	}

	void Custom_Alligned_New() {
		//std::string* p1 = new(std::align_val_t{ 64 }) std::string; // 64-bytes aligned

		//::operator delete(p1, std::align_val_t{ 64 });

		//MyType32* p2 = new(std::align_val_t{ 64 }) MyType32{};
	}
}

//*************************** Alligned_New_Delete: ***************************//

namespace Memory::Address {

	class unreferenceable {
	public:
		int value;

		// Comment this!!
#if 1
		unreferenceable* operator&() {
			return nullptr;
		}
#endif
	};

	void print(unreferenceable* m) {
		if (m)
			std::cout << m->value << std::endl;
		else 
			std::cout << "[null pointer]" << std::endl;
	}

	void Address_Of() {
		unreferenceable val { 10 };
		unreferenceable* foo = &val;
		unreferenceable* bar = std::addressof(val);

		/*
		void(*pfn)(unreferenceable*) = std::addressof(print);
		(*pfn)(foo);   // prints [null pointer]
		(*pfn)(bar);   // prints 10
		*/

		print(foo);   // prints [null pointer]
		print(bar);   // prints 10
	}
}

//*************************** Objects copy: ***************************//

namespace Memory::ObjectsCopy {

	class BadLong : public Utilities::Long {
	protected:
		Utilities::Long* somePtr = nullptr;

	public:
		explicit BadLong(long val = 0, Utilities::Long* ptr = nullptr) : Utilities::Long(val) {
			this->somePtr = ptr;
		}

		virtual ~BadLong() {
			std::cout << "~BadLong()" << std::endl;
			delete somePtr;
		}
	};

	void Bad_Copy() {
		BadLong* array = new BadLong[1];

		Utilities::Long* ptr = new  Utilities::Long(222);
		BadLong l(111, ptr);

		memcpy(array, &l, sizeof(BadLong));

		delete[] array;
	}
}

//*************************** Objects copy: ***************************//

namespace Memory::OffSet {

	struct foo {
		char a;
		char b[10];
		char c;
	};

	struct foo2 {
		int a;
		int b;
		char c;
		char d;
	};

	void Class_Params_OFFSET_OF() {
		std::cout << "offsetof(struct foo, a) = " << (int)offsetof(struct foo, a) << std::endl;
		std::cout << "offsetof(struct foo, b) = " << (int)offsetof(struct foo, b) << std::endl;
		std::cout << "offsetof(struct foo, c) = " << (int)offsetof(struct foo, c) << std::endl;


		std::cout << "\noffsetof(struct foo2, a) = " << (int)offsetof(struct foo2, a) << std::endl;
		std::cout << "offsetof(struct foo2, b) = " << (int)offsetof(struct foo2, b) << std::endl;
		std::cout << "offsetof(struct foo2, c) = " << (int)offsetof(struct foo2, c) << std::endl;
		std::cout << "offsetof(struct foo2, d) = " << (int)offsetof(struct foo2, d) << std::endl;
	}
}

namespace Memory::Construct {


    template<size_t Max>
    struct BadObject {
        static inline int counter {0};

        BadObject() {
            if (++counter >= Max)
                throw std::runtime_error("Failed to construct BadObject");
            std::cout << "BadObject(counter = " << counter <<")\n";
        }

        ~BadObject() {
            std::cout << "~BadObject(counter = " << --counter <<")\n";
        }
    };

    void ConstructN_Uninitialized_HandleException() {
        constexpr size_t size {5}, maxCapacity {3};
        using TestType = BadObject<maxCapacity>;

        alignas(alignof(TestType))
        char buffer[sizeof(TestType) * size];


        auto *begin = reinterpret_cast<TestType*>(buffer);

        try {
            std::uninitialized_default_construct_n(begin, size);
            std::destroy_n(begin, size); // not reached
        } catch (std::exception& e) {
            std::cout << e.what() << "\n";
        }
    }
}

namespace Memory::RestrictObjectHeapCreation
{

    template <typename... Ts>
    struct LocalObject* createObject(Ts&&... params);

    struct LocalObject
    {
        int value { 0 };
        std::string name;

        LocalObject(int v, std::string s): value {v}, name { std::move(s)} {
            std::cout << "LocalObject(" << value << ", " << std::quoted(name) << ")\n";
        }

        ~LocalObject() {
            std::cout << "~LocalObject(" << value << ", " << std::quoted(name) << ")\n";
        }

    private:

        void* operator new(size_t size)
        {
            std::cout << "LocalObject created on Heap. Size = " << size << std::endl;
            decltype(auto) memBlock = operator new[](size);
            // decltype(auto) memBlock1 = malloc(size);
            return memBlock;
        }

        template <typename... Ts>
        friend LocalObject* createObject(Ts&&... params);
    };

    template <typename... Ts>
    struct LocalObject* createObject(Ts&&... params)
    {
        return new LocalObject(std::forward<Ts>(params)...);
    }

    void CreateObjects_PrivateFunc()
    {
        /** Will not compile **/
        // LocalObject* obj = new LocalObject(10, "sdsds");

        LocalObject* obj = createObject(12, "dsdsdsd");

        delete obj;
    }
}

namespace VectorOfUniquePointers_KeepReference_StoredInVector
{
    struct Client
    {
        int value {0};

        explicit Client(int v) : value {v} { std::cout << "Client(" << value << ") created\n"; }
        ~Client() { std::cout << "Client(" << value << ") destroyed\n"; }
        void close() { std::cout << "Client(" << value << ") closed\n"; }

        void info() {
            std::cout << "info Client(" << value << ")\n";
        }
    };

    template<typename T>
    struct Closer
    {
        void operator()(T *ptr) const {
            ptr->close();
            delete ptr;
        }
    };

    using ClientDeleter = Closer<Client>;


    // The idea of the test is to save a pointer in the LAMBDA function to an object in
    // the form of a unique_ptr, which will then be moved to VECTOR
    void Test()
    {
        std::vector<std::unique_ptr<Client, ClientDeleter>> clients;

        std::function<void(void)> callback;

        {
            auto client = std::unique_ptr<Client, ClientDeleter>(new Client{123}, ClientDeleter{});
            callback = [client_ref = client.get()] {
                client_ref->info();
            };

            clients.push_back(std::move(client));
        }

        std::cout << std::string(100, '=') << std::endl;

        callback();
        callback();

        clients.back()->value = 100500;

        callback();

        std::cout << std::string(100, '=') << std::endl;
    }

}


namespace Memory::Override_Global_Memory_Handler
{
	void handler()
	{
		std::cout << "Memory allocation failed, terminating\n";
		std::set_new_handler(nullptr);
	}

	void test()
	{
		std::set_new_handler(handler);
		try
		{
			while (true)
			{
				new int [1000'000'000ul] ();
			}
		}
		catch (const std::bad_alloc& e)
		{
			std::cout << e.what() << '\n';
		}
	}
}

void Memory::TestAll()
{
    // Launder::TestAll();
    Alignment::TestAll();
    // AlignedStackAllocator::TestAll();
    // CustomStackAllocator::TestAll();
    // MemoryUsageMonitor::TestAll();
    // MemoryPool::TestAll();

	// ObjectPool_Stack_FixedSize::TestAll();


	// Delete_Array();
	// NoThrowTest();

	// ObjectPool::Test1();

	// ObjectsCopy::Bad_Copy();

	// New_Placement::Test();
	// New_Placement::Test2();
	// New_Placement::Test3();
	// New_Placement::Good_Example();


	// AlignedStorage::Construct_Type();

	// Address::Address_Of();

	// Pointers::Memcpy_Test1();
	// Pointers::Pointer_to_array();
	// Pointers::Pointer_to_array_TTEEESSSTT();
	// Pointers::Arrays_Of_Pointers();
	// Pointers::Arrays_Of_Pointers2();

	// Pointers::Pointer_to_Arrays_NewPlacement();
	// Pointers::Pointer_to_Arrays_ConstructAt();

    // Construct::ConstructN_Uninitialized_HandleException();

	// Destroy::Call_Destructor_Manualy();
	// Destroy::Destroy_Simple();
	// Destroy::Destroy_Test1();
	// Destroy::Destroy_Test2();
	// Destroy::Destroy_N_Test();
	// Destroy::Destroy_AT_Simple();
	// Destroy::Destroy_At_PlacmentNew();

	// CustomAllocator::Test();
	// CustomAllocator::Test_Integer();

	//--------------------------------------------------------------------------//

	// OperatorNew::Classic_Allocation();
	// OperatorNew::Allocate_NoThrow();
	// OperatorNew::Allocate_Placement();
	// OperatorNew::Allocate_Placement_DestroyAt();
	// OperatorNew::Allocate_Placement2();
	// OperatorNew::CreateObject_NoConstructor();
	// OperatorNew::Construct_AT();

	// Reload_New_and_Delete::TestOverloadedNew();
	// Reload_New_and_Delete::Disable_New__UseOnly_MakeUnique();

    // Reload_New_and_Delete::Delete_Object_Size(); /** calling delete(size) depending of the Size of the object **/

	// OffSet::Class_Params_OFFSET_OF();

	// ReleoadGlobal_NEW_DELETE::Test();

    // RestrictObjectHeapCreation::CreateObjects_PrivateFunc();

	//------------------------------------------------------------------------//

	// Allocators::Basic_STD_Allocator();
	// Allocators::SimpleTest();
	// Allocators::StackAllocatorTests();
	// Allocators::AllocateShared_And_Trace();

	// Allocators_Vector::Emplace_CustomType_1();
	// Allocators_Vector::Emplace_CustomType_2();
	// Allocators_Vector::Emplace_CustomType_Allocator();

	//------------------------------------------------------------------------//

    // VectorOfUniquePointers_KeepReference_StoredInVector::Test();

    //------------------------------------------------------------------------//

    // Tracker::Test();

	// PolymorphicMemoryResources::NotAllocatingMemoryContainers();
	// PolymorphicMemoryResources::NotAllocatingMemoryAtAll();
	// PolymorphicMemoryResources::ReusingMemoryPools();
	// PolymorphicMemoryResources::_TESTS_();

	// Alligned_New_Delete::Simple_OverAlligned_New();
	// Alligned_New_Delete::Custom_Alligned_New();

};