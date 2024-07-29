/**============================================================================
Name        : Vector.cpp
Created on  : 05.05.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ List container testing
============================================================================**/

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <list>
#include <algorithm>

#include "List.h"
#include "../Helpers/Wrapper.h"

using Helpers::Integer;

namespace
{

	template<typename T>
	void print_list(const T& list)
    {
		std::for_each(list.begin(), list.end(), [](const auto& val) {
			std::cout << val << " ";
		});
	}

	template<typename T>
	void print_list_ex(const T& list,
					   std::string_view text = "",
					   std::string_view postfix = "\n") {
		std::cout << text;
		std::for_each(list.begin(), list.end(), [](const auto& val) {
			std::cout << val << "  ";
		});
		std::cout << postfix;
	}


    template<typename T>
    std::ostream& operator<<(std::ostream& ostr, const std::list<T>& list)
    {
        std::cout << __PRETTY_FUNCTION__ << std::endl;
        for (const auto& i : list)
            ostr << " " << i;
        return ostr;
    }
}

namespace List
{
	void Constructors()
    {
		// c++11 initializer list syntax:
		std::list<std::string> words1{ "the", "frogurt", "is", "also", "cursed" };
		print_list_ex(words1, "");

		std::list<std::string> words11 = { "val1", "val2", "val3", "val4", "val5" };
		print_list_ex(words11);

		// words2 == words1
		std::list<std::string> words2(words1.begin(), words1.end());
		print_list_ex(words1);

		// words3 == words1
		std::list<std::string> words3(words1);
		print_list_ex(words3);

		// words4 is {"Mo", "Mo", "Mo", "Mo", "Mo"}
		std::list<std::string> words4(5, "Mo");
		print_list_ex(words4);
	}

	void Reverse_and_Sort()
	{
		std::list<int> list = { 8,7,5,9,0,1,3,2,6,4 };
		std::cout << "before:     " << list << "\n";

		std::cout << "\nSort....\n" << std::endl;
		list.sort();
		std::cout << "ascending:  " << list << "\n";

		std::cout << "\nReverse....\n" << std::endl;
		list.reverse();

		std::cout << "descending: " << list << "\n";
	}

	void Unique() {
		std::list<Integer> list;

		list.emplace_back(1);
		list.emplace_back(2);
		list.emplace_back(2);
		list.emplace_back(3);
		list.emplace_back(3);
		list.emplace_back(3);
		list.emplace_back(4);
		list.emplace_back(4);
		list.emplace_back(4);
		list.emplace_back(4);

		print_list_ex(list, "\nList1 contains: ", "");

		std::cout << "\nCall unique()....\n" << std::endl;
		list.unique();
		std::cout << std::endl;

		print_list_ex(list, "\nList1 contains: ", "");
	}

	void Resize() {
		{
			std::list<Integer> integer_list;
			integer_list.emplace_back(1);
			integer_list.emplace_back(2);
			integer_list.emplace_back(3);
			integer_list.emplace_back(4);
			integer_list.emplace_back(5);

			print_list_ex(integer_list, "\nList contains: ");

			std::cout << "\nCall resize(3)...." << std::endl;
			integer_list.resize(3);
			std::cout << std::endl;

			print_list_ex(integer_list, "\nList contains: ");
		}

		std::cout << "\nTEST2:" << std::endl;
		{
			std::list<Integer> integer_list;
			integer_list.emplace_back(1);
			integer_list.emplace_back(2);
			integer_list.emplace_back(3);
			integer_list.emplace_back(4);
			integer_list.emplace_back(5);

			print_list_ex(integer_list, "\nList contains: ");

			std::cout << "\nCall resize(8)...." << std::endl;
			integer_list.resize(8);
			std::cout << std::endl;

			print_list_ex(integer_list, "\nList contains: ");
		}
	}

	void Swap() {
		std::list<Integer> list1;
		list1.emplace_back(1);
		list1.emplace_back(2);

		std::list<Integer> list2;
		list2.emplace_back(3);
		list2.emplace_back(4);

		print_list_ex(list1, "\nList1: ");
		print_list_ex(list2, "List2: ", "\n\n");

		std::cout << "swap()...." << std::endl;
		list1.swap(list2);

		print_list_ex(list1, "\nList1: ");
		print_list_ex(list2, "List2: ", "\n\n");
	}

