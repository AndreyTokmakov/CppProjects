//============================================================================
// Name        : Stringstream.cpp
// Created on  : 28.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Stringstream src
//============================================================================

#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <string_view>
#include <iterator>


#include <deque>
#include <algorithm>
#include <functional>

#include "../Helpers/Helpers.h"
#include "StringStreamTests.h"


namespace StringStream
{
	template <class Container>
	void split_string(const std::string& str, Container& cont, char delim = ' ') {
		std::stringstream ss(str);
		std::string token;
		while (std::getline(ss, token, delim)) {
			cont.emplace_back(token);
		}
	}

	template <class Container>
	void split_string_2(const std::string& str, Container& cont) {
		std::istringstream iss(str);
		std::copy(std::istream_iterator<std::string>(iss),
				  std::istream_iterator<std::string>(),
				  std::back_inserter(cont));
	}
}

/*
void* operator new(std::size_t sz){
    std::cout << "Allocating: " << sz << '\n';
    return std::malloc(sz);
}
*/

namespace StringStream
{
	void Create_Tests()
	{
		std::cout << "Test1" << std::endl;
		{
			std::stringstream myString;
			myString << "Lorem ipsum!" << std::endl;
			std::cout << myString.str();
		}

		std::cout << "\nTest2" << std::endl;
		{
			std::stringstream myString;
			myString.str("Lorem ipsum!");
			std::cout << myString.str() << std::endl;
		}

		std::cout << "\nTest3" << std::endl;
		{
			std::stringstream myString;
			myString << "336000 12.14" << std::endl;
			std::cout << myString.str() << std::endl;
		}

		std::cout << "\nTest4" << std::endl;
		{
			std::stringstream myString;
			myString << "336000 12.14"; 

			std::string part1;
			myString >> part1;

			std::string part2;
			myString >> part2;

			std::cout << part1 << " and " << part2 << std::endl;
		}
	}

	void Handle_Numbers() {
		{
			std::stringstream str_stream;

			int nValue = 336000;
			double dValue = 12.14;
			str_stream << nValue << " " << dValue;

			std::string str1, str2;
			str_stream >> str1 >> str2;

			std::cout << str1 << " " << str2 << std::endl;
		}
		std::cout << "\nTest2:\n" << std::endl;
		{
			std::stringstream str_stream;
			str_stream << "336000 12.14";
			int nValue;
			double dValue;

			str_stream >> nValue >> dValue;

			std::cout << nValue << " " << dValue << std::endl;
		}
		std::cout << "\nTest3:\n" << std::endl;
		{
			std::stringstream str_stream;
			str_stream << "1 2 3 4 5";
			int val;

			while (str_stream >> val)
				std::cout << val << std::endl;
		}
		std::cout << "\nTest4:\n" << std::endl;
		{
			std::stringstream str_stream("1 2 3 4 5");
			int val;
			while (str_stream >> val)
				std::cout << val << std::endl;
		}
	}

	void Clear() {
		{
			std::stringstream str_stream;
			str_stream << "Hello ";

			std::cout << str_stream.str();
			str_stream.str("");
			str_stream.clear();

			str_stream << "World!" << std::endl;
			std::cout << str_stream.str();
		}
	}

	void SplitString_Simple_Spaces() {
		const std::string text = "AAAA BBBB CCCC DDDD";

		std::cout << "------ Test1:\n" << std::endl;
		{
			std::stringstream stream(text);
			std::string tmp;
			while (stream >> tmp) 
				std::cout << tmp << std::endl;
		}

		std::cout << "------ Test11:\n" << std::endl;
		{
			std::stringstream stream(text);
			for (std::string str; stream >> str; )
				std::cout << str << std::endl;
		}

		std::cout << "------ Test2:\n" << std::endl;
		{
			std::stringstream iss(text);
			std::for_each(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), [](const std::string& str) {
				std::cout << str << std::endl;
			});
		}

		std::cout << "------ Test3:\n" << std::endl;
		{
			std::stringstream iss(text);
			std::vector<std::string> parts(std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>());
			std::for_each(parts.begin(), parts.end(), [](const std::string& str) {std::cout << str << std::endl; });
		}

