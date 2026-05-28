//============================================================================
// Name        : SortingAlgorithms.cpp
// Created on  : 08.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : SortingAlgorithms src class
//============================================================================

#include <iostream>
#include <vector>
#include <cstring>
#include <array>
#include <iterator>
#include <string_view>
#include <chrono>
#include <thread>
#include <random>

#include "Sorting.h"
#include "PerfUtilities.hpp"


namespace Sorting::HeapSort {

	// MaxHeap heapify
	void heapify_max(int* data, size_t lenght, size_t index) {
		size_t left, right, current;
		while (true) {
			left = 2 * index + 1;
			right = 2 * index + 2;
			current = index;

			if (lenght > left && data[current] < data[left])
				current = left;
			if (lenght > right && data[current] < data[right])
				current = right;
			if (current == index)
				break;
			std::swap(data[index], data[current]);
			index = current;
		}
	}

	void heapSort(int arr[], int length) {
		// Build heap (rearrange array) 
		for (int i = (length / 2 - 1); i >= 0; i--)
			heapify_max(arr, length, i);

		// One by one extract an element from heap 
		for (int i = length - 1; i > 0; i--) {
			// Move current root to end 
			std::swap(arr[0], arr[i]);
			// call max heapify on the reduced heap 
			heapify_max(arr, i, 0);
		}
	}

	void SortTest() {
		unsigned int size = 1000;
		int* data = MakeIntArray(size, 0, (size * 2));

		//print_array(data, size, "Before: ");
		std::cout << std::boolalpha << std::is_sorted(data, data + size) << std::endl;

		heapSort(data, size);

		//print_array(data, size, "After: ");
		std::cout << std::boolalpha << std::is_sorted(data, data + size) << std::endl;
	}
}


namespace Sorting {

	auto start = std::chrono::high_resolution_clock::now();
	auto end = std::chrono::high_resolution_clock::now();
	auto durtion = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

	void FillArray(int* input,
					size_t size,
					size_t range_start = 0,
					size_t range_end = 10000) {
		std::random_device random_device;
		std::mt19937 generator(random_device());
		std::uniform_int_distribution<> distribution(range_start, range_end);
		for (size_t i = 0; i < size; i++) {
			int x = distribution(generator);
			input[i] = x;
		}
	}

	int* MakeIntArray(size_t size, size_t range_start, size_t range_end) {
		int* data = new int[size];
		FillArray(data, size, range_start, range_end);
		return  data;
	}

	auto printer_coma = [](const auto& val) {
		std::cout << val << ", ";
	};

	auto print_collection = [](const auto& array) {
		for (const auto& enrty : array)
			printer_coma(enrty);
	};

	template<typename T>
	void print_array(const T* data,
					 unsigned int size,
				     std::string_view text) {
		if (false == text.empty()) {
			std::cout << text << "";
		}
		for (unsigned int pos = 0; pos < (size - 1); pos++)
			std::cout << data[pos] << ", ";
		std::cout << data[size - 1] << std::endl;
	};

	auto print_collection_ex = [](const auto& array,
		std::string_view text,
		std::string_view postfix = "\n") {
			std::cout << text;
			print_collection(array);
			std::cout << postfix << std::endl;
	};

	//////////////////////////////////////////////////

	void BubbleSort(int* data, unsigned int size) {
		for (unsigned int i = 0; i < size - 1; i++) {
			for (unsigned int n = 0; n < size - i - 1; n++) {
				if (data[n] > data[n + 1])
					std::swap(data[n], data[n + 1]);
			}
		}
	}

	void SelectionSort(int* data, unsigned int size) {
		for (unsigned int min_element_index = 0, i = 0; i < size; i++) {
			min_element_index = i;
			for (unsigned int n = i; n < size; n++) {
				if (data[min_element_index] > data[n])
					min_element_index = n;
			}
			std::swap(data[i], data[min_element_index]);
		}

	}

	void InsertionSort(int* data, unsigned int size) {
		for (unsigned int i = 1; i < size; i++) {
			for (int j = i; j > 0 && data[j - 1] > data[j]; j--) {
				std::swap(data[j - 1], data[j]);
			}
		}
	}

	void ShakerSort(int* data, unsigned int size) {
		unsigned int left = 0, right = size - 1;
		while (left <= right) {
			for (unsigned int i = right; i > left; --i) {
				if (data[i - 1] > data[i])
					std::swap(data[i - 1], data[i]);
			}
			++left;
			for (unsigned int i = left; i < right; ++i) {
				if (data[i] > data[i + 1])
					std::swap(data[i], data[i + 1]);
			}
			--right;
		}
	}

