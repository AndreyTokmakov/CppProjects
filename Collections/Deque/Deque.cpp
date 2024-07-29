//============================================================================
// Name        : Deque.h
// Created on  : 05.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Deque container testing
//============================================================================

#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <algorithm>
#include <deque>
#include <thread>


#include <mutex>
#include <functional>
#include <condition_variable>


#include "Deque.h"
#include "../Helpers/Integer.h"

namespace Deque {

	template<typename T>
	void print(const T& list) {
		std::for_each(list.begin(), list.end(), [](const auto& val) {
			std::cout << val << ", ";
		});
	}

	template<typename T>
	void print_ex(const T& list,
		std::string_view text = "",
		std::string_view postfix = "\n") {
		std::cout << text;
		std::for_each(list.begin(), list.end(), [](const auto& val) {
			std::cout << val << ", ";
		});
		std::cout << postfix;
	}
}

namespace Deque {

	void Construct() {

		std::deque<std::string> words0{ "one"};
		print_ex(words0);

		// c++11 initializer list syntax:
		std::deque<std::string> words1 { "the", "frogurt", "is", "also", "cursed" };
		print_ex(words1);

		// words2 == words1
		std::deque<std::string> words2(words1.begin(), words1.end());
		print_ex(words2);

		// words3 == words1
		std::deque<std::string> words3(words1);
		print_ex(words3);

		// words4 is {"Mo", "Mo", "Mo", "Mo", "Mo"}
		std::deque<std::string> words4(5, "Mo");
		print_ex(words4);
	}

	void PushBack() {
		std::deque<Integer> numbers;
		for (int i = 1; i <= 3; i++)
			numbers.push_back(Integer(i));
	}

	void Push_Front() {
		std::deque<int> numbers;
		for (auto i : {1,2,3,4,5,6,7,8,9})
			numbers.push_front(i);
		print_ex(numbers);
	}

	void EmplaceBack() {
		std::deque<Integer> numbers;
		for (int i = 1; i <= 3; i++)
			numbers.emplace_back(i);
		print_ex(numbers);
	}

	void EmplaceFront() {
		std::deque<Integer> numbers;
		for (int i = 1; i <= 3; i++)
			numbers.emplace_front(i);
		print_ex(numbers);
	}

	void Emplace() {
		std::deque<Integer> numbers;
		std::deque<Integer>::iterator it = numbers.begin();
		for (int i = 1; i <= 3; i++)
			it = numbers.emplace(it, i);
		print_ex(numbers);
	}

    void Erase() {
        std::deque<int> numbers {1,2,3,4,5,6,7,8,9};
        print_ex(numbers);
        numbers.erase(std::find(numbers.cbegin(), numbers.cend(), 5));
        print_ex(numbers);
    }

	void For_Loops() {
		std::cout << "\nTest2:" << std::endl;
		std::deque<int> numbers = {1, 2, 3, 4, 5, 6 ,7 ,8 ,9};
		{
			std::for_each(numbers.begin(), numbers.end(), [](int i) { std::cout << i << " " ; });
			std::cout << std::endl;
		}
		std::cout << "\nTest2:" << std::endl;
		{
			for (const auto& v: numbers)
				std::cout << v << " ";
			std::cout << std::endl;
		}
		std::cout << "\nTest3:" << std::endl;
		{
			for (unsigned int i = 0; i < numbers.size(); ++i)
				std::cout << numbers[i] << " ";
			std::cout << std::endl;
		}
		std::cout << "\nTest4:" << std::endl;
		{
			for (unsigned int i = 0; i < numbers.size(); ++i)
				std::cout << numbers.at(i) << " ";
			std::cout << std::endl;
		}
	}

	void Pop_Front() {
		std::deque<Integer> numbers;

		numbers.emplace_back(1);
		numbers.emplace_back(2);
		numbers.emplace_back(3);

		Integer front = numbers[0];
	}
};

namespace Deque::Applications {

	Integer Pop_Front() {
		std::deque<Integer> numbers;

		numbers.emplace_back(1);
		numbers.emplace_back(2);
		numbers.emplace_back(3);

		auto integer = std::move(numbers.front());
		numbers.pop_front();

		return integer;
	}

	/*
	void Pop_Front_TEST() {
		std::deque<Integer> numbers;

		numbers.emplace_back(1);
		numbers.emplace_back(2);
		numbers.emplace_back(3);

		Integer&& integer = std::move(numbers[1]);
		numbers.pop_front();

		integer.printInfo();
	}
	*/

	void Pop_Element_Performance() {
		Integer integer = Pop_Front();
		integer.printInfo();

		// Pop_Front1();
	}

	//------------------------------------------------------

	void Move_Pop_FromDeque() {

		std::deque<Integer> numbers;

		numbers.emplace_back(1);
		numbers.emplace_back(2);
		numbers.emplace_back(3);

		auto&& integer = std::move(numbers.front());
		numbers.pop_front();


		integer.printInfo();
	}

	void Move_Pop_FromDeque_Lambdas() 
	{

		auto job1 = []() {
			std::cout << "Starting job" << std::endl;
			std::this_thread::sleep_for(std::chrono::microseconds(250));
			std::cout << "Job done" << std::endl;
		};

		auto job2 = []() {
			std::cout << "Starting job" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << "Job done" << std::endl;
		};

		std::deque<std::function<void()>> jobs;
		jobs.emplace_back(job1);
		jobs.emplace_back(job2);

		const auto&& task = std::move(jobs.front());
		jobs.pop_front();

		// jobs.front()();

		task();
	}
}

void Deque::TEST_ALL() {
	// Construct();

	// PushBack();

	// Push_Front();
	// Pop_Front();

	// Emplace();

    Erase();

	// EmplaceBack();

	// EmplaceFront();

	// For_Loops();

	// Applications::Pop_Element_Performance();
	// Applications::Move_Pop_FromDeque();
	// Applications::Move_Pop_FromDeque_Lambdas();
};
