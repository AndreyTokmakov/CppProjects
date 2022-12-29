//============================================================================
// Name        : Valarray.h
// Created on  : 01.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ STL Valarray container testing
//============================================================================

#include "Valarray.h"

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <cassert>
#include <algorithm>
#include <chrono>
#include <valarray>     // std::valarray, std::slice

/** Valarray namespace: **/
namespace Valarray {

	using String = std::string;
	using CString = const String&;

	auto print_valarray = [](const auto& valarray) {
		for (const auto& entry : valarray)
			std::cout << entry << ", ";
	};

	auto print_valarray_ex = [](const auto& vector,
							    std::string_view text,
							    std::string_view postfix = "\n") {
		std::cout << text;
		print_valarray(vector);
		std::cout << postfix << std::endl;
	};
}

namespace Valarray
{
	void SimpleTest() {
		const int data[] = {1,2,3,4,5,6,7,8,9};
		std::valarray<int> varray(data, 9);
		print_valarray_ex(varray, "");
	}


	void Constructor()
	{
		const int init[] = {1,2,3,4,5,6,7,8,9};

		std::valarray<int> first;
		print_valarray_ex(first, "first: ");

		std::valarray<int> second(5);
		print_valarray_ex(second, "second: ");

		std::valarray<int> third(10, 3);
		print_valarray_ex(third, "third: ");

		std::valarray<int> fourth(init,4);
		print_valarray_ex(fourth, "fourth: ");

		std::valarray<int> fifth(fourth);
		print_valarray_ex(fifth, "fifth: ");

		std::valarray<int> sixth(fifth[std::slice(1,2,1)]);
		print_valarray_ex(sixth, "sixth: ");
	
		// std::cout << "sixth sums " << sixth.sum() << '\n';
	}

	void Apply()
	{
		const auto increment = [](int x)-> int { return ++x; };

		int init[] = { 10,20,30,40,50 };
		std::valarray<int> foo (init, 5);
		std::valarray<int> bar = foo.apply(increment);

		print_valarray_ex(foo, "foo: ");
		print_valarray_ex(bar, "bar: ");
	}

	void Operators() {
		int init[] = { 10,20,30,40 };
		std::valarray<int> foo(init, 4);  
		std::valarray<int> bar(25, 4);    

		print_valarray_ex(foo, "foo: ");
		print_valarray_ex(bar, "bar: ");

		bar += foo;
		print_valarray_ex(bar, "bar: ");

		foo = bar + 10;
		print_valarray_ex(foo, "foo: ");

		foo -= 10;
		print_valarray_ex(foo, "foo: ");

		std::valarray<bool> comp = (foo == bar);

		if (comp.min() == true)
			std::cout << "foo and bar are equal.\n";
		else
			std::cout << "foo and bar are not equal.\n";
	}

	void Cshift()
	{
		const int init[] = { 1,2,3,4,5,6,7,8,9 };
		constexpr size_t length = sizeof(init) / sizeof(init[0]);

		std::valarray<int> myvalarray(init, length);
		print_valarray_ex(myvalarray, "myvalarray: ");

		myvalarray = myvalarray.cshift(3);
		print_valarray_ex(myvalarray, "myvalarray: ");

		myvalarray = myvalarray.cshift(-5);
		print_valarray_ex(myvalarray, "myvalarray: ");
	}

	void Shift()
	{
		const int init[] = { 1,2,3,4,5 };
		constexpr size_t length = sizeof(init) / sizeof(init[0]);

		std::valarray<int> myvalarray(init, length);
		print_valarray_ex(myvalarray, "myvalarray: ");

		myvalarray = myvalarray.shift(2);
		print_valarray_ex(myvalarray, "myvalarray: ");

		myvalarray = myvalarray.shift(-1);
		print_valarray_ex(myvalarray, "myvalarray: ");
	}

	void max()
	{
		int init[] = { 20,40,10,30 };
		std::valarray<int> myvalarray(init, 4);
		std::cout << "The max is " << myvalarray.max() << std::endl;
	}

	void min()
	{
		int init[] = { 20,40,10,30 };
		std::valarray<int> myvalarray(init, 4);
		std::cout << "The min is " << myvalarray.min() << std::endl;
	}

	void abs()
	{
		const int init[] = { 1,-2,3,-4,5 };
		constexpr size_t length = sizeof(init) / sizeof(init[0]);

		std::valarray<int> foo(init, length);
		print_valarray_ex(foo, "foo: ");

		std::valarray<int> bar = std::abs(foo);
		print_valarray_ex(bar, "bar: ");

	}
}

void Valarray::TEST_ALL() 
{
	// SimpleTest();

	// Constructor();

	// Apply();

	// Operators();

	// Cshift();
	//  Shift();

	// max();
	// min();

	abs();
}