	///////////////////////////////////////////////////

	void BubbleSort_Updated(int* data, unsigned int size) {
		bool changed{ true };
		for (unsigned int i = 0; i < size - 1 && changed; i++) {
			changed = false;
			for (unsigned int n = 0; n < size - i - 1; n++) {
				if (data[n] > data[n + 1]) {
					std::swap(data[n], data[n + 1]);
					changed = true;
				}
			}
		}
	}

	void BubbleSort_Updated_2(int* data, int size) {
		bool run = true;
		int i = 1;
		while (true == run) {
			run = false;
			for (int pos = 0; pos < size - i; pos++) {
				if (data[pos] > data[pos + 1]) {
					std::swap(data[pos + 1], data[pos]);
					run = true;
				}
			}
			i++;
		}
	}

	void BubbleSortTest()
	{
		const unsigned short size = 10;
		int* data = MakeIntArray(size, 0, size * 5);

		std::cout << "Sorted: " << std::boolalpha << std::is_sorted(data, data + size) << std::endl;
		print_array(data, size, "Before: ");

		BubbleSort(data, size);
		// BubbleSort2(data, size);

		std::cout << "\nSorted: " << std::boolalpha << std::is_sorted(data, data + size) << std::endl;
		print_array(data, size, "After: ");
	}

	void BubbleSort_Updated_Test()
	{
		const unsigned short size = 10;
		int* data = MakeIntArray(size, 0, size * 5);

		std::cout << "Sorted: " << std::boolalpha << std::is_sorted(data, data + size) << std::endl;
		print_array(data, size, "Before: ");

		BubbleSort_Updated(data, size);

		std::cout << "\nSorted: " << std::boolalpha << std::is_sorted(data, data + size) << std::endl;
		print_array(data, size, "After: ");
	}

	void SelectionSort()
	{
		std::array<int, 9> int_array = { 8, 6, 4, 1, 12, 34, 43, 11, 8 };
		print_collection_ex(int_array, "Before: ");

		for (size_t i = 0; i < int_array.size(); i++) {
			size_t min_element_index = i;
			for (unsigned int n = i; n < int_array.size(); n++) {
				if (int_array[min_element_index] > int_array[n])
					min_element_index = n;
			}
			std::swap(int_array[i], int_array[min_element_index]);
		}
		print_collection_ex(int_array, "After: ");
	}

	void ShakerSort()
	{
		std::vector<int> values = { 8, 6, 4, 1, 12, 34, 43, 11, 8 };
		print_collection_ex(values, "Before: ");
		int left = 0;
		int right = static_cast<int>(values.size() - 1);
		while (left <= right) {
			for (int i = right; i > left; --i) {
				if (values[i - 1] > values[i]) {
					std::swap(values[i - 1], values[i]);
				}
			}
			++left;
			for (int i = left; i < right; ++i) {
				if (values[i] > values[i + 1]) {
					std::swap(values[i], values[i + 1]);
				}
			}
			--right;
		}
		print_collection_ex(values, "After: ");
	}

	void QuickSort(int* data, const unsigned int length)
	{
		unsigned int rand_element_pos = length / 2;
		unsigned int j = 0, k = 0;
		if (length > 1) {
			int* left = new int[length];
			int* right = new int[length];
			int rand_element = data[rand_element_pos];
			for (unsigned int iter = 0; iter < length; iter++) {
				if (iter != rand_element_pos) {
					if (data[iter] < rand_element)
						left[j++] = data[iter];
					else
						right[k++] = data[iter];
				}
			}
			QuickSort(left, j);
			QuickSort(right, k);
			for (unsigned int iter = 0; iter < length; iter++) {
				if (iter < j)
					data[iter] = left[iter];
				else if (iter == j)
					data[iter] = rand_element;
				else
					data[iter] = right[iter - (j + 1)];
			}
		}
	}

	void qs(int* data,
		const int first,
		const int last)
	{
		int left = first, right = last;
		int x_element = data[(first + last) / 2];
		do {
			while (data[left] < x_element)
				left++;
			while (data[right] > x_element)
				right--;
			if (left <= right) {
				if (data[left] > data[right])
					std::swap(data[left], data[right]);
				left++;
				right--;
			}
		} while (left <= right);
		if (left < last)
			qs(data, left, last);
		if (first < right)
			qs(data, first, right);
	}


