//============================================================================
// Name        : Stack.cpp
// Created on  : 16.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ STL Stack container testing
//============================================================================

#include <iostream>
#include <string>
#include <string_view>

#include <stack>
#include <vector>

#include "../Integer/Integer.h"
#include "Stack.h"

using String = std::string;
using CString = const String&;

namespace Stack {

	void Constructors() {
		std::deque<int> mydeque(3, 100);         // deque with 3 elements
		std::vector<int> myvector(2, 200);       // vector with 2 elements

		std::stack<int> first;                               // empty stack
		std::stack<int> second(mydeque);                     // stack initialized to copy of deque
		std::stack<int, std::vector<int> > third;            // empty stack using vector
		std::stack<int, std::vector<int> > fourth(myvector);

		std::cout << "size of first: " << first.size() << std::endl;
		std::cout << "size of secon d: " << second.size() << std::endl;
		std::cout << "size of third: " << third.size() << std::endl;
		std::cout << "size of fourth: " << fourth.size() << std::endl;
	}

	void Empty() {
		std::stack<int> mystack;
		int sum(0);

		for (int i = 1; i <= 10; i++) {
			mystack.push(i);
		}
		while (false == mystack.empty()) {
			std::cout << sum << " += " << mystack.top() << " = ";
			sum += mystack.top();
			std::cout << sum << std::endl;
			mystack.pop();
		}

		std::cout << "total: " << sum << std::endl;
	}

	void Top() {
		std::stack<int> mystack;

		mystack.push(10);
		mystack.push(20);

		std::cout << "mystack.top() " << mystack.top() << std::endl;

		mystack.top() -= 5;

		std::cout << "mystack.top() is now " << mystack.top() << std::endl;
	}

	void Emplace()
	{
		std::stack<Integer> mystack;

		mystack.emplace(111);
		mystack.emplace(222);

		std::cout << "mystack contains:" << std::endl;
		while (false == mystack.empty()) {
			std::cout << mystack.top() << std::endl;
			mystack.pop();
		}
	}

	void LIFO_Test() {
		std::stack<Integer> stack({ 1,2,3,4,5,6,7,8,9 });

		std::cout << "\nExtracting elements from stack:\n" << std::endl;
		while (false == stack.empty()) {
			std::cout << stack.top() << ".   ";
			stack.pop();
		}
	}

	void Swap() {
		std::stack<Integer> stack1, stack2;
		for (int i : {1, 2, 3}) {
			stack1.emplace(i);
			stack2.emplace(5 * i);
		}

		stack1.swap(stack2);

		while (false == stack1.empty()) {
			std::cout << stack1.top() << " ";
			stack1.pop();
		}
		std::cout  << std::endl;
		while (false == stack2.empty()) {
			std::cout << stack2.top() << " ";
			stack2.pop();
		}
		std::cout  << std::endl;
	}

	void __TEST__() {
		std::stack<int> stack({1,2,3,4,5,6,7,8,9});


		while (false == stack.empty()) {
			std::cout << stack.top() << std::endl;
			stack.pop();
		}
	}
}

namespace Stack {
	void TEST_ALL()
	{
		// Constructors();
		// Empty();
		// Top();
		// Emplace();
		// LIFO_Test();

		Swap();

		// __TEST__();
	}
}
