//============================================================================
// Name        : StringViewTests.cpp
// Created on  : 24.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : StringViewTests C++  src
//============================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <chrono>
#include <charconv>
#include <array>

#include "../Helpers/Helpers.h"
#include "StringViewTests.h"



namespace StringView {
	std::string_view askForName()
	{
		std::cout << "What's your name?\n";

		std::string str{};
		std::cin >> str;

		std::string_view view{ str };

		std::cout << "Hello " << view << '\n';

		return view;
	} // str get destroy
};

namespace StringView {

	class Constants {
	public:
		constexpr static std::string_view getName() {
			return "Some_Constat_Name";
		}
	};

	void Test() {
		std::string str = "lllloooonnnngggg sssstttrrriiinnnggg"; //A really long string

		//Bad way - 'string::substr' returns a new string (expensive if the string is long)
		std::cout << "std::string value: " << str.substr(0, 16) << std::endl;

		//Good way - No copies are created!
		std::string_view view = str;

		// string_view::substr returns a new string_view
		std::cout << "std::string_view value: " << view.substr(0, 16) << std::endl;
	}

	void Create() {
		{
			char chars[] = { 'a', 'e', 'i', 'o', 'u' };
			std::string_view str { chars, std::size(chars) };

			std::cout << str << std::endl;
		}
	}

	void ToString() {
		const auto print_str = [](const std::string& text)-> void {
			std::cout << text << std::endl;
		};

		std::string_view sv{ "balloon" };
		sv.remove_suffix(3);
		std::string str{ sv };
		print_str(str);
		print_str(static_cast<std::string>(sv));
	}

	void ReturnSrting()
	{
		const auto get_str_viw = []()-> std::string_view {
			std::string str = "ETETETET";
			return std::string_view(str);
		};

		std::cout  << get_str_viw() << std::endl;
	}

	void Create_2() {
		std::string_view view{ askForName() };
		std::cout << "Your name is " << view << std::endl;
	}

	void Basic_Tests()
	{
		std::string_view stringView{ "Trains are fast!" };

		// Length.
		// Result: 16
		std::cout << stringView.length() << std::endl;

		// Result: Trains
		std::cout << stringView.substr(0, stringView.find(' ')) << std::endl;

		// Result: 1
		std::cout << (stringView == "Trains are fast!") << std::endl;

		// Since C++20
		// std::cout << str.starts_with("Boats") << '\n'; // 0
		// std::cout << str.ends_with("fast!") << '\n'; // 1

		// Result: Trains are fast!
		std::cout << stringView << std::endl;

		std::cout << "empty: " << stringView.empty() << std::endl;

		stringView.remove_prefix(7);
		std::cout << "'remove_prefix(7)' result: " << stringView << ".   Length: " << stringView.length() << std::endl;

		stringView.remove_suffix(5);
		std::cout << "'remove_suffix(5)' result: " << stringView << ".   Length: " << stringView.length() << std::endl;

	}

	void  Basic_Tests_2() {

		char arr[] { "Gold" };
		std::string_view str{ arr };

		std::cout << str << std::endl;
		arr[3] = 'f';
		std::cout << str << std::endl;
	}

	void worker_str(const std::string& str)
	{

		const int len = str.length();
		std::cout << len << std::endl;
	}

	void  worker_str_view(const std::string_view str)
	{
		const int len = str.length();
		std::cout << len << std::endl;
	}

	void PerformanceTest()
	{
		const std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

		constexpr char text[] = "Some_test_value";
		//std::this_thread::sleep_for(std::chrono::milliseconds(125));

		for (int i = 0; i < 10000; i++) {
			for (int n = 0; n < 10000; n++) {
				// StringView_Tests::worker_str(text);
				StringView::worker_str_view(text);
			}
		}

		const std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
		const double duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start).count();
		std::cout << "Execution time: " << duration << std::endl;
	}

	template<class Type>
	Type atoi_17(std::string_view str) {
		Type res{};
		std::from_chars(str.data(), str.data() + str.size(), res);
		return res;
	}


	void GlobalConstVar() {
		std::cout << Constants::getName() << std::endl;
	}

	void Various_Tests() {
		// std::literals::string_view_literals::operator""sv;
		using namespace std::literals;

		std::string_view s1 = "abc\0\0def";
		std::string_view s2 = "abc\0\0def"sv;
	}
};

namespace StringView::UsageExamples 
{
	using namespace std::literals;

	void Compare_Part_Of_String()
	{
		const std::string str1 = "SomeRandomText_1234";
		const std::string str2 = "SomeRandomText_4321";

		constexpr size_t offset = ("SomeRandomText_"sv).size();
		constexpr size_t TESTS_COUNT = 100'000'000;	

		{
			const auto start = std::chrono::high_resolution_clock::now();
			for (size_t i = 0; i < TESTS_COUNT; ++i) {
				auto x1 = str1.substr(offset, str1.length() - offset);
				auto x2 = str2.substr(offset, str1.length() - offset);
				auto result = x1.compare(x2);
			}
			const auto end = std::chrono::high_resolution_clock::now();
			const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			std::cout << "Result: " << duration << " microseconds" << std::endl;
		}

		{
			const auto start = std::chrono::high_resolution_clock::now();
			for (size_t i = 0; i < TESTS_COUNT; ++i) {
				const auto x1 = std::string_view(str1).substr(offset, str1.length() - offset);
				const auto x2 = std::string_view(str2).substr(offset, str1.length() - offset);
				auto result = x1.compare(x2);
			}
			const auto end = std::chrono::high_resolution_clock::now();
			const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			std::cout << "Result: " << duration << " microseconds" << std::endl;
		}
	}

	void Atoi_Partial() {
		const std::string strNumber = "123456789";
		constexpr size_t TESTS_COUNT = 100'000'000;

		{
			auto start = std::chrono::high_resolution_clock::now();
			for (size_t i = 0; i < TESTS_COUNT; ++i) {
				int v = atoi(strNumber.substr(3, 4).data());
			}
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			std::cout << "Result: " << duration << " microseconds" << std::endl;
		}

		{
			auto start = std::chrono::high_resolution_clock::now();
			for (size_t i = 0; i < TESTS_COUNT; ++i) {
				int v = atoi(std::string_view(strNumber).substr(3, 4).data());
			}
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
			std::cout << "Result: " << duration << " microseconds" << std::endl;
		}
	}
}

namespace StringView::Tests {

	void Test_Append() {
		std::string text { "qwerty" };

		auto sv = std::string_view { text };

		auto s1 = sv.substr(0, 2);
		auto s2 = sv.substr(3, 5);
		// auto s3 = std::string_view( s1.data() );

		std::cout << s1 << std::endl;
		std::cout << s2 << std::endl;
	}

    void Contains()
    {
        std::string_view text { "123_456_789" };

        std::cout << std::boolalpha << text.contains("456") << std::endl;
    }
}


void StringView::TestAll()
{
	// number_parser::TestAll();
	failure_cases::TestAll();

	// Create();
	// Create_2();
	// Test();
	// Basic_Tests();
	// Basic_Tests_2();
	// ToString();
	// ReturnSrting();

    // Tests::Contains();

	// Various_Tests();

	// GlobalConstVar();

	// UsageExamples::Compare_Part_Of_String();
	// UsageExamples::Atoi_Partial();

	// Tests::Test_Append();

	// const std::string rootFolder { R"(/home/andtokm/mlpack/tests/mlpack/src/data/)" };
	// const std::string filePath{ rootFolder  + "  "};
}