	void QSort(int* data,
		const int start,
		const int end) {
		int left_pos = start, right_pos = end;
		int x_element_pos = (start + end) / 2;
		int x_element = data[x_element_pos];
		//std::cout << "x_element = " << x_element << std::endl;
		//int count = 0;

		do {
			//std::cout << "START WITH : left_pos = " << left_pos << ", right_pos = " << right_pos << std::endl;
			while (data[left_pos] < x_element) {
				//std::cout << "Left. data[" << left_pos << "] = " << data[left_pos] << std::endl;
				left_pos++;
			}
			while (data[right_pos] > x_element) {
				//std::cout << "Right. data[" << right_pos << "] = " << data[right_pos] << std::endl;
				right_pos--;
			}

			if (left_pos <= right_pos) {
				if (data[left_pos] > data[right_pos])
					std::swap(data[left_pos], data[right_pos]);
				left_pos++;
				right_pos--;
			}
		} while (left_pos <= right_pos);
		if (left_pos < end)
			QSort(data, left_pos, end);
		if (start < right_pos)
			QSort(data, start, right_pos);
	}

	int tmp = 0;
	void _swap(int& a, int& b) {
		tmp = a;
		a = b;
		a = tmp;
	}

	void QSortEx(int* data,
		const int start,
		const int end) {
		int left_pos = start, right_pos = end;
		int x_element_pos = (start + end) / 2;
		int x_element = data[x_element_pos];

		do {
			while (data[left_pos] < x_element) {
				left_pos++;
			}
			while (data[right_pos] > x_element) {
				right_pos--;
			}
			if (left_pos <= right_pos) {
				if (data[left_pos] > data[right_pos]) {
					_swap(data[left_pos], data[right_pos]);
				}
				left_pos++;
				right_pos--;
			}
		} while (left_pos <= right_pos);
		if (left_pos < end)
			QSort(data, left_pos, end);
		if (start < right_pos)
			QSort(data, start, right_pos);
	}

	int partition(int* a, int start, int end)
	{
		int pivot = a[end];
		//P-index indicates the pivot value index

		int P_index = start;
		int i, t; //t is temporary variable

		//Here we will check if array value is 
		//less than pivot
		//then we will place it at left side
		//by swapping 

		for (i = start; i < end; i++)
		{
			if (a[i] <= pivot)
			{
				t = a[i];
				a[i] = a[P_index];
				a[P_index] = t;
				P_index++;
			}
		}
		//Now exchanging value of
		//pivot and P-index
		t = a[end];
		a[end] = a[P_index];
		a[P_index] = t;

		//at last returning the pivot value index
		return P_index;
	}


	void QuickSortTest()
	{
		unsigned int size = 1000;
		int* data = MakeIntArray(size, 0, (size * 2));

		//print_array(data, size, "Before: ");
		std::cout << std::boolalpha << std::is_sorted(data, data + size) << std::endl;

		QuickSort(data, size);
		// qs(data, 0, size - 1);
		QSort(data, 0, size - 1);

		//print_array(data, size, "After: ");
		std::cout << std::boolalpha << std::is_sorted(data, data + size) << std::endl;
	}

	//-------------------------------------------------------------------------------//

	void Quick_Sort(int* data, int start, int end) {
		int pivot = data[(start + end) / 2];
		int left = start, right = end;

		while (right > left) {
			while (data[left] < pivot)
				left++;
			while (data[right] > pivot)
				right--;

			if (left <= right) {
				if (data[left] > data[right]) {
					std::swap(data[left], data[right]);
				}
				left++;
				right--;
			}
		}
		if (left < end)
			Quick_Sort(data, left, end);
		if (start < right)
			Quick_Sort(data, start, right);
	}

	int partition2(int* data, int start, int end) {
		int left = start, right = end + 1;
		while (true) {
			while (data[++left] < data[start]) {
				if (left == end) break;
			}
			while (data[--right] > data[start]) {
				if (right == start) break;
			}
			if (left >= right)
				break;
			std::swap(data[left], data[right]);
		}
		std::swap(data[start], data[right]);
		return right;
	}

	void Quick_Sort_2(int* data, int start, int end) {
		if (end <= start)
			return;
		int j = partition2(data, start, end);
		Quick_Sort_2(data, start, j - 1);
		Quick_Sort_2(data, j + 1, end);
	}

