//============================================================================
// Name        : Circular_Buffer.cpp
// Created on  : 17.01.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Circular_Buffer src class
//============================================================================

#include "CircularBuffers.h"

// #include <concepts>
#include <iostream>
#include <mutex>
#include <memory>
#include <optional>
#include <vector>
#include <cassert>

namespace CircularBuffers::Base
{
	template <class T>
	class circular_buffer {
	private:
		std::mutex mtx;
		std::unique_ptr<T[]> buffer;
		size_t head{ 0 };
		size_t tail{ 0 };
		const size_t max_size;
		bool full{ false };

	public:
		explicit circular_buffer(size_t size) :
			buffer(std::unique_ptr<T[]>(new T[size])),
			max_size(size) {
			// empty
		}

		void put(T item) noexcept;
		T get() noexcept;
		void reset() noexcept;

		bool empty() const noexcept;
		bool isFull() const noexcept;

		size_t capacity() const noexcept;
		size_t size() const noexcept;
	};


	template <class T>
	inline bool circular_buffer<T>::isFull() const noexcept {
		/// If tail is ahead the head by 1, we are full
		return full;
	}

	template <class T>
	inline bool circular_buffer<T>::empty() const noexcept {
		/// if head and tail are equal, we are empty
		return (!full && (head == tail));
	}

	template <class T>
	void circular_buffer<T>::put(T item) noexcept {
		std::lock_guard<std::mutex> lock(mtx);
		buffer[head++] = item;

		if (full) {
			tail = (tail + 1) % max_size;
		}

		head = head % max_size;
		full = (head == tail);
	}

	// TODO: Return T as std::optional ???
	template <class T>
	T circular_buffer<T>::get() noexcept {
		std::lock_guard<std::mutex> lock(mtx);

		if (true == empty()) {
			return T();
		}

		// Read data and advance the tail (we now have a free space)
		T val = buffer[tail];
		full = false;
		tail = (tail + 1) % max_size;

		return val;
	}

	template <class T>
	void circular_buffer<T>::reset() noexcept {
		std::lock_guard<std::mutex> lock(mtx);
		head = tail;
		full = false;
	}

	template <class T>
	size_t circular_buffer<T>::capacity() const noexcept {
		// Max size is the capacity of the collection.
		return max_size;
	}

	template <class T>
	size_t circular_buffer<T>::size() const noexcept {
		size_t size{ max_size };

		if (false == full) {
			if (head >= tail)
				size = head - tail;
			else
				size = max_size + head - tail;
		}
		return size;
	}

	//----------------------------------------------------------------------------//


	void Test1() {
		circular_buffer<int> cb(10);

		cb.reset();

		for (int i = 1; i < 15; i++) {
			cb.put(i);
			std::cout << "add " << i << ". size = " << cb.size() << "\n";
		}



		auto x = cb.get();
		std::cout << x << std::endl;

		x = cb.get();
		std::cout << x << std::endl;

		std::cout << "Size = " << cb.size() << "\n";
	}
}


namespace CircularBuffers::Demo1
{
	// template <class T>
	// concept NotVoidType = !std::is_void_v<T>;

	// template <class T, size_t Capacity> class RingBuffer {};

	template <class T, size_t _Size>
	class RingBuffer {
	private:
		using value_type = T;
		using size_type = size_t;

		static_assert(!std::is_same_v<T, void>, "Type of the RingBuffer can not be void");
		static_assert(0 != _Size, "Please try a little bigger buffer");

	private:
		//std::mutex mtx;
		value_type ring_buffer[_Size]{};

		size_type head{ 0 };
		size_type tail{ 0 };

		bool full{ false };

	public:

		void put(value_type item) noexcept {
			//std::lock_guard<std::mutex> lock(mtx);
			ring_buffer[head++] = item;

			// If we buffer already full we have to advance the  'tail' position
			if (full) {
				tail = (tail + 1) % _Size;
			}

			head = head % _Size;
			full = (head == tail);
		}

		void reset() noexcept {
			//std::lock_guard<std::mutex> lock(mtx);
			head = tail;
			full = false;
		}

