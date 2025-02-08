//============================================================================
// Name        : Queues.cpp
// Created on  : 13.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Queues C++ algoritms
//============================================================================

#include "Queues.h"

#include <iostream>
#include <stack>
#include <queue>
#include <algorithm>
#include <vector>

namespace Queues::Queue_Using_Stacks {

	template<typename T>
	class Queue {
	private:
		std::stack<T> stack1;
		std::stack<T> stack2;

	public:
		Queue() = default;

		void put(T v) {
			stack1.push(v);
		}

		T get() {
			if (stack2.empty() && stack1.empty())
				throw std::runtime_error("We are empty;");

			if (stack2.empty()) {
				while (false == stack1.empty()) {
					stack2.push(stack1.top());
					stack1.pop();
				}
			}

			T result = stack2.top();
			stack2.pop();
			return result;
		}

		bool empty() {
			return stack1.empty() && stack2.empty();
		}
	};

	void Test() {
		Queue<int> queue;

		for (int i : {1, 2, 3, 4, 5})
			queue.put(i);

		while (false == queue.empty()) {
			std::cout << queue.get() << std::endl;
		}

	}

	void Test1() {
		Queue<int> queue;

		for (int i : {1, 2, 3, 4, 5, 6, 7, 8, 9})
			queue.put(i);

		int i = 5;
		while (i--)
			std::cout << queue.get() << std::endl;

		std::cout << std::endl;
		for (int i : {11, 12, 13, 14, 15})
			queue.put(i);

		while (false == queue.empty()) {
			std::cout << queue.get() << std::endl;
		}
	}
}

namespace Queues::Queue_Using_Stacks_Recursive {

	template<typename T>
	class Queue {
	private:
		std::stack<T> stack;

	public:
		Queue() = default;

		void put(T v) {
			stack.push(v);
		}

		T get() {
			if (stack.empty())
				throw std::runtime_error("We are empty;");

			T x = stack.top();
			stack.pop();
			if (stack.empty())
				return x;

			// recursive call 
			T item = get();

			stack.push(x);
			return item;
		}

		bool empty() {
			return stack.empty();
		}
	};

	void Test() {
		Queue<int> queue;

		for (int i : {1, 2, 3, 4, 5})
			queue.put(i);

		while (false == queue.empty()) {
			std::cout << queue.get() << std::endl;
		}

	}

	void Test1() {
		Queue<int> queue;

		for (int i : {1, 2, 3, 4, 5, 6, 7, 8, 9})
			queue.put(i);

		int i = 5;
		while (i--)
			std::cout << queue.get() << std::endl;

		std::cout << std::endl;
		for (int i : {11, 12, 13, 14, 15})
			queue.put(i);

		while (false == queue.empty()) {
			std::cout << queue.get() << std::endl;
		}
	}
}

namespace Queues::Algoritms {

	int max(const std::vector<int>& orig, std::vector<int>& current_vector, int iter) {
		auto max = std::max_element(current_vector.begin(), current_vector.end());
		int maxVal = *max + iter;
		current_vector.erase(max);
		int count = 0;
		for (int v : orig) {
			count++;
			if (v = maxVal)
				return count;
		}
		return -1;
	}

	int magic(std::vector<int>& arr, int x, int iter, const std::vector<int>& orig) {
		std::queue<int> queue;
		for (int i : arr)
			queue.push(i);

		arr.clear();
		while (false == queue.empty() && x-- > 0) {
			arr.push_back(queue.front());
			queue.pop();
		}

		int pos = max(orig, arr, iter);
		std::for_each(arr.begin(), arr.end(), [&queue](int v) {
			queue.push(v > 0 ? v - 1 : 0);
			});

		arr.clear();
		while (false == queue.empty()) {
			arr.push_back(queue.front());
			queue.pop();
		}
		return pos;
	}

	void findPositions(std::vector<int> arr, int x) {
		std::vector<int> data = arr;
		for (int i = 0; i < x; i++) {
			int X = magic(data, x, i, arr);

			std::cout << X << " [ ";
			for (int i : data)
				std::cout << i << " ";
			std::cout << "]" << std::endl;
		}
	}


	void QueueRemovals()
	{
		int x = 5;
		std::vector<int> arr = { 1, 2, 2, 3, 4, 5 };
		findPositions(arr, x);
	}
}

void Queues::TEST_ALL() {
	Queue_Using_Stacks::Test();
	// Queue_Using_Stacks::Test1();

	// Queue_Using_Stacks_Recursive::Test();
	// Queue_Using_Stacks_Recursive::Test1();

	// Algoritms::QueueRemovals();
}