	void QuickSortTest_2() {

		unsigned int size = 10;
		int* data = MakeIntArray(size, 0, (size * 2));

		print_array(data, size, "Before: ");
		std::cout << std::boolalpha << std::is_sorted(data, data + size) << std::endl;

		// Quick_Sort(data, 0, size - 1);
		Quick_Sort_2(data, 0, size - 1);

		print_array(data, size, "After: ");
		std::cout << std::boolalpha << std::is_sorted(data, data + size) << std::endl;
	}

	//-------------------------------------------------------------------------------//

	/*
	void MergeSortTest()
	{
		std::vector<int> values = { 8, 6, 4, 1, 12, 34, 43, 11, 8 };
		print_array_ex(values, "Before: ");
		MergeSort(values, values.size());
	}

	void merge(std::vector<int> merged, int lenD, int L[], int lenL, int R[], int lenR) {
		int i = 0;
		int j = 0;
		while (i < lenL || j < lenR) {
			if (i < lenL & j < lenR) {
				if (L[i] <= R[j]) {
					merged[i + j] = L[i];
					i++;
				}
				else {
					merged[i + j] = R[j];
					j++;
				}
			}
			else if (i < lenL) {
				merged[i + j] = L[i];
				i++;
			}
			else if (j < lenR) {
				merged[i + j] = R[j];
				j++;
			}
		}
	}

	void MergeSort(std::vector<int> data, int lenD)
	{
		if (lenD > 1) {
			int middle = lenD / 2;
			int rem = lenD - middle;
			int* L = new int[middle];
			int* R = new int[rem];
			for (int i = 0; i < lenD; i++) {
				if (i < middle) {
					L[i] = data[i];
				}
				else {
					R[i - middle] = data[i];
				}
			}
			MergeSort(L, middle);
			MergeSort(R, rem);
			merge(data, lenD, L, middle, R, rem);
		}
	}
	*/

	void qsort(int* data, size_t left, size_t right)
	{
		int element = data[(right + left) / 2];
		size_t start = left, end = right;
		do {
			while (data[start] < element)
				start++;
			while (data[end] > element)
				end--;
			if (end >= start) {
				if (data[start] >= data[end])
					std::swap(data[end], data[start]);
				start++;
				end--;
			}
		} while (end >= start);
	}


	void TESTS()
	{
		size_t size = 1000;
		int* data = MakeIntArray(size, 0, size * 5);

		std::cout << "Sorted: " << std::boolalpha << std::is_sorted(data, data + size) << std::endl;
		print_array(data, size, "Before: ");

		bool changed{ true };
		for (size_t i = 0; i < size && changed; i++) {
			changed = false;
			for (size_t n = 1; n < size - i; n++) {
				if (data[n - 1] > data[n]) {
					std::swap(data[n - 1], data[n]);
					changed = true;
				}
			}
		}



		/// Bubble sort
		/*
		for (int i = 0; i < size; i++) {
			for (int n = 1; n < size - i; n++) {
				if (data[n - 1] > data[n])
					std::swap(data[n - 1], data[n]);
			}
		}
		*/

		/// shaker sort
		/*
		int left = 1, right = size;
		while (right >= left) {
			for (int i = left; i < right; i++) {
				if (data[i - 1] > data[i])
					std::swap(data[i - 1], data[i]);
			}
			right--;
			for (int i = right; i >= left; i--) {
				if (data[i -1] > data[i])
					std::swap(data[i - 1], data[i]);
			}
			left++;
		}
		*/

		// Quick Sort!
		// qsort(data,0 , size - 1);


		/*
		size_t min_el_pos = 0;
		for (size_t i = 0; i < size; i++) {
			min_el_pos = i;
			for (size_t n = i; n < size; n++) {
				if (data[min_el_pos] > data[n])
					min_el_pos = n;
			}
			std::swap(data[min_el_pos], data[i]);
		}
		*/

		/*
		for (size_t i = 0; i < size; i++) {
			for (size_t n = i; n > 0 && data[n - 1] > data[n]; --n)
				std::swap(data[n - 1], data[n]);
		}*/

		//STOP_TIME_MEASURE;
		//PRINT_RESULT;

		std::cout << "\n\nSorted: " << std::boolalpha << std::is_sorted(data, data + size) << std::endl;
		print_array(data, size, "Before: ");
	}