		std::optional<value_type> get() noexcept {
			//std::lock_guard<std::mutex> lock(mtx);

			if (true == empty())
				return std::nullopt;

			// Read data and advance the tail (we now have a free space)
			std::optional<value_type> value =
				std::make_optional<value_type>(ring_buffer[tail]);

			full = false;
			tail = (tail + 1) % _Size;

			return value;
		}

		inline bool empty() const noexcept {
			// if head and tail are equal, we are empty
			return (!full && (head == tail));
		}

		inline bool isFull() const noexcept {
			return full;
		}

		inline size_type capacity() const noexcept {
			// Max size is the capacity of the collection.
			return _Size;
		}

		size_type size() const noexcept {
			size_type size{ _Size };

			if (false == full) {
				if (head >= tail)
					size = head - tail;
				else
					size = _Size + head - tail;
			}
			return size;
		}
	};



	void Test1() {
		RingBuffer<int, 5> ringBuffer;

		/*
		ringBuffer.reset();

		for (int i = 1; i < 15; i++) {
			ringBuffer.put(i);
			std::cout << "add " << i << ". size = " << ringBuffer.size() << "\n";
		}
		*/

		/*
		auto x = ringBuffer.get();
		std::cout << x << std::endl;

		x = ringBuffer.get();
		std::cout << x << std::endl;

		std::cout << "Size = " << ringBuffer.size() << "\n";
		*/
	}
}


namespace CircularBuffers::SimpleRingArray {

	template<typename T = int, size_t _Size = 10>
	class RingArray {
	private:
		using value_type = T;
		using size_type = size_t;

		static_assert(!std::is_same_v<T, void>, "Type of the RingBuffer can not be void");
		static_assert(0 != _Size, "Please try a little bigger buffer");

	private:
		value_type buffer[_Size]{};
		size_t head{ 0 };
		// size_t tail {0};

	public:
		inline constexpr size_type size() const {
			return _Size;
		}

		void put(value_type item) {
			head = head % _Size;
			buffer[head++] = item;
		}

		void printContent() const {
			for (const auto& entry : buffer)
				std::cout << entry << " ";
			std::cout << "\n";
		}
	};

	void Test() {
		RingArray<int, 5> ringArray;

		ringArray.printContent();

		for (int i = 1; i <= 5; ++i)
			ringArray.put(i);

		ringArray.printContent();

		for (int i = 6; i <= 10; ++i)
			ringArray.put(i);

		ringArray.printContent();

		for (int i = 11; i <= 22; ++i)
			ringArray.put(i);

		ringArray.printContent();
	}
}

namespace CircularBuffers::PerformanceTests {

	constexpr size_t _Size = 1000;
	constexpr size_t _Count = 100000;

	void TestVector() {
		std::vector<int> numbers;
		numbers.reserve(_Size);

		auto start = std::chrono::high_resolution_clock::now();

		for (size_t n = 0; n < _Count; ++n) {
			for (size_t i = 0; i < _Size; ++i) {
				numbers.push_back(static_cast<int>(i));
			}
			for (size_t i = 0; i < _Size; ++i) {
				numbers.erase(numbers.begin());
			}
			//assert(true == numbers.empty());
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
			std::cout << "Result: " << duration << " microseconds" << std::endl;
	}

	void TestRingBuffer() {
		Demo1::RingBuffer<int, _Size> ringBuffer;

		auto start = std::chrono::high_resolution_clock::now();

		for (size_t n = 0; n < _Count; ++n) {
			for (size_t i = 0; i < _Size; ++i) {
				ringBuffer.put(static_cast<int>(i));
			}
			for (size_t i = 0; i < _Size; ++i) {
				ringBuffer.get();
			}
			//assert(true == numbers.empty());
		}
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
			std::cout << "Result: " << duration << " microseconds" << std::endl;
	}
}


void CircularBuffers::TEST_ALL()
{
	// Base::Test1();

	// Demo1::Test1();

	// PerformanceTests::TestVector();
	// PerformanceTests::TestRingBuffer();

	SimpleRingArray::Test();
}
