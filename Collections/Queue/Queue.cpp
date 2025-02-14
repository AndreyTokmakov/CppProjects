//============================================================================
// Name        : Queue.cpp
// Created on  : 16.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ STL Stack container testing
//============================================================================

#include <iostream>
#include <string>
#include <string_view>

#include <queue>
#include <list>

#include "../Helpers/Integer.h"
#include "Queue.h"

namespace Queue {

	template<typename T>
	void print(const std::queue<T>& queue, std::string_view text = "") {
		std::queue<T> tmp(queue);
		std::cout << text;
		while (false == tmp.empty()) {
			std::cout << tmp.front() << " ";
			tmp.pop();
		}
		std::cout << std::endl;
	}

	void Construct()
	{
		std::deque<int> mydeck(3, 100);         // deque with 3 elements
		std::list<int> mylist(2, 200);          // list with 2 elements

		std::queue<int> first;                  // empty queue
		std::queue<int> second(mydeck);         // queue initialized to copy of deque
		std::queue<int, std::list<int> > third; // empty queue with list as underlying container
		std::queue<int, std::list<int> > fourth(mylist);

		std::cout << "size of first: " << first.size() << std::endl;
		std::cout << "size of second: " << second.size() << std::endl;
		std::cout << "size of third: " << third.size() << std::endl;
		std::cout << "size of fourth: " << fourth.size() << std::endl;
	}

	void Empty() {
		std::queue<int> myqueue;
		int sum(0);

		for (int i = 1; i <= 10; i++) {
			myqueue.push(i);
		}
		while (false == myqueue.empty()) {
			sum += myqueue.front();
			myqueue.pop();
		}
		std::cout << "Sum: " << sum << std::endl;
	}

	void Front_Back() {
		std::queue<int> queue;

		std::cout << "Before initalzation:" << std::endl;
		std::cout << "queue.size()  = " << queue.size() << std::endl;
		std::cout << "queue.empty() = " << queue.empty() << std::endl;

		for (int i = 5; i <= 10; i++)
			queue.push(i);

		std::cout << "\nAfter initalzation:\n" << std::endl;

		std::cout << "queue.front() = " << queue.front() << std::endl;
		std::cout << "queue.back()  = " << queue.back() << std::endl;
		std::cout << "queue.size()  = " << queue.size() << std::endl;
		std::cout << "queue.empty() = " << queue.empty() << std::endl;
	}

	void Emplace() {
		std::queue<std::string> myqueue;

		myqueue.emplace("First sentence");
		myqueue.emplace("Second sentence");

		std::cout << "myqueue contains:" << std::endl;
		while (!myqueue.empty()) {
			std::cout << myqueue.front() << std::endl;
			myqueue.pop();
		}
	}

	void Emplace2() {
		std::queue<Integer> queue;

		queue.emplace(111);
		queue.emplace(222);

		std::cout << "myqueue contains:" << std::endl;
		while (false == queue.empty()) {
			std::cout << queue.front() << std::endl;
			queue.pop();
		}
	}

	void Pop_Test() {
		std::queue<Integer> myqueue;

		std::cout << "Please enter some integers (enter 0 to end):\n";
		int tmp_int;
		do {

			std::cin >> tmp_int;
			myqueue.emplace(tmp_int);
		} while (tmp_int);

		std::cout << "myqueue contains: ";
		while (false == myqueue.empty()) {
			std::cout << ' ' << myqueue.front();
			myqueue.pop();
		}
		std::cout << std::endl;
	}

	Integer&& get_int_from_q() {
		std::queue<Integer> queue;
		for (auto i : { 1,2,3,4,5,6,7,8,9 })
			queue.emplace(i);

		auto&& entry = queue.back();
		queue.pop();
		return std::move(entry);
	}

	void Pop_Test_NoCopy() {
		auto integer = get_int_from_q();
		integer.printInfo();
	}

	void Swap_Test()
	{
		{
			std::queue<int> foo, bar;

			foo.push(10);
			foo.push(20);
			foo.push(30);

			bar.push(111);
			bar.push(222);

			std::cout << "size of foo: " << foo.size() << std::endl;
			std::cout << "size of bar: " << bar.size() << std::endl;

			foo.swap(bar);

			std::cout << "size of foo: " << foo.size() << std::endl;
			std::cout << "size of bar: " << bar.size() << std::endl;
		}
		std::cout << "----------------------------- Test2 ---- " << std::endl;
		{
			std::queue<int> foo, bar;
			foo.push(10);
			foo.push(20);
			foo.push(30);

			std::cout << "size of foo: " << foo.size() << std::endl;
			std::cout << "size of bar: " << bar.size() << std::endl;

			foo.swap(bar);

			std::cout << "size of foo: " << foo.size() << std::endl;
			std::cout << "size of bar: " << bar.size() << std::endl;
		}
	}

	void FIFO_Test() {
		std::queue<Integer> queue;

		std::cout << "Filling queue:\n" << std::endl;
		for (int i = 1; i <= 5; i++) {
			queue.emplace(i * 10);
		}

		std::cout << "\nExtracting elements from queue:\n" << std::endl;
		while (false == queue.empty()) {
			std::cout << queue.front() << ".   ";
			queue.pop();
		}
	}

	void TESTSS() {
		std::queue<std::string> strings;
		for (std::string str : { "One","Two", "Thre", "For", "Five" })
			strings.push(str);

		while (false == strings.empty()) {
			std::cout << strings.front() << "  ";
			strings.pop();
		}
	}
}

void Queue::TestAll() {
	// Construct();

	// Empty();

	// Front_Back();

	// Emplace();

	// Emplace2();

	// Pop_Test();
	Pop_Test_NoCopy();

	// Swap_Test();

	// FIFO_Test();

	// TESTSS();
}