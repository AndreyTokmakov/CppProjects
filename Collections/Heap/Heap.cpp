//============================================================================
// Name        : Heap.h
// Created on  : 26.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Heap testing
//============================================================================

#include "Heap.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <string_view>

namespace Heap {

	template<typename Type>
	void print_collection(const Type& collection) {
		for (const auto& entry : collection)
			std::cout << entry << ' ';
		std::cout << std::endl;
	}

	template<typename Type>
	void print_collection_ex(std::string_view text, const Type& collection) {
		std::cout << text;
		print_collection(collection);
	}

	/////////////////////////////////////////////////////////////////////////

	void IsHeap()
	{
		std::vector<int> numbers = { 3, 2, 4, 1, 5, 9 };
		print_collection_ex("Before: ", numbers);

		std::cout << "Is Heap:" << std::boolalpha << std::is_heap(numbers.begin(), numbers.end()) << std::endl;

		std::make_heap(numbers.begin(), numbers.end());
		print_collection_ex("after make_heap: ", numbers);
	}


	void Sort_Heap() {
		std::vector<int> numbers = { 3, 2, 4, 1, 5, 9 };
		std::make_heap(numbers.begin(), numbers.end());
		print_collection_ex("Before: ", numbers);

		std::sort_heap(numbers.begin(), numbers.end());
		//std::pop_heap(numbers.begin(), numbers.end());
		print_collection_ex("Sorted : ", numbers);
	}

	void Pop_Heap() {
		std::vector<int> numbers = { 3, 2, 4, 1, 5, 9 };
		std::make_heap(numbers.begin(), numbers.end());
		print_collection_ex("Before: ", numbers);

		std::pop_heap(numbers.begin(), numbers.end());
		print_collection_ex("After : ", numbers);
	}

	void Push_Heap() {
		std::vector<int> numbers = { 3, 2, 4, 1, 5, 9 };
		std::make_heap(numbers.begin(), numbers.end());
		print_collection_ex("Before 1: ", numbers);

		numbers.push_back(12);
		print_collection_ex("Before 2: ", numbers);


		std::push_heap(numbers.begin(), numbers.end());
		print_collection_ex("After   : ", numbers);
	}

	void MakeHeap()
	{
		std::cout << "Max heap:\n";
		std::vector<int> numbers = { 3, 2, 4, 1, 5, 9 };
		print_collection_ex("initially: ", numbers);

		std::make_heap(numbers.begin(), numbers.end());
		print_collection_ex("after make_heap: ", numbers);

		std::pop_heap(numbers.begin(), numbers.end());
		print_collection_ex("after pop_heap: ", numbers);

		auto top = numbers.back();
		numbers.pop_back();
		std::cout << "former top element: " << top << '\n';

		print_collection_ex("after removing the former top element: ", numbers);
	}

	void MakeHeap_Min()
	{
		std::vector<int> numbers{ 3, 2, 4, 1, 5, 9 };
		print_collection_ex("Before: ", numbers);

		std::make_heap(numbers.begin(), numbers.end(), std::greater<>{});
		print_collection_ex("After: ", numbers);

		std::pop_heap(numbers.begin(), numbers.end(), std::greater<>{});
		numbers.pop_back();
		print_collection_ex("After pop_heap: ", numbers);
	}

	void MakeHeap_Min_Lambda()
	{
		std::vector<int> v1{ 3, 2, 4, 1, 5, 9 };
		print_collection_ex("Before 1: ", v1);

		std::make_heap(v1.begin(), v1.end());
		print_collection_ex("Before 1: ", v1);
	}

	void Tests() {
        std::vector<int> numbers = {10, 8, 5, 3, 2};
        std::make_heap(numbers.begin(), numbers.end());

        for (const auto i: numbers)
            std::cout << i << " ";
        std::cout << std::endl;


        numbers.push_back(1);
        numbers.push_back(7);

        std::make_heap(numbers.begin(), numbers.end());

        for (const auto i: numbers)
            std::cout << i << " ";
        std::cout << std::endl;
    }

}

namespace Heap {
	void TEST_ALL() {

		// Sort_Heap();
		// IsHeap();

		// MakeHeap();
		// MakeHeap_Min();
		// MakeHeap_Min_Lambda();

		// Push_Heap();
		// Pop_Heap();

		 Tests();
	}
};