		std::cout << "------ Test4:\n" << std::endl;
		{
			std::stringstream iss(text);
			std::vector<std::string> parts;
			parts.assign(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>());
			std::for_each(parts.begin(), parts.end(), [](const std::string& str) {std::cout << str << std::endl; });
		}
	}

	void Spilit_Test_Delimiter() {
		std::cout << "Test (' ' delimiter):" << std::endl;
		{
			const std::string text = "AAAA BBBB CCCC DDDD";
			std::stringstream stream(text);
			std::string part;
			while (std::getline(stream, part, ' ')) {
				std::cout << part << std::endl;
			}
		}
		std::cout << "\nTest (':' delimiter):" << std::endl;
		{
			const std::string text = "AAAA:BBBB:CCCC:DDDD";
			std::stringstream stream(text);
			std::string part;
			while (std::getline(stream, part, ':')) {
				std::cout << part << std::endl;
			}
		}
	}

	void SplitStringTests() {
		{
			const std::string text = "1 22 333 4444 55555";
			std::vector<std::string> parts;
			split_string(text, parts);
			std::for_each(parts.begin(), parts.end(), [](const std::string& s) { std::cout << s << std::endl; });
		}
		std::cout << "\n====================================================" << std::endl;
		{
			const std::string text = "1 22 333 4444 55555";
			std::vector<std::string> parts;
			split_string_2(text, parts);
			std::for_each(parts.begin(), parts.end(), [](const std::string& s) { std::cout << s << std::endl; });
		}
	}


	void Count_Words() {
		const auto counter = [](const std::string& str)	{
			std::stringstream string_stream(str);
			std::string word; 

			int count = 0;
			while (string_stream >> word)
				count++;
			return count;
		};

		std::cout << " Number: " << counter("geeks for geeks geeks contribution placements");
	}

	void Print_Frequency() {
		const auto printFrequency = [](const std::string& str) {
			std::map<std::string, int> dict;
			std::stringstream string_stream(str); 
			std::string word; 

			while (string_stream >> word)
				dict[word]++;
			for (auto &[key, value] : dict)
				std::cout << key << "  ->  " << value << std::endl;
		};

		printFrequency("Geeks For Geeks Quiz Geeks Quiz Practice Practice");
	}

	void Swap() {
		std::stringstream foo, bar;

		foo << "Test1";
		bar << "Test2";

		std::cout << "foo: " << foo.str() << std::endl;
		std::cout << "bar: " << bar.str() << std::endl;

		foo.swap(bar);

		std::cout << "foo: " << foo.str() << std::endl;
		std::cout << "bar: " << bar.str() << std::endl;
	}

	void Test() {
		std::string str = "11 22 33 44 55";
		std::istringstream iss(str);

		// std::deque<std::string> strings;
		// std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::front_inserter(strings));

		std::vector<std::string> strings;
		std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(strings));

		for (const auto& str : strings)
			std::cout << str << std::endl;
	}


	void Test2()
    {
		{
			std::string str = "11:22:33:44:55";
			std::stringstream ss(str);
			std::string token;
			while (std::getline(ss, token, ':')) {
				std::cout << token << " ";
			}
			std::cout << std::endl;
		}
		{
			std::string str = "1 2 3 4 5";
			std::stringstream ss(str);
			int val;
			while (ss >> val)
				std::cout << val << " ";
			std::cout << std::endl;
		}
	}



    void View()
    {
        std::cout << "start...\n";
        std::stringstream str;
        str << 42;
        str << " Hello C++20/23 Programming World";

        std::cout << "str(1)\n";
        std::cout << str.str() << '\n';

        std::cout << "str(2)\n";
        std::cout << str.str() << '\n';

        std::cout << "view()\n";
        std::cout << str.view() << '\n';
    }
};


void StringStream::TestAll()
{
    // Test();
    // Test2();

    // Create_Tests();
    // Handle_Numbers();
    // Swap();
    // Clear();

    View();

    // Count_Words();
    // Print_Frequency();

    // Spilit_Test_Delimiter();
    // SplitStringTests();
    // SplitString_Simple_Spaces();
};