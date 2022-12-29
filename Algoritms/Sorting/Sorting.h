//============================================================================
// Name        : Sorting.h
// Created on  : 08.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Sorting algorithms C++
//============================================================================

#ifndef SORTING_ALGORITHMS_TESTS_INCLUDE_GUARD__H
#define SORTING_ALGORITHMS_TESTS_INCLUDE_GUARD__H

namespace Sorting {

	template<typename T>
	void print_array(const T* data,
		const unsigned int size,
		std::string_view text = "");


	int* MakeIntArray(size_t size, size_t range_start = 0, size_t range_end = 10000);

	void TEST_ALL();
};

#endif // !SORTING_ALGORITHMS_TESTS_INCLUDE_GUARD__H