	void PushFront_PopFront() {
		std::list<Integer> list;

		for (int i = 1; i <= 5; i++)
			list.push_front(Integer(i));

		print_list_ex(list, "\nList: ", "\n\n");

		while (false == list.empty()) {
			list.pop_front();
			print_list_ex(list, "", "\n");
		}
	}


	void PopBack() {
		std::list<Integer> list;

		for (int i = 1; i <= 5; i++)
			list.push_front(Integer(i));

		print_list_ex(list, "\nList: ", "\n\n");

		while (false == list.empty()) {
			list.pop_back();
			print_list_ex(list, "", "\n");
		}

	}

	void Emplace_front() {
		std::list<Integer> list;

		for (int i = 1; i < 6; i++)
			list.emplace_front(i);

		print_list_ex(list, "\nList: ", "\n\n");
	}

	void Emplace_front_ReturnValue() {
		std::list<std::string> list;

		auto result = list.emplace_front("123");
		std::cout << "Type: " << typeid(result).name() << std::endl;
		std::cout << "Type: " << result << std::endl;

	}

	void Remove() {
		std::list<int> list = { 0,1,2,3,4,5,6,7,8,9 };
		print_list_ex(list, "");

		list.remove(3);

		print_list_ex(list, "");
	}

	void Remove_IF() {
		std::list<int> list = { 0,1,2,3,4,5,6,7,8,9 };
		print_list_ex(list, "");

		// Delete ODD numbers
		list.remove_if([](int i) { return 1 == i % 2; });

		print_list_ex(list, "");
	}

	void AccessElement() {
		std::list<int> list = {0,1,2,3,4,5,6,7,8,9};
		print_list_ex(list, "");
		//std::cout << list[30] << std::endl;
	}

	void Sort()
    {
		{
			std::list<int> list = { 9,8,7,6,5,4,3,2,1};
			print_list_ex(list, "");

			list.sort();
			print_list_ex(list, "");
		}
		std::cout << std::endl;
		{
			std::list<int> list = { 1,2,3,4,5,6,7,8,9 };
			print_list_ex(list, "");

			list.sort([](int a, int b) { return a > b;  });
			print_list_ex(list, "");
		}
    }

    void Sort_WithCustomValue()
    {
        std::list<Integer> list = {3, 2, 1};
        std::cout << std::string(120, '=') << std::endl;

        std::cout << list << std::endl;

        list.sort([](const Integer& a, const Integer&  b) { return b > a;  });

        std::cout << list << std::endl;

        std::cout << std::string(120, '=') << std::endl;
    }

	void Merge() {
		{
			std::list<int> first = { 1,3,5,7,9 }, second = {2,4,6,8,10};
			print_list_ex(second, "list2: ", "\n\n");

			first.merge(second);
			print_list_ex(first, "Result: ");
			print_list_ex(second, "list2: ");
		}
		std::cout << "----------------- Test2: -----------------" << std::endl;
		{
			std::list<int> first = { 1,3,5,7,9 }, second = { 2,4,6,8,10 };
			print_list_ex(second, "list2: ", "\n\n");

			first.merge(second, [](int a, int b) { return a > b;  });
			print_list_ex(first, "Result: ");
			print_list_ex(second, "list2: ");
		}
	}

	void Slice_1() {
		{
			std::list<int> list1 = { 1, 2, 3, 4, 5 };
			std::list<int> list2 = { 10, 20, 30, 40, 50 };

			auto it = list1.begin();
			std::advance(it, 2);

			list1.splice(it, list2);

			std::cout << "list1: " << list1 << "\n";
			std::cout << "list2: " << list2 << "\n";
		}
		{
			std::list<int> list1 = { 1, 2, 3, 4, 5 };
			std::list<int> list2 = { 10, 20, 30, 40, 50 };

			auto it = list1.begin();
			std::advance(it, 2);

			list2.splice(list2.begin(), list1);

			std::cout << "list1: " << list1 << "\n";
			std::cout << "list2: " << list2 << "\n";
		}
	}