	void Performance_TEST()
	{
		unsigned int size = 100;
		int* values = MakeIntArray(size, 0, (size * 2));

		const PerfUtilities::ScopedTimer timer { "Benchmark" };
		for (int i = 0; i < 1000; i++) {
			for (int n = 0; n < 1000; n++) {
				int* data = new int[100];
				memcpy(data, values, (sizeof(int) * size));
				////////////////////////////////////////////////////////////

				for (size_t i = 0; i < size; i++) {
					for (size_t n = i; n > 0 && data[n - 1] > data[n]; --n)
						std::swap(data[n - 1], data[n]);
				}

				/*
				for (size_t i = 0; i < size; i++) {
					for (size_t n = 1; n < size - i; n++) {
						if (data[n - 1] > data[n])
							std::swap(data[n - 1], data[n]);
					}
				}
				*/

				////////////////////////////////////////////////////////////
				delete data;
			}
		}
	}

	void Performance_Tests()
	{
		unsigned int size = 200;
		int* values = MakeIntArray(size, 0, (size * 2));

		/*
		///////////////////////////////////////////////////
		print_array(values, size);

		// BubbleSort(values, size);
		// BubbleSort2(values, size);
		// BubbleSort_Updated_2(values, size);
		InsertionSort(values, size);

		// ShakerSort(values, size);

		print_array(values, size);
		///////////////////////////////////////////////////
		*/


		{
			const PerfUtilities::ScopedTimer timer { "Benchmark" };
			for (int i = 0; i < 1000; i++) {
				for (int n = 0; n < 1000; n++) {
					int* test_values = new int[size];
					memcpy(test_values, values, (sizeof(int) * size));
					////////////////////////////////////////////////////////////

					// ShakerSort(test_values, size);
					// BubbleSort(test_values, size);
					// BubbleSort2(test_values, size);
					// BubbleSort_Updated_2(test_values, size);
					// InsertionSort(test_values, size);
					// SelectionSort(test_values, size);
					// QuickSort(test_values, size);
					Quick_Sort(test_values, 0, size - 1);
					// qs(test_values, 0, size - 1);
					// QSort(test_values, 0, size - 1);
					// Quicksort(test_values, 0, size - 1);
					// QSortEx(test_values, 0, size - 1);

					////////////////////////////////////////////////////////////
					delete test_values;
				}
			}
		}

		{
			const PerfUtilities::ScopedTimer timer { "Benchmark" };
			for (int i = 0; i < 1000; i++) {
				for (int n = 0; n < 1000; n++) {
					int* test_values = new int[size];
					memcpy(test_values, values, (sizeof(int) * size));
					////////////////////////////////////////////////////////////

					// ShakerSort(test_values, size);
					// BubbleSort(test_values, size);
					// BubbleSort2(test_values, size);
					// BubbleSort_Updated_2(test_values, size);
					// InsertionSort(test_values, size);
					// SelectionSort(test_values, size);
					// QuickSort(test_values, size);
					// qs(test_values, 0, size - 1);
					// QSort(test_values, 0, size - 1);
					// Quicksort(test_values, 0, size - 1);
					// QSortEx(test_values, 0, size - 1);
					// HeapSort::heapSort(test_values, size);
					Quick_Sort_2(test_values, 0, size - 1);

					////////////////////////////////////////////////////////////
					delete test_values;
				}
			}
		}

		{
			const PerfUtilities::ScopedTimer timer { "Benchmark" };;
			for (int i = 0; i < 1000; i++) {
				for (int n = 0; n < 1000; n++) {
					int* test_values = new int[size];
					memcpy(test_values, values, (sizeof(int) * size));
					////////////////////////////////////////////////////////////

					// ShakerSort(test_values, size);
					// BubbleSort(test_values, size);
					// BubbleSort_Updated(test_values, size);
					// BubbleSort_Uodated_2(test_values, size);
					// InsertionSort(test_values, size);
					// SelectionSort(test_values, size);
					// QuickSort(test_values, size);
					// qs(test_values, 0, size - 1);
					QSort(test_values, 0, size - 1);
					// Quicksort(test_values, 0, size - 1);
					// QSortEx(test_values, 0, size - 1);
					// HeapSort::heapSort(test_values, size);

					////////////////////////////////////////////////////////////
					delete test_values;
				}
			}
		}
	}
};

void Sorting::TEST_ALL() {
	// SelectionSort();

	// BubbleSortTest();
	// BubbleSort_Updated_Test();

	// InsertionSort();
	 ShakerSort();
	// MergeSortTest();

	// QuickSortTest();
	// QuickSortTest_2();

	// HeapSort::SortTest();



	// TESTS();


	// Performance_TEST();
	// Performance_Tests();
};

