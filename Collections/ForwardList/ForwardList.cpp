//============================================================================
// Name        : ForwardList.h
// Created on  : 29.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ ForwardList container testing
//============================================================================

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <algorithm>
#include <forward_list>
#include <array>

#include "../Helpers/Integer.h"
#include "ForwardList.h"

using String = std::string;
using CString = const String&;

namespace ForwardList {

	auto printer_coma = [](const auto& val) {
		std::cout << val << ", ";
	};

	auto print_list = [](const auto& list) {
		std::for_each(list.begin(), list.end(), printer_coma);
	};

	auto print_list_ex = [](const auto& list,
		std::string_view text,
		std::string_view postfix = "\n") {
		std::cout << text;
		print_list(list);
		std::cout << postfix << std::endl;
	};

	void Constructors() {
		std::forward_list<int> first;                      // default: empty
		std::forward_list<int> second(3, 77);              // fill: 3 seventy-sevens
		std::forward_list<int> third(second.begin(), second.end()); // range initialization
		std::forward_list<int> fourth(third);            // copy constructor
		std::forward_list<int> fifth(std::move(fourth));  // move ctor. (fourth wasted)
		std::forward_list<int> sixth = { 3, 52, 25, 90 };    // initializer_list constructor

		std::cout << "first:";  for (int& x : first)  std::cout << " " << x; std::cout << std::endl;
		std::cout << "second:"; for (int& x : second) std::cout << " " << x; std::cout << std::endl;
		std::cout << "third:";  for (int& x : third)  std::cout << " " << x; std::cout << std::endl;
		std::cout << "fourth:"; for (int& x : fourth) std::cout << " " << x; std::cout << std::endl;
		std::cout << "fifth:";  for (int& x : fifth)  std::cout << " " << x; std::cout << std::endl;
		std::cout << "sixth:";  for (int& x : sixth)  std::cout << " " << x; std::cout << std::endl;
	}

	void PushFront() {
		std::forward_list<int> mylist = { 77, 2, 16 };

		print_list_ex(mylist, "List (Before):");

		mylist.push_front(19);
		mylist.push_front(34);

		print_list_ex(mylist, "List (After):");
	}

	void PopFront_Front() {
		std::forward_list<int> mylist = { 10, 20, 30, 40, 50 };
		std::cout << "Popping out the elements in mylist: " << std::endl;
		while (false == mylist.empty()) {
			std::cout << "Front element: " << mylist.front();
			print_list_ex(mylist, ", List: ", "");
			mylist.pop_front();
		}
		std::cout << std::endl;
	}

	void BeforeBegin() {
		std::forward_list<int> mylist = { 20, 30, 40, 50 };
		print_list_ex(mylist, "before: ", "");


		mylist.insert_after(mylist.before_begin(), 11);
		print_list_ex(mylist, "after: ", "");
		std::cout << std::endl;
	}

	void InsertAfter()
	{
		std::array<int, 3> myarray{ 11, 22, 33 };
		std::forward_list<int> mylist;
		std::forward_list<int>::iterator it;

		it = mylist.insert_after(mylist.before_begin(), 10);
		print_list_ex(mylist, "List (Test 1): ", "");

		it = mylist.insert_after(it, 2, 20);
		print_list_ex(mylist, "List (Test 2): ", "");

		it = mylist.insert_after(it, myarray.begin(), myarray.end());
		print_list_ex(mylist, "List (Test 3): ", "");

		it = mylist.begin();

		it = mylist.insert_after(it, { 1,2,3 });
		print_list_ex(mylist, "List (Test 4): ", "");
	}

	void EmplaceFront() {
		//std::forward_list<std::pair<String, Integer>> dict;
		std::forward_list<Integer> list;

		std::cout << "=======>  Using emplace_front():\n" << std::endl;
		for (int i = 1; i <= 3; i++)
			list.emplace_front(i * 100);

		print_list_ex(list, "\nList contains: ", "\nClean up list.\n");
		list.clear();

		std::cout << " \n\n=======>Using push_front():\n" << std::endl;
		for (int i = 1; i <= 3; i++)
			list.push_front(Integer(i * 100));
	}


	void EmplaceAfter() {
		std::forward_list<Integer> list;
		auto iter = list.before_begin();

		std::cout << "=======>  Using emplace_after() [Test1]:\n" << std::endl;
		for (int i = 1; i <= 3; i++)
			iter = list.emplace_after(iter, i * 100);

		print_list_ex(list, "\nList contains: ", "\nClean up list.\n");
		list.clear();


		std::cout << "\n=======>  Using emplace_after() [Test2]:\n" << std::endl;
		for (int i = 1; i <= 3; i++)
			iter = list.emplace_after(list.before_begin(), i * 100);

		print_list_ex(list, "\nList contains: ", "\nClean up list.\n");
		list.clear();

		iter = list.before_begin();
		std::cout << " \n\n=======>Using insert_after():\n" << std::endl;
		for (int i = 1; i <= 3; i++)
			iter = list.insert_after(iter, Integer(i * 100));
	}

