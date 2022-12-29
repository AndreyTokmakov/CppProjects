//============================================================================
// Name        : ReferenceWrapperTests.cpp
// Created on  : 
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Reference Wrapper src
//============================================================================

#include <iostream>
#include <string>
#include <string_view>

#include <functional>
#include <numeric>
#include <list>
#include <random>
#include <vector>

#include "../Integer/Integer.h"
#include "../TestSupport/TestSupport.h"
#include "ReferenceWrapper.h"

using String = std::string;
using CString = const String&;

namespace ReferenceWrapper {

	void Random_Ints_List()
	{
		std::list<int> int_list(10);

		std::iota(int_list.begin(), int_list.end(), -4);
		std::vector<std::reference_wrapper<int>> v(int_list.begin(), int_list.end());

		// can't use shuffle on a list (requires random access), but can use it on a vector
		std::shuffle(v.begin(), v.end(), std::mt19937{ std::random_device{}() });

		std::cout << "Contents of the list: ";
		for (int n : int_list) {
			std::cout << n << ' ';
		}

		std::cout << "\nContents of the list, as seen through a shuffled vector: ";
		for (int i : v) {
			std::cout << i << ' ';
		}

		std::cout << "\n\nDoubling the values in the initial list...\n\n";
		for (int& i : int_list) {
			i *= 2;
		}

		std::cout << "Contents of the list, as seen through a shuffled vector: ";
		for (int i : v) {
			std::cout << i << ' ';
		}
	}

	void Simple_Tests() {
		int a(10), b(20), c(30);
		std::reference_wrapper<int> refs[] = { a ,b, c };

		std::cout << "refs:";
		for (int& x : refs)
			std::cout << ' ' << x;
		std::cout << std::endl;

	}

	void CRef_Test() {
		auto func = [](int& n1, int& n2, const int& n3) {
			std::cout << "Inside function:" << n1 << ' ' << n2 << ' ' << n3 << '\n';
			++n1; // increment the copy n1 located in the functor
			++n2; // increments n2 located in main ()
			// ++n3; // compiler error
		};

		int n1 = 1, n2 = 2, n3 = 3;
		std::function<void()> bound_f = std::bind(func, n1, std::ref(n2), std::cref(n3));
		n1 = 10;
		n2 = 11;
		n3 = 12;
		std::cout << "Befor: " << n1 << ' ' << n2 << ' ' << n3 << std::endl;
		bound_f();
		std::cout << "Afte: " << n1 << ' ' << n2 << ' ' << n3 << std::endl;
	}

	void Vector_With_References() {

		std::vector<std::reference_wrapper<TestSupport::FirstType>> refVector;

		TestSupport::FirstType var1("Value1");
		refVector.push_back(var1);


		for (const auto& obj : refVector)
			std::cout << obj.get().getValue() << std::endl;

		var1.setValue("Velue1_TEST");

		for (const auto& obj : refVector)
			std::cout << obj.get().getValue() << std::endl;

		refVector.back().get().setValue("Velue1_TEST_NEW");

		std::cout << "Var1 : " << var1.getValue() << std::endl;
	}

	void Vector_List() {

		std::vector<std::reference_wrapper<TestSupport::FirstType>> refVector;

		TestSupport::FirstType var1("Value1");
		TestSupport::FirstType var2("Value2");
		TestSupport::FirstTypeDerived var3("Value3");
		 
		refVector.push_back(var1);
		refVector.push_back(var2);
		refVector.push_back(var3);

		for (const auto& obj : refVector)
			std::cout << obj.get().getValue() << std::endl;
	}

	void Vector_Test()
	{
		std::vector<std::reference_wrapper<Integer>> integers;
		integers.reserve(10);

		Integer int1(111);
		Integer int2(222);
		Integer int3(333);


		for (const std::reference_wrapper<Integer> intRef : integers)
			std::cout << intRef.get() << std::endl;

		integers.emplace_back(int1);
		integers.emplace_back(int2);
		integers.emplace_back(int3);
	}

	void Pass_And_Modify_Value()
	{
		const auto modify_integer = [](std::reference_wrapper<Integer> ref, int new_val) {
			ref.get().setValue(new_val);
		};

		Integer integer(111);
		std::reference_wrapper<Integer> int_ref(integer);

		std::cout << integer << std::endl;
		modify_integer(int_ref, 222);
		std::cout << integer << std::endl;
	}

	void callable0() {
		std::cout << "Referenced callable invoked without params" << std::endl;
	}

	void callable1(const std::string& text) {
		std::cout << "Referenced callable invoked: " << text << std::endl;
	}

	void Callable_Test_1() {
		{
			const auto func = std::reference_wrapper<void()>(callable0);
			func();
		}
		{
			const auto func = std::reference_wrapper<void(const std::string&)>(callable1);
			func("Input_Param_1");
		}
		{
			const auto func = callable1;
			func("Input_Param_1");
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////

	void InitRefVector(std::vector<std::reference_wrapper<Integer>>& intsVector) {
		Integer int1(111);
		Integer int2(222);
		Integer int3(333);

		intsVector.emplace_back(int1);
		intsVector.emplace_back(int2);
		intsVector.emplace_back(int3);

		for (const auto entry : intsVector)
			std::cout << entry.get() << std::endl;
	}


	void Bad_Usage() {
		std::vector<std::reference_wrapper<Integer>> intsVector;

		std::cout << "Init vector" << std::endl;
		InitRefVector(intsVector);
		

		std::cout << "\nPrint vector:" << std::endl;
		for (const auto entry : intsVector)
			std::cout << entry.get() << std::endl;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////

	void Reassign_Test() {
		std::string str1{"111"};
		std::string str2{"222"};

		std::reference_wrapper<std::string> ref = str1;

		std::cout << ref.get() << "   " << str1  << "   " << str2 << std::endl;
		ref.get().append("_new");
		std::cout << ref.get() << "   " << str1 << "   " << str2 << std::endl;


		ref = str2;

		std::cout << ref.get() << "   " << str1 << "   " << str2 << std::endl;
		ref.get().append("_new");
		std::cout << ref.get() << "   " << str1  << "   " << str2 << std::endl;
	}

	void Reassign_TestScoped() {
		std::string str1{ "111" };
		std::string str2{ "222" };

		std::reference_wrapper<std::string> ref = str1;
		std::cout << ref.get() << std::endl;

		{
			std::string str2{ "222" };
			ref = str2;
		}

		std::cout << ref.get() << std::endl;
	}
}


void ReferenceWrapper::TestAll()
{
	// Random_Ints_List();
	 Simple_Tests();
	// CRef_Test();

	// Vector_With_References();

	// Vector_List();
	// Vector_Test();
	// Pass_And_Modify_Value();

	// Callable_Test_1();

	// Bad_Usage();

	// Reassign_Test();
	Reassign_TestScoped();

	// Tests();

}