	void Slice_2() {
		{
			std::list<Integer> list1, list2;

			for (auto i : { 1,2,3,4,5 }) {
				list1.emplace_back(i);
				list2.emplace_back(i * 10);
			}

			auto it = list1.begin();
			std::advance(it, 2);

			list1.splice(it, list2);

			std::cout << "list1: " << list1 << "\n";
			std::cout << "list2: " << list2 << "\n";
		}
	}

	void Slice_3() {
		std::list<Integer> list;
		for (int i = 0; i < 10; ++i)
			list.emplace_back(i);

		std::list<Integer>::iterator iter = list.begin();
		std::advance(iter, 3);


		std::cout << list << "\n";

		std::cout << "Iterator points to -> " << *iter << "\n";
		list.splice(list.begin(), list, iter);

		std::cout << list << "\n";
	}

	void Slice_Iterator_Invalidate() {
		std::list<Integer> list1, list2;

		std::list<Integer>::iterator iter{ list1.end() };

		for (auto i : { 1,2,3,4,5 }) {
			list1.emplace_back(i);
			list2.emplace_back(i * 10);

			if (30 == i * 10) {
				iter = std::prev(list2.end());
			}
		}

		std::cout << "Iter points to: " << *iter << "\n\n";

		auto it = list1.begin();
		std::advance(it, 2);

		list1.splice(it, list2);

		std::cout << "list1: " << list1 << "\n";
		std::cout << "list2: " << list2 << "\n";
		std::cout << "Iter points to: " << *iter << "\n\n";
		
	}


	void Insert() {
		std::list<Integer> list1, list2;
		for (auto i : { 1,2,3,4,5 }) {
			list1.emplace_back(i);
			list2.emplace_back(i * 10);
		}

		list1.insert(list1.end(), list2.begin(), list2.end());

		std::cout << "list1: " << list1 << "\n";
		std::cout << "list2: " << list2 << "\n";
	}


    void Insert_ReturnValue() {
        std::list<int> list;
        for (auto i : { 1,2,3,4,5 }) {
            list.emplace_back(i);
        }

        auto iter = list.insert(list.end(), 6);


        std::cout << *iter << "\n";
        std::cout << list << "\n";
    }
};

namespace List
{
    struct Order {
        int value {0};
        int timestamp {0};
    };

    std::ostream& operator<<(std::ostream& stream, const Order& order) {
        stream << "Order[" << order.value << ", " << order.timestamp << "]";
        return stream;
    }

    bool operator<(const Order& a, const Order& b) {
        if (a.value == b.value)
            return a.timestamp > b.timestamp;
        else
            return a.value > b.value;
    }

    void Sort_CustomComparator() {
        std::list<Order> orders{};

        orders.emplace_back(Order{10, 3});
        orders.emplace_back(Order{12, 5});
        orders.emplace_back(Order{12, 7});
        orders.emplace_back(Order{4, 1});
        orders.emplace_back(Order{12, 9});

        auto beginIter = orders.begin();
        std::cout << "[ " << *beginIter << " ]" << std::endl;

        auto comparator = [](const auto &a, const auto& b) {
            if (a.value == b.value)
                return a.timestamp < b.timestamp;
            else
                return a.value > b.value;
        };
        orders.sort(comparator);

        /*
        orders.sort();  // INFO: Using ' bool operator<(const Order& a, const Order& b)'
        */


        for (const auto& obj: orders)
            std::cout << obj << std::endl;

        std::cout << "[ " << *beginIter << " ]" << std::endl;
    }
}

void List::TestAll()
{
	// Emplace_front();
	// Emplace_front_ReturnValue();

	// PushFront_PopFront();

	// PopBack();

	// Constructors();

	// Reverse_and_Sort();

	// Unique();

	// Resize();

	// Swap();

	// Insert();
    // Insert_ReturnValue();

	// Remove();
	// Remove_IF();

	// Sort();
	Sort_WithCustomValue();

	// Merge();

	// Slice_1();
	// Slice_2();
	// Slice_3();
	// Slice_Iterator_Invalidate();

	// AccessElement();

    // Sort_CustomComparator();
};