	void MaxSize() {
		size_t myint;
		std::string mystring;
		std::forward_list<int> mylist;

		std::cout << "Enter size: ";
		std::getline(std::cin, mystring);
		std::stringstream(mystring) >> myint;

		if (myint <= mylist.max_size())
			mylist.resize(myint);
		else
			std::cout << "That size exceeds the maximum.\n";
	}

	void Swap() {
		std::forward_list<Integer> list1;
		auto it = list1.emplace_after(list1.before_begin(), 1);
		it = list1.emplace_after(it, 2);

		std::forward_list<Integer> list2;
		it = list2.emplace_after(list2.before_begin(), 3);
		it = list2.emplace_after(it, 4);

		print_list_ex(list1, "\nList1 contains: ", "");
		print_list_ex(list2, "List2 contains: ", "");

		list1.swap(list2);

		print_list_ex(list1, "List1 contains: ", "");
		print_list_ex(list2, "List2 contains: ");

	}

	std::ostream& operator<<(std::ostream& ostr, const std::forward_list<int>& list) {
		for (auto &i : list) 
			ostr << " " << i;
		return ostr;
	}

	void Reverse_and_Sort()
	{
		std::forward_list<int> list = { 8,7,5,9,0,1,3,2,6,4 };
		std::cout << "before:     " << list << "\n";

		std::cout << "\nSort....\n" << std::endl;
		list.sort();
		std::cout << "ascending:  " << list << "\n";

		std::cout << "\nReverse....\n" << std::endl;
		list.reverse();

		std::cout << "descending: " << list << "\n";
	}

	void Unique() {

		std::forward_list<Integer> list1;

		auto it = list1.emplace_after(list1.before_begin(), 1);

		it = list1.emplace_after(it, 2);
		it = list1.emplace_after(it, 2);

		it = list1.emplace_after(it, 3);
		it = list1.emplace_after(it, 3);
		it = list1.emplace_after(it, 3);

		it = list1.emplace_after(it, 4);
		it = list1.emplace_after(it, 4);
		it = list1.emplace_after(it, 4);
		it = list1.emplace_after(it, 4);


		print_list_ex(list1, "\nList1 contains: ", "");
		
		std::cout << "\nCall unique()...." << std::endl;
		list1.unique();
		std::cout << std::endl;

		print_list_ex(list1, "\nList1 contains: ", "");
	}

	void Merge()
	{
		std::forward_list<int> list1 = { 5,9,0,1,3 };
		std::forward_list<int> list2 = { 8,7,2,6,4 };

		list1.sort();
		list2.sort();
		std::cout << "list1:  " << list1 << std::endl;
		std::cout << "list2:  " << list2 << std::endl;
		list1.merge(list2);
		std::cout << "merged: " << list1 << std::endl;
	}

	void Resize() {
		{
			std::forward_list<Integer> list1;

			auto it = list1.emplace_after(list1.before_begin(), 1);
			it = list1.emplace_after(it, 3);
			it = list1.emplace_after(it, 2);
			it = list1.emplace_after(it, 4);
			it = list1.emplace_after(it, 5);

			print_list_ex(list1, "\nList contains: ", "");

			std::cout << "\nCall resize(3)...." << std::endl;
			list1.resize(3);
			std::cout << std::endl;

			print_list_ex(list1, "\nList contains: ", "");
		}
		std::cout << "\nTEST2:" << std::endl;
		{
			std::forward_list<Integer> list1;

			auto it = list1.emplace_after(list1.before_begin(), 1);
			it = list1.emplace_after(it, 2);
			it = list1.emplace_after(it, 3);
			it = list1.emplace_after(it, 4);
			it = list1.emplace_after(it, 5);

			print_list_ex(list1, "\nList contains: ", "");

			std::cout << "\nCall resize(8)...." << std::endl;
			list1.resize(8);
			std::cout << std::endl;

			print_list_ex(list1, "\nList contains: ", "");
		}
	}


}

void ForwardList::TEST_ALL() {
	// Constructors();
	// MaxSize();

	BeforeBegin();

	// PushFront();
	// EmplaceFront();

	// InsertAfter();

	// EmplaceAfter();

	// PopFront_Front();

	// Swap();

	// Reverse_and_Sort();

	//  Unique();

	// Merge();

	// Resize();
}