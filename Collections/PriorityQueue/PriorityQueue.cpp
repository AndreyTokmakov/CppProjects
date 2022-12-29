//============================================================================
// Name        : PriorityQueue.cpp
// Created on  : 16.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ STL PriorityQueue container testing
//============================================================================

#include <iostream>       
#include <queue>          // std::priority_queue
#include <list>          // std::priority_queue

#include "PriorityQueue.h"

namespace PriorityQueue {

	/*
	auto printer_coma = [](const auto& val) {
		std::cout << val << ", ";
	};

	auto print_queue = [](const auto& queue) {
		std::for_each(staqueueck.begin(), queue.end(), printer_coma);
	};

	auto print_queue_ex = [](const auto& queue,
							 std::string_view text,
							 std::string_view postfix = "\n") {
		std::cout << text;
		print_stack(queue);
		std::cout << postfix << std::endl;
	};
	*/

	class comparator {
	private:
		bool reverse;
	public:
		comparator(const bool& revparam = false) {
			this->reverse = revparam;
		}
		bool operator()(const int& lhs,
			const int& rhs) const {
			if (this->reverse) {
				return (lhs > rhs);
			}
			else {
				return (lhs < rhs);
			}
		}
	};

	struct compare1 {
		bool operator()(const int& l, const int& r) 	{
			return l < r;
		}
	};

	//////////////////////////////////////////////////////////////

	void Constructors()
	{
		const int myints[] = { 10, 60, 50, 20 };

		std::priority_queue<int> first;
		std::priority_queue<int> second(myints, myints + 4);
		std::priority_queue<int, std::vector<int>, std::greater<int>> third(myints, myints + 4);

		// using mycomparison:
		typedef std::priority_queue<int, std::vector<int>, comparator> mypq_type;

		mypq_type fourth;                       // less-than comparison
		mypq_type fifth(myints, myints + 4, comparator(true));      // greater-than comparison
		mypq_type six(myints, myints + 4, comparator(false));      // greater-than comparison

		std::cout << "second contains:";
		while (false == second.empty()) {
			std::cout << ' ' << second.top();
			second.pop();
		}
		std::cout << std::endl;

		std::cout << "third contains:";
		while (false == third.empty()) {
			std::cout << ' ' << third.top();
			third.pop();
		}
		std::cout << std::endl;

		std::cout << "fifth contains:";
		while (false == fifth.empty()) {
			std::cout << ' ' << fifth.top();
			fifth.pop();
		}
		std::cout << std::endl;

		std::cout << "six contains:";
		while (false == six.empty()) {
			std::cout << ' ' << six.top();
			six.pop();
		}
		std::cout << std::endl;
	}

	void SimpleTest() {
		{
			std::priority_queue<int, std::vector<int>, std::greater<int>> queue;

			queue.push(6);
			queue.push(3);
			queue.push(5);
			queue.push(1);
			queue.push(12);

			while (false == queue.empty()) {
				std::cout << ' ' << queue.top();
				queue.pop();
			}
			std::cout << std::endl;
		}

		{
			std::priority_queue<int, std::vector<int>, std::less<int>> queue;

			queue.push(6);
			queue.push(3);
			queue.push(5);
			queue.push(1);
			queue.push(12);

			while (false == queue.empty()) {
				std::cout << ' ' << queue.top();
				queue.pop();
			}
			std::cout << std::endl;
		}
	}

	void Update() {

		std::priority_queue<int, std::vector<int>, std::greater<int>> queue;

		queue.push(6);
		queue.push(3);
		queue.push(5);


		int v = 10;
		if (v > queue.top()) {
			queue.pop();
			queue.push(v);
		}

		v = 20;
		if (v > queue.top()) {
			queue.pop();
			queue.push(v);
		}

		while (false == queue.empty()) {
			std::cout << ' ' << queue.top();
			queue.pop();
		}
		std::cout << std::endl;
	}

	using my_pair_t = std::pair<size_t, bool>;
	using my_container_t = std::vector<my_pair_t>;

	void Custom_Comparator()
	{
		{
			auto my_comp = [](const my_pair_t& e1, const my_pair_t& e2) {
				return e1.first > e2.first;
			};
			std::priority_queue<my_pair_t, my_container_t, decltype(my_comp)> queue(my_comp);

			queue.push(std::make_pair(5, true));
			queue.push(std::make_pair(3, false));
			queue.push(std::make_pair(7, true));
			std::cout << std::boolalpha;

			while (!queue.empty())
			{
				const auto& p = queue.top();
				std::cout << p.first << " " << p.second << "\n";
				queue.pop();
			}
		}
		std::cout << "----------------------------------- Test2: -------------------------------------" << std::endl;
		{
			using Pair = std::pair<size_t, bool>;
			using ContainerType = std::vector<Pair>;

			auto comparator = [](const Pair& e1, const Pair& e2) {
				return e1.first < e2.first;
			};
			std::priority_queue<Pair, ContainerType, decltype(comparator)> queue(comparator);

			queue.push(std::make_pair(5, true));
			queue.push(std::make_pair(3, false));
			queue.push(std::make_pair(7, true));


			std::cout << std::boolalpha;

			while (!queue.empty())
			{
				const auto& p = queue.top();
				std::cout << p.first << " " << p.second << "\n";
				queue.pop();
			}
		}
	}

	void Custom_Comparator_UPDATE_value()
	{
		using Pair = std::pair<size_t, bool>;
		using ContainerType = std::vector<Pair>;

		auto comparator = [](const Pair& e1, const Pair& e2) {
			return e1.first < e2.first;
		};
		std::priority_queue<Pair, ContainerType, decltype(comparator)> queue(comparator);

		queue.emplace(3, true);
		queue.emplace(5, true);
		queue.emplace(7, true);


		std::cout << std::boolalpha;


		while (!queue.empty()) {
			const auto& p = queue.top();
			std::cout << p.first << " " << p.second << "\n";
			queue.pop();
		}
	}





	void Different_Order_Comparators() {
		const std::vector<int> data { 5,6, 2, 8, 12,4,87,24,1 ,13,4,45,1 };

		std::cout << "------------------------------------------------- Test1: -----------------------------------------------" << std::endl;
		{
			std::priority_queue<int, std::vector<int>, std::greater<int>> result(data.begin(), data.end());
			while (false == result.empty()) {
				std::cout << ' ' << result.top();
				result.pop();
			}
		}

		std::cout << "\n\n------------------------------------------------- Test2: -----------------------------------------------\n" << std::endl;

		{
			std::priority_queue<int, std::vector<int>> result(data.begin(), data.end());
			while (false == result.empty()) {
				std::cout << ' ' << result.top();
				result.pop();
			}
		}
	}
}
	
void PriorityQueue::TEST_ALL()
{
	// Constructors();

	// SimpleTest();

	// Update();

	// Different_Order_Comparators();

	// Custom_Comparator();

	// Custom_Comparator_UPDATE_value();

}