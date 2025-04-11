/**============================================================================
Name        : Iterators.cpp
Created on  : 20.04.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <string>
#include <fstream>
#include <string_view>
#include <algorithm>
#include <iterator>
#include <functional>
#include <numeric>
#include <vector>
#include <memory>
#include <deque>
#include <map>
#include <list>
#include <cassert>
#include "../Helpers/Helpers.h"

#include "Iterators.h"
#include "Custom_Iterators.h"

namespace {
	using Helpers::Integer;
}

namespace Iterators::ReverseIterators
{

	void ReverseString()
	{
		std::string number_string = "123456789";
		{
			std::reverse_iterator<std::string::iterator> r_iter = number_string.rbegin();
			std::string rev(r_iter, number_string.rend());
			std::cout << rev << std::endl;
		}
		std::cout << "\n------------------- Modify ----------------\n" << std::endl;
		{
			std::reverse_iterator<std::string::iterator> r_iter = number_string.rbegin();

			r_iter[4] = '6'; // replaces '5' with '6'

			std::string rev(r_iter, number_string.rend());
			std::cout << rev << std::endl;
		}
	}

	void ReverseIterator_Vector()
	{
		std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

		std::cout << "----------------------------Test1\n" << std::endl;
		{
			std::sort(numbers.begin(), numbers.end());
			std::copy(numbers.begin(), numbers.end(), std::ostream_iterator<int>(std::cout, ", "));

			std::cout << std::endl;

			std::copy(std::make_reverse_iterator(numbers.end()),
		  			  std::make_reverse_iterator(numbers.begin()),
				      std::ostream_iterator<int>(std::cout, ", "));
		}

		std::cout << "\n\n----------------------------Test2" << std::endl;
		{
			std::reverse_iterator<std::vector<int>::iterator> rev_until(numbers.begin());
			std::reverse_iterator<std::vector<int>::iterator> rev_from(numbers.end());

			// or
			// std::reverse_iterator<std::vector<int>::iterator> rev_from = numbers.rbegin();
			// std::reverse_iterator<std::vector<int>::iterator> rev_until = numbers.rend();


			std::cout << "myvector:";
			while (rev_from != rev_until)
				std::cout << ' ' << *rev_from++;
			std::cout << std::endl;
		}


		std::cout << "\n\n----------------------------Test3" << std::endl;
		{
			std::copy(numbers.rbegin() , numbers.rend(), std::ostream_iterator<int>(std::cout, ", "));
		}

		std::cout << "\n\n----------------------------Test4" << std::endl;
		{
			for (auto it = numbers.rbegin(); numbers.rend() != it; ++it) 
				std::cout << *it << ", ";
			std::cout << std::endl;
		}
	}

	void ReverseIterator_Vector2()
	{
		std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

		std::vector<int>::iterator end = numbers.end();
		std::vector<int>::iterator begin = numbers.begin();
		while (end != begin)
			std::cout << ' ' << *(--end);
		std::cout << std::endl;
	}

	void ReverseIterator_Map()
	{
		{
			std::map<std::string, int> mapOfWordCount = { { "aaa", 10 }, { "ddd", 11 }, { "bbb", 12 }, { "ccc", 13 } };
			std::map<std::string, int>::reverse_iterator it = mapOfWordCount.rbegin();

			// Iterate over the map using Iterator till beginning.
			while (it != mapOfWordCount.rend()) {
				std::string word = it->first;
				int count = it->second;
				std::cout << word << " --> " << count << std::endl;
				it++;
			}
		}
	}
}

namespace Iterators::Base
{
	void Test1() {
		std::vector<int> v;
		v.reserve(5);

		for (int i = 1; i <= 5; ++i)
			v.push_back(i);

		std::vector<int>::reverse_iterator rev_iter = std::find(v.rbegin(), v.rend(), 3);
		std::cout << "*rev_iter  = " << *rev_iter << std::endl;

		std::vector<int>::iterator iter(rev_iter.base());
		std::cout << "*iter  = " << *iter << std::endl;
	}

	void Simple_Test() {
		std::vector<int> myvector;
		for (int i = 0; i < 10; i++) 
			myvector.push_back(i);

		auto rev_begin = myvector.rbegin();
		auto rev_end = myvector.rend();


		for (auto it = rev_begin; it != rev_end; ++it)
			std::cout << ' ' << *it;
		std::cout << std::endl;

		// Using base os Reverse_interator --> Normal interator

		for (auto it = rev_end.base(); it != rev_begin.base(); ++it)
			std::cout << ' ' << *it;
		std::cout << std::endl;
	}
}

namespace Iterators
{
    void FrotInserver() {
        {
            std::vector<int> numbers{1, 2, 3, 4, 5};
            std::deque<int> d;

            std::copy(numbers.begin(), numbers.end(), std::ostream_iterator<int>(std::cout, ", "));
            std::cout << std::endl;

            std::copy(numbers.begin(), numbers.end(), std::front_inserter(d));
            for (int n: d)
                std::cout << n << ", ";
        }
        std::cout << "\n-------------------------Test1" << std::endl;
        {
            std::deque<int> numbers = {1, 2, 3, 4, 5};
            std::cout << "Before: ";
            for (int n: numbers)
                std::cout << n << ' ';

            std::fill_n(std::front_inserter(numbers), 5, 0);

            std::cout << "\nAfter : ";
            for (int n: numbers)
                std::cout << n << ' ';
        }
        std::cout << "\n-------------------------Test2" << std::endl;
        {
            std::deque<int> numbers;
            std::cout << "Before: ";
            for (int n: numbers)
                std::cout << n << ' ';

            std::fill_n(std::front_inserter(numbers), 5, 0);

            std::cout << "\nAfter : ";
            for (int n: numbers)
                std::cout << n << ' ';
        }
    }

    void Back_Insert_Iterator() {
        std::vector<int> foo, bar;
        for (int i = 1; i <= 5; i++) {
            foo.push_back(i);
            bar.push_back(i * 10);
        }

        std::back_insert_iterator<std::vector<int>> back_it(foo);
        std::copy(bar.begin(), bar.end(), back_it);

        std::cout << "foo:";
        std::for_each(foo.begin(), foo.end(), [](const auto &v) -> void { std::cout << v << " "; });
        std::cout << std::endl;
    }

    void Back_Insert() {
        std::vector<int> numbers = {1, 1, 1, 1, 1};

        std::cout << "Before: ";
        for (int n: numbers)
            std::cout << n << ' ';

        std::fill_n(std::back_inserter(numbers), 3, 5);

        std::cout << "\nAfter : ";
        for (int n: numbers)
            std::cout << n << ' ';
    }

    void Front_Insert_Iterator() {
        {
            std::deque<int> foo, bar;
            for (int i = 1; i <= 5; i++) {
                foo.push_back(i);
                bar.push_back(i * 10);
            }

            std::front_insert_iterator<std::deque<int> > front_it(foo);
            std::copy(bar.begin(), bar.end(), front_it);

            std::cout << "foo:";
            std::for_each(foo.begin(), foo.end(), [](const auto &v) -> void { std::cout << v << " "; });
            std::cout << std::endl;
        }
        std::cout << "\n\nTEST2:" << std::endl;
        {
            std::vector<int> v{1, 2, 3, 4, 5};
            std::deque<int> d;
            std::copy(v.begin(), v.end(), std::front_insert_iterator<std::deque<int>>(d)); // or std::front_inserter(d)
            for (int n: d)
                std::cout << n << ' ';
            std::cout << std::endl;
        }
    }

    void Advance() {
        {
            std::vector<int> v{3, 1, 4};
            auto vi = v.begin();
            std::advance(vi, 2);
            std::cout << *vi << std::endl;
        }
        {
            std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9};
            auto vi = v.begin();
            std::advance(vi, 6);
            std::cout << *vi << std::endl;
        }
    }
}

namespace Iterators
{
	void Distance() {
		const std::vector<int> numbers = { 3, 1, 4, 3, 7, 1, 9, 33};
		std::cout << "distance(first, last) = "
				  << std::distance(numbers.begin(), numbers.end()) << '\n'
				  << "distance(last, first) = "
				  << std::distance(numbers.end(), numbers.begin()) << '\n';

	}
}

namespace Iterators
{
	void Prev() {
		const std::vector<int> numbers = {0,1,2,3,4,5,6,7,8,9};
		auto iter = numbers.end();
		auto pv = std::prev(iter, 2);
		std::cout << *pv << std::endl;

		pv = std::prev(pv, 2);
		std::cout << *pv << std::endl;

		pv = std::prev(pv, 2);
		std::cout << *pv << std::endl;
	}
}

namespace Iterators
{
	void Next() {
		const std::vector<int> numbers = { 0,1,2,3,4,5,6,7,8,9 };
		auto iter = numbers.begin();
		do {
			std::cout << "std::next(iter, 2) = " << *iter << std::endl;
		} while (numbers.end() != (iter = std::next(iter, 3)));
	}

	void Next2() {
		std::vector<int> numbers = { 0,1,2,3,4,5,6,7,8,9 };
		std::vector<int>::iterator iter = numbers.begin();
		std::cout << *std::next(iter) << std::endl;
		std::cout << *iter << std::endl;
	}
}

namespace Iterators
{
	void Size() {
		{
			std::vector<int> v = { 3, 1, 4 };
			std::cout << std::size(v) << std::endl;

			int a[] = { -5, 10, 15,1,2,2,3,3 };
			std::cout << std::size(a) << std::endl;
		}
	}
}

namespace Iterators::Files
{

	void ReadFile()
    {
		std::ifstream input_file_stream(R"(S:\Temp\Folder_For_Testing\COPY\Input.txt)");
		std::string text;
		text.reserve(1024 * 1024);

		input_file_stream.unsetf(std::ios::skipws);
		std::copy(std::istream_iterator<char>(input_file_stream),
				  std::istream_iterator<char>(),
				  std::back_inserter(text));
		std::cout << text << std::endl;
	}

	
	void ReadFile1() {
		/*
		std::ifstream input_file_stream("S:\\Temp\\Folder_For_Testing\\COPY\\Input.txt");
		std::string text(std::istreambuf_iterator<char>(input_file_stream), std::istreambuf_iterator<char>());
		std::cout << text << std::endl;
		*/
	}
	

	void ReadFile2() {
		std::ifstream input_file_stream(R"(S:\Temp\Folder_For_Testing\COPY\Input.txt)");
		input_file_stream.unsetf(std::ios::skipws);
		std::string text((std::istream_iterator<char>(input_file_stream)), std::istream_iterator<char>());

		std::cout << text << std::endl;
	}

	void ReadFile3() {
		std::shared_ptr<std::ifstream> file_stream_ptr = std::make_shared<std::ifstream>(R"(S:\Temp\Folder_For_Testing\COPY\Input.txt)");
		std::string text(std::istreambuf_iterator<char>(*file_stream_ptr.get()),std::istreambuf_iterator<char>());
		std::cout << text << std::endl;
	}

	void CopyFile() {
		std::ifstream input_file_stream(R"(S:\Temp\Folder_For_Testing\COPY\Input.txt)");
		std::ofstream outnput_file_stream(R"(S:\Temp\Folder_For_Testing\COPY\Output.txt)");
		std::copy(std::istream_iterator<std::string>(input_file_stream),
				  std::istream_iterator<std::string>(),
			      std::ostream_iterator<std::string>(outnput_file_stream, "\n"));
	}

	void CopyFile2()
    {
		std::ifstream  src(R"(S:\Temp\Folder_For_Testing\COPY\Input.txt)", std::ios::binary);
		std::ofstream  dst(R"(S:\Temp\Folder_For_Testing\COPY\Output.txt)", std::ios::binary);
		dst << src.rdbuf();
	}
}

namespace Iterators
{
    void Move_Iterator_0()
    {
        std::vector<std::unique_ptr<Integer>> src;
        std::generate_n(std::back_inserter(src), 5, []{
            return std::make_unique<Integer>();
        });

        // Wouldn't compile: incompatible type
        // std::list<std::unique_ptr<MyClass>> dst1(std::move(src));

        // Wouldn't compile: std::unique_ptr is not copyable
        // std::list<std::unique_ptr<MyClass>> dst2(src.begin(), src.end());

        // OK
        std::list<std::unique_ptr<Integer>> dst3(std::move_iterator(src.begin()),std::move_iterator(src.end()));
    }



    void Move_Iterator() {
        std::vector<std::string> strings = {"this", "is", "an", "example"};
        std::for_each(strings.begin(), strings.end(), [](const auto &str) -> void { std::cout << '"' << str << "\" "; });

        using iter_t = std::vector<std::string>::iterator;
        std::string concat = std::accumulate(std::move_iterator<iter_t>(strings.begin()),
                                             std::move_iterator<iter_t>(strings.end()),
                                             std::string());  // Can be simplified with std::make_move_iterator

        std::cout << "\nConcatenated as string: " << concat << '\n' << "New contents of the vector: ";

        std::for_each(strings.begin(), strings.end(), [](const auto &str) -> void { std::cout << '"' << str << "\" "; });
        std::cout << std::endl;
    }
}

namespace Iterators
{
    void print(const std::string_view remark, const std::vector<std::string>& v)
    {
        const long size = std::ssize(v);
        std::cout << remark << '[' << size << "] { ";
        for (auto it = std::counted_iterator{std::cbegin(v), size}; it != std::default_sentinel; ++it)
            std::cout << *it << ", ";
        std::cout << "}\n";
    }

    void test_print()
    {
        const std::vector<std::string> values {"One", "Two", "Three", "Four", "Five"};
        print("src", values);
        std::vector<decltype(values)::value_type> dst;
        std::ranges::copy(std::counted_iterator{values.begin(), 3},
                          std::default_sentinel,
                          std::back_inserter(dst));
        print("dst", dst);
    }


    void test_simple()
    {
        const std::vector<std::string> values {"One", "Two", "Three", "Four", "Five"};
        for (auto iter = std::counted_iterator{values.begin(), 3}; std::default_sentinel != iter; ++iter)
        {
            std::cout << *iter << ' ';
        }
        std::cout << '\n';
    }
}

namespace Iterators::MoveIterator
{
    using Helpers::Integer;

    std::vector<std::unique_ptr<Integer>> get_elements()
    {
        std::vector<std::unique_ptr<Integer>> ints;
        ints.emplace_back(std::make_unique<Integer>(111));
        ints.emplace_back(std::make_unique<Integer>(222));
        ints.emplace_back(std::make_unique<Integer>(333));
        return ints;
    }

    void Test()
    {
        std::vector<std::unique_ptr<Integer>> src = get_elements();

        std::cout << "src: ";
        for (const auto& var: src)
            std::cout << var->value << ' ';
        std::cout << std::endl;


        /** Wouldn't compile: incompatible type **/
        // std::list<std::unique_ptr<Integer>> err1(std::move(src));

        /** Wouldn't compile: std::unique_ptr is not copyable **/
        // std::list<std::unique_ptr<Integer>> err2(src.begin(), src.end());

        // OK
        const std::list<std::unique_ptr<Integer>> dst(
                std::move_iterator(src.begin()),
                std::move_iterator(src.end())
        );

        std::cout << "dst: ";
        for (const auto& var: dst)
            std::cout << var->value << ' ';
        std::cout << std::endl;
    }
}

void Iterators::TestAll()
{
    Custom_Iterators::TestAll();

	// ReverseIterators::ReverseString();
	// ReverseIterators::ReverseIterator_Vector();
	// ReverseIterators::ReverseIterator_Vector2();
	// ReverseIterators::ReverseIterator_Map();

    // CountingIterator::test_simple();
    // CountingIterator::test_print();

	// FrotInserver();
	// Front_Insert_Iterator();

	// Move_Iterator_0();
	// Move_Iterator();

	// Base::Test1();
	// Base::Simple_Test();

	// Back_Insert();
	// Back_Insert_Iterator();

	// Advance();
	// Prev();
	// Next();
	// Next2();
	// Distance();

	// Size();

	// Files::ReadFile();
	// Files::ReadFile2();
	// Files::ReadFile3();
	// Files::CopyFile();
	// Files::CopyFile2();

    // MoveIterator::Test();

}
