//============================================================================
// Name        : Stack.cpp
// Created on  : 16.07.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Stack src class
//============================================================================

#include <iostream>
#include <queue>
#include <stack>

#include "Stack.h"

namespace Stack::Stack_Using_Two_Queues {

	template<typename T>
	class Stack {
	private:
		std::queue<T> queue;
		std::queue<T> tmp;

	public:
		Stack() = default;

		void put(T v) {
			queue.push(v);
		}

		T get() {
			if (queue.empty())
				throw std::runtime_error("We are empty;");

			while (1 != queue.size()) {
				tmp.push(queue.front());
				queue.pop();
			}

			T result = queue.front();
			queue.pop();
			queue.swap(tmp);

			return result;
		}

		bool empty() {
			return queue.empty();
		}
	};

	///////////////////////////////// TESTS //////////////////////////////////////////

	void Test() {
		Stack<int> queue_stack;

		for (int i : {1, 2, 3, 4, 5})
			queue_stack.put(i);

		while (false == queue_stack.empty()) {
			std::cout << queue_stack.get() << std::endl;
		}

	}

	void Test1() {
		Stack<int> stack;

		for (int i : {1, 2, 3, 4, 5, 6, 7, 8, 9})
			stack.put(i);

		int i = 5;
		while (i--)
			std::cout << stack.get() << std::endl;

		std::cout << std::endl;
		for (int i : {11, 12, 13, 14, 15})
			stack.put(i);

		while (false == stack.empty()) {
			std::cout << stack.get() << std::endl;
		}
	}
}

namespace Stack::Stack_Using_One_Queue {
	template<typename T>
	class Stack {
	private:
		std::queue<T> queue;

	public:
		Stack() = default;

		void put(T v) {
			size_t size = queue.size();
			queue.push(v);

			while (size--) {
				queue.push(queue.front());
				queue.pop();
			}

		}

		T get() {
			T result = queue.front();
			queue.pop();
			return result;
		}

		bool empty() {
			return queue.empty();
		}
	};


	///////////////////////////////// TESTS //////////////////////////////////////////

	void Test() {
		Stack<int> queue;

		for (int i : {1, 2, 3, 4, 5})
			queue.put(i);

		while (false == queue.empty()) {
			std::cout << queue.get() << std::endl;
		}

	}

	void Test1() {
		Stack<int> stack;

		for (int i : {1, 2, 3, 4, 5, 6, 7, 8, 9})
			stack.put(i);

		int i = 5;
		while (i--)
			std::cout << stack.get() << std::endl;

		std::cout << std::endl;
		for (int i : {11, 12, 13, 14, 15})
			stack.put(i);

		while (false == stack.empty()) {
			std::cout << stack.get() << std::endl;
		}
	}
}

namespace Stack::MinStack {

	struct MinStack
	{
		std::stack<int> s;
		int min_element;

		// Prints minimum element:
		void getMin() {
			if (true == s.empty()) {
				std::cout << "Stack is empty" << std::endl;
			}
			else {
				std::cout << "Minimum Element in the stack is: " << min_element << std::endl;
			}
		}

		// Prints top element:
		void peek() {
			if (true == s.empty()) {
				std::cout << "Stack is empty " << std::endl;
				return;
			}

			std::cout << "Top Element: ";
			if (int top = s.top(); top > min_element) {
				std::cout << top << std::endl;
			}
			else {
				std::cout << min_element << std::endl;
			}
		}

		// Removes the top element:
		void pop() {
			if (true == s.empty()) {
				std::cout << "Stack is empty " << std::endl;
				return;
			}

			std::cout << "Top Element Removed: ";
			int top = s.top();
			s.pop();

			// Minimum will change as the minimum element of the stack is being removed. 
			if (min_element > top) {
				std::cout << min_element << std::endl;
				min_element = 2 * min_element - top;
			}
			else
				std::cout << top << std::endl;
		}

		// Removes top element from MyStack 
		void push(int x) {
			// Insert new number into the stack 
			if (true == s.empty()) {
				min_element = x;
				s.push(x);
				std::cout << "Number Inserted: " << x << std::endl;
				return;
			}

			// If new number is less than minEle 
			if (min_element > x) {
				s.push(2 * x - min_element);
				min_element = x;
			}
			else {
				s.push(x);
			}

			std::cout << "Number Inserted: " << x << std::endl;
		}
	};

	void TestMinStack() {
		{
			MinStack s;
			for (int i : {3, 4, 2, 1, 5, 6})
				s.push(i);

			s.getMin();
			s.pop();
			s.getMin();
			s.pop();
			s.getMin();
			s.pop();
			s.getMin();
		}

		/*
		s.push(2);
		s.push(1);
		s.getMin();
		s.pop();
		s.getMin();
		s.pop();
		s.peek();
		*/
	}
}

void Stack::TEST_ALL() {
	 Stack_Using_Two_Queues::Test();
	// Stack_Using_Two_Queues::Test1();

	// Stack_Using_One_Queue::Test();
	// Stack_Using_One_Queue::Test1();

	// MinStack::TestMinStack();
}
