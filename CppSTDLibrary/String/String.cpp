/**============================================================================
Name        : String.cpp
Created on  : 30.04.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : String.cpp
============================================================================**/

#define _CRT_SECURE_NO_WARNINGS


#include <charconv>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <cctype>
#include <algorithm>
#include <functional>
#include <algorithm>
#include <numeric>
#include <iterator>
#include <functional>
#include <iostream>
#include <vector>
#include <chrono>
#include <iterator>
#include <regex>
#include <array>
#include <iomanip>

#include "String.h"
#include "StringUtilities.h"
#include "PerfUtilities.hpp"


namespace String
{
	void Create_Test() {

		std::string s0("Initial string");

		// constructors used in the same order as described above:
		std::string s1;
		std::string s2(s0);
		std::string s3(s0, 8, 6);
		std::string s4("A character sequence");
		std::string s5("Another character sequence", 12);
		std::string s7(10, 'x');
		std::string s8(10, 42);      // 42 is the ASCII code for '*'
		std::string s9(s0.begin(), s0.begin() + 7);

		std::cout << "s1: " << s1 << std::endl;
		std::cout << "s2: " << s2 << std::endl;
		std::cout << "s3: " << s3 << std::endl;
		std::cout << "s4: " << s4 << std::endl;
		std::cout << "s5: " << s5 << std::endl;
		std::cout << "s7: " << s7 << std::endl;
		std::cout << "s8: " << s8 << std::endl;
		std::cout << "s9: " << s9 << std::endl;
	}

	void Create_FromStrings() {
		std::string s1("One");
		std::string s2("Two");

		std::string str2 = std::string(s1).append(s2);
		std::cout << str2 << std::endl;
	}

	void Create_FromStrings2() {
		std::string src { "0123456789" };
		std::string dst { src, 2, 3 };

		std::cout << src << std::endl;
		std::cout << dst << std::endl;
	}

	void Vector_of_String() {
		std::vector<std::string> texts;

		texts.emplace_back("11111");
		texts.emplace_back(10, 'a');

		std::for_each(texts.cbegin(), texts.cend(), [](const auto& s) { std::cout << s << std::endl; });
	}

	void Itearate()
	{
		std::string str("Test string");
		for (std::string::iterator it = str.begin(); it != str.end(); ++it)
			std::cout << *it;
		std::cout << std::endl;
	}

	void ItearateBackward_RbeginRend()
	{
		std::string str("now step live");
		for (std::string::reverse_iterator rit = str.rbegin(); rit != str.rend(); ++rit)
			std::cout << *rit;
	}

	void Crbegin_Crend_Test()
	{
		std::string str("123 456 789");
		for (auto rit = str.crbegin(); rit != str.crend(); ++rit)
			std::cout << *rit;
		std::cout << std::endl;
	}

	void Append()
	{
		std::string str;
		std::string str2 = "Writing ";
		std::string str3 = "print 10 and then 5 more";


		str.append(str2);                       // "Writing "
		std::cout << str << std::endl;

		str.append(str3, 6, 3);                   // "10 "
		std::cout << str << std::endl;

		str.append("dots are cool", 5);          // "dots "
		std::cout << str << std::endl;

		str.append("here: ");                   // "here: "
		std::cout << str << std::endl;

		str.append(10u, '.');                    // ".........."
		std::cout << str << std::endl;

		str.append(str3.begin() + 8, str3.end());  // " and then 5 more"
		std::cout << str << std::endl;

		// str.append<int>(5, 0x2E);                // "....."

		std::cout << str << std::endl;
	}

	void Append2()
	{
		std::string str;
		str.append("1").append("2").append("3").append("4").append("5").append("6");

		std::cout << str << std::endl;
	}

	void StartsWith()
	{
		std::string str{ "232323" };
		std::cout << std::boolalpha << str.starts_with("#") << std::endl;
	}

	void ConcatStrings() {

		using namespace std::string_literals;

		std::string str1 = "Hello";
		std::string text = str1 + " World";

		std::cout << text << std::endl;

		const char kBrowserUIScheme[] = "browser";
		std::string str12(std::string(kBrowserUIScheme) + "s");

		std::cout << str12 << std::endl;


		std::string str3 = kBrowserUIScheme + " World"s;
		std::cout << str3 << std::endl;
	}

	void PushBack()
	{
		std::string str = "Initial";
		std::cout << str << std::endl;

		for (auto& c : " 12345")
			str.push_back(c);

		std::cout << str << std::endl;
	}

	void PopBack()
	{
		const std::string base = "AAAA.BBBB.CCCC.DDDD.AAAA.DDDD.CCCC.AAAA.XXXX";
		std::cout << "Sample string: " << base << std::endl;
		std::cout << "str.length() = " << base.length() << std::endl;

		std::cout << "\n --------------------- TEST1:" << std::endl << std::endl;
		{
			std::string str(base);
			std::cout << str << std::endl;
			const auto size = str.length();
			for (size_t i = 1; i < size; i++)
			{
				str.pop_back();
				std::cout << str << std::endl;
			}
		}

		std::cout << "\n --------------------- TEST2:" << std::endl << std::endl;
		{
			std::string str(base);
			std::cout << str << std::endl;
			str.pop_back();
			std::cout << str << std::endl;
		}
	}

	void max_size()
	{
		std::string str("Test string");
		std::cout << "size: " << str.size() << "\n";
		std::cout << "length: " << str.length() << "\n";
		std::cout << "capacity: " << str.capacity() << "\n";
		std::cout << "max_size: " << str.max_size() << "\n";
	}

	void Replace() {
		{
			std::string base = "One Two Three";
			std::string src = "Two";
			std::string dst = "Six";

			base.replace(base.find(src), dst.length(), dst);
			std::cout << base << std::endl;
		}
		{
			std::string base = "One Two Three";
			std::string src = "Two";
			std::string dst = "Five";

			base.replace(base.find(src), dst.length(), dst);
			std::cout << base << std::endl;
		}
		{
			std::string base = "One Two Three";
			std::string src = "Three";
			std::string dst = "Eleven";

			base.replace(base.find(src), dst.length(), dst);
			std::cout << base << std::endl;
		}
	}

	void Replace2() {
		std::string path = R"(R:\Projects\Html\Tables\css-responsive-table-layout)";
		std::cout << path << std::endl;

		auto pos = 0;
		while (std::string::npos != (pos = path.find(R"(\)", pos))) {
			path[pos] = '/';
		}

		std::cout << path << std::endl;
	}

	void Swap_First_and_Last_Chars() {
		std::string str = "a11111b";

		/*
		std::cout << str << std::endl;
		char back = str.back();
		str.back() = str.front();
		str.front() = back;
		*/
		std::swap(str.front(), str.back());

		std::cout << str << std::endl;
	}

	void Remove() {
		{
			std::string str = "AAA.BBB.CCCC.DDD.";
			str.erase(str.length() - 1);
			size_t pos = str.find_last_of(".");

			if (std::string::npos != pos) {
				str.erase(pos + 1);
			}

			std::cout << str << std::endl;
		}
		std::cout << "\nRemove char\n" << std::endl;
		{
			std::string str = "01234556789";
			str.erase(str.begin() + 3);
			std::cout << str << std::endl;
		}
	}

	void Find_Tests() {
		const std::string str = "AAAA.BBBB.CCCC.DDDD.AAAA.DDDD.CCCC.AAAA.XXXX";
		std::cout << "Sample string: " << str << std::endl;
		std::cout << "str.length() = " << str.length() << std::endl;

		{
			const std::string to_find("BBBB");
			std::cout << "\nfind (" << to_find << ") = " << str.find(to_find) << std::endl;
		}

		{
			const std::string to_find("BBBB");
			std::cout << "\nfind (" << to_find << " 2) = " << str.find(to_find, 4) << std::endl;
		}

		{
			const std::string to_find("BBBB");
			std::cout << "\nfind (" << to_find << " 5 ) = " << str.find(to_find, 8) << std::endl;
		}

		{
			const std::string to_find("AABBCC");
			std::cout << "\nfind_first_of (" << to_find << ") = " << str.find_first_of(to_find) << std::endl;
		}

		{
			const std::string to_find("AABBCC");
			std::cout << "\nfind_last_of (" << to_find << ") = " << str.find_last_of(to_find) << std::endl;
		}

		{
			std::string to_find("AA");
			std::cout << "\nfind_first_not_of (" << to_find << ") = " << str.find_first_not_of(to_find) << std::endl;
			to_find = "BB";
			std::cout << "find_first_not_of (" << to_find << ") = " << str.find_first_not_of(to_find) << std::endl;
		}

		{
			std::string to_find("AA");
			std::cout << "\nfind_last_not_of (" << to_find << ") = " << str.find_last_not_of(to_find) << std::endl;
			to_find = "NN";
			std::cout << "find_last_not_of (" << to_find << ") = " << str.find_last_not_of(to_find) << std::endl;
		}
	}

	void Find_FirstOf() {
		const std::string str = "AAAA.BBBB.CCCC.DDDD.AAAA.DDDD.CCCC.AAAA.XXXX";
		std::cout << "Original string: " << str << "\n" << std::endl;

		{
			const std::string text("CCAABB");
			std::cout << "Find First for '" << text << "': ";

			auto pos = str.find_first_of(text);
			std::cout << pos << ". Substr: " << str.substr(pos, str.length() - pos) << std::endl;
		}
		{
			const std::string text("DXC");
			std::cout << "Find First for '" << text << "': ";

			auto pos = str.find_first_of(text);
			std::cout << pos << ".   Substr: " << str.substr(pos, str.length() - pos) << std::endl;
		}
	}

	void Insert() {
		std::string text = "abcc";
		std::string b_Str = "B";

		std::cout << text << std::endl;

		text.insert(std::next(text.begin()), b_Str.begin(), b_Str.end());

		std::cout << text << std::endl;
	}

	void Assign__Substr_vs_Iterators() {
		constexpr size_t start = 3, end = 7;

		{
			std::string text = "123456789";
			std::string dest;

			dest.assign(text.substr(start, end - start));
			std::cout << dest << std::endl;
		}


		{
			std::string text = "123456789";
			std::string dest;

			dest.assign(text.begin() + start, text.begin() + end);
			std::cout << dest << std::endl;
		}
	}

	void SubString() {
		{
			std::string text = "0123456789";
			std::cout << text.substr(1, 5) << std::endl;
		}
	}

	//--------------------------------------------------------------------------------------------------------------//

	void consume(std::string&& str) {
		// std::string part(std::move(str));
		// std::string part(str.begin(), str.begin() + 5);
		std::string part(str.data());

		std::cout << "consume  : " << part << std::endl;
	}

	void SubString_Move() {
		std::string text = "0123456789";

		std::cout << "original : " << text << std::endl;

		consume(std::move(text));

		std::cout << "original : " << text << std::endl;
	}

	//--------------------------------------------------------------------------------------------------------------//


	void Trim()
    {
        for (const std::string& base: std::vector<std::string>{
            "   Some   Sample    String  "
        })
        {   std::cout << "Input: " << std::quoted(base) << std::endl;

            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_1(str);
                std::cout << std::quoted(str) << std::endl;
            }
            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_2(str);
                std::cout << std::quoted(str) << std::endl;
            }
            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_3(str);
                std::cout << std::quoted(str) << std::endl;
            }
        }
	}

	void Trim_Performance()
    {
		const std::string base = "   Some   Sample    String  ";

		{
			const PerfUtilities::ScopedTimer timer { "trim_1" };
			for (int i = 0; i < 10000; i++) {
				for (int n = 0; n < 10000; n++) {
					std::string str(base);
                    StringUtilities::trim_1(str);
				}
			}
		}
		{
			const PerfUtilities::ScopedTimer timer { "trim_2" };
			for (int i = 0; i < 10000; i++) {
				for (int n = 0; n < 10000; n++) {
					std::string str(base);
                    StringUtilities::trim_2(str);
				}
			}
		}

        {
			const PerfUtilities::ScopedTimer timer { "trim_3" };
            for (int i = 0; i < 10000; i++) {
                for (int n = 0; n < 10000; n++) {
                    std::string str(base);
                    StringUtilities::trim_3(str);
                }
            }
        }
	}

	void Size_Storage()
	{
		std::string base;

		int i = 100;
		while (i--) {
			base.append("A");
			std::cout << "Size    : " << base.size() << std::endl;
			std::cout << "Capacity: " << base.capacity() << "\n" << std::endl;
		}
	}

	void Const_Lambda_Create() {
		//const int i = std::rand();
		const int i = 2;

		const std::string s = [&]() {
			switch (i % 4) {
			case 0:
				return "long string is mod 0";
			case 1:
				return "long string is mod 1";
			case 2:
				return "long string is mod 2";
			case 3:
				return "long string is mod 3";
			}
		}();

		std::cout << s << std::endl;
	}

	void Compare_String() {
		std::cout << "\n-------------------------- Test 1:" << std::endl;
		{
			std::string str1("qwerty"), str2("qwerty");
			bool result = (str1 == str2);


			std::cout << std::boolalpha << result << std::endl;
			std::cout << str1.compare(str2) << std::endl;
		}
		std::cout << "\n-------------------------- Test 2:" << std::endl;
		{
			std::string str1("qwerty"), str2("123231");
			bool result = (str1 == str2);

			std::cout << std::boolalpha << result << std::endl;
			std::cout << str1.compare(str2) << std::endl;
		}
		std::cout << "\n-------------------------- Test 3:" << std::endl;
		{
			std::string str1("1234"), str2("123456");
			bool result = (str1 == str2);

			std::cout << std::boolalpha << result << std::endl;
			std::cout << str1.compare(str2) << std::endl;
		}
		std::cout << "\n-------------------------- Test 4:" << std::endl;
		{
			std::string str1("1234"), str2("123456");
			bool result = (str1 == str2);

			std::cout << std::boolalpha << result << std::endl;
			std::cout << str1.compare(str2) << std::endl;
		}
		std::cout << "\n-------------------------- Test 5:" << std::endl;
		{
			std::string text("1234");

			std::cout << ("1234" == text) << std::endl;
		}
	}

	void Split_String() {
		std::vector<std::string> parts;
		std::string text = "One__Two__Three__Four__Five";
		std::string delimiter = "__";

		size_t pos = 0, prev = 0;
		while ((pos = text.find(delimiter, prev)) != std::string::npos) {
			std::cout << text.substr(prev, pos - prev) << std::endl;
			prev = pos + delimiter.length();
		}
		std::cout << text.substr(prev, text.length()) << std::endl;
	}

	void Split_String2() {
		std::vector<std::string> parts;
		std::string text = "One__Two__Three__Four__Five";
		std::string delimiter = "__";

		size_t pos = 0, prev = 0;
		while ((pos = text.find(delimiter, prev)) != std::string::npos) {
			parts.emplace_back(text, prev, pos - prev);
			prev = pos + delimiter.length();
		}
		parts.emplace_back(text, prev, text.length() - prev);

		for (const auto& s : parts)
			std::cout << '[' << s << ']' << std::endl;
	}

    std::vector<std::string> split(std::string_view input,
                                   std::string_view delims = " ") {
        std::vector<std::string> output;
        for (size_t first = 0;first < input.size(); ) {
            const auto second = input.find_first_of(delims, first);
            if (first != second)
                output.emplace_back(input.substr(first, second - first));
            if (second == std::string_view::npos)
                break;
            first = second + 1;
        }
        return output;
    }

    void Split_String3() {
        std::string text = "One__Two__Three__Four__Five";
        std::string delimiter = "__";
        for (auto&& parts = split(text, delimiter); const auto& s: parts) {
            std::cout << "[" << s << "]" << std::endl;
        }
    }

    void Find_Middle() {
		std::string S = "abcd", R = "";
		decltype(S)::size_type mid = S.length() / 2 - (0 == S.length() % 2) ? 1 : 0;

		R.append(1, S[mid]);
		R += S.substr(0, mid);
		R += S.substr(mid + 1, S.length() - mid);

		std::cout << R << std::endl;

	}

	void Data_Tests() {

		std::string text {"0123456789"};

		std::cout << text << std::endl;

		text.data()[2] = 'A';

		std::cout << text << std::endl;
	}

	void Reserve() {
		std::string text{ "0123456789" };

		std::cout << text << std::endl;
		std::cout << text.capacity() << std::endl;

		text.reserve(30);

		std::cout << text << std::endl;
		std::cout << text.capacity() << std::endl;
	}
};

namespace String::Literals {

	void L_String() {
		std::wstring ws = L"hello world";

		// std::cout << ws.c_str() << std::endl;
	}

	std::string BuildScript(const std::string& href) {
		std::string script("element = Array.from(document.querySelectorAll('a'));");
		script += "element.find(el=>el.href.includes('";
		script += href;
		script += "'));";
		script += "if (element) element.click(); ";
		return script;
	}

	void R_String() {
		// A Normal string 
		std::string string1 = "Geeks.\nFor.\nGeeks.\n";
		// A Raw string 
		std::string string2 = R"(Geeks.\nFor.\nGeeks.\n)";

		std::cout << string1 << std::endl;
		std::cout << string2 << std::endl;


		std::cout << "\n-------------------------------------------- Str3 ---------------------------------" << std::endl;

		std::string string3 = R"(
			Hello
			World
			)";

		std::cout << string3<< std::endl;

		std::cout << "\n-------------------------------------------- Str4 ---------------------------------" << std::endl;

		std::string string4 = R"(Some/String\n)";
		std::cout << string4 << std::endl;


		std::string script(R"(const element = Array.from(document
			.querySelectorAll('a')).find(el=>el.href.includes(')");

		std::cout << script << std::endl;

		std::cout << BuildScript("TTTTT") << std::endl;
	}

	//////////////////

	void R_String_2() {
		std::string script(R"(
		array = Array.from(document.querySelectorAll('a')); 
		element = array.find(el=>el.href.includes('%s'));
		if (element) { 
			element.click();
		}
		)");

		std::cout << script << std::endl;
	}


	// -------------------------------------------------------------------------------//

	void R_String_ConstExpr() {

		/*
		constexpr auto jsv = R"({
			"feature-x-enabled": true,
			"value-of-y": 1729,
			"z-options": {"a": null,
			"b": "220 and 284",
			"c": [6, 28, 496]}
			})"_json;
		if constexpr (jsv["feature-x-enabled"]) {
			// code for feature x
		}
		else {
			// code when feature x turned off
		}*/
	}
}

namespace String::Sprintf {

	template <typename ...Args>
	std::string stringWithFormat(const std::string& format, Args && ...args)
	{
		auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
		std::string output(size + 1, '\0');
		std::sprintf(&output[0], format.c_str(), std::forward<Args>(args)...);
		return output;
	}

	template <typename ...Args>
	std::string stringWithFormatM(std::string&& format, Args && ...args)
	{
		// Calling std::snprintf with zero buf_size and null pointer for buffer 
		// is useful to determine the necessary buffer size to contain the output:
		size_t size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
		std::string output(size + 1, '\0');
		std::sprintf(output.data(), format.c_str(), std::forward<Args>(args)...);
		return output;
	}

	/////////////////////////////////////////////

	void Test() {
		char output[128] = {};
		int result = std::sprintf(output, "String: %s, Value: %d", "Str_Text", 123);
		std::cout << "result = " << result << std::endl;
		std::cout << "output: " << output << std::endl;
	}

	void Format_String_1() {
		std::string result = stringWithFormat("String: %s, Value: %d", "Str_Text", 123);
		std::cout << result << std::endl;
	}

	void Format_String_2() {
		std::string result = stringWithFormatM("String: %s, Value: %d", "Str_Text", 123);
		std::cout << result << std::endl;
	}

}

namespace String::Utilities {


	template <class Container>
	void split_string_1(const std::string& str, 
		                Container& cont) {
		std::istringstream iss(str);
		std::copy(std::istream_iterator<std::string>(iss),
			std::istream_iterator<std::string>(),
			std::back_inserter(cont));
	}

	void split_string_2(const std::string& str,
						std::vector<std::string>& cont,
						const std::string& delimiter) {
		size_t pos = 0, prev = 0;
		while ((pos = str.find(delimiter, prev)) != std::string::npos) {
			cont.emplace_back(str.substr(prev, pos - prev));
			prev = pos + delimiter.length();
		}
		cont.emplace_back(str.substr(prev, pos - prev));
	}

	void split_string_3(const std::string& str,
						std::vector<std::string>& cont,
						const std::string& delimiter) {
		size_t pos = 0, prev = 0;
		while ((pos = str.find(delimiter, prev)) != std::string::npos) {
			cont.emplace_back(str.begin() + prev, str.begin() + pos);
			prev = pos + delimiter.length();
		}
		cont.emplace_back(str.substr(prev, pos - prev));
	}

	void split_string_4(const std::string& str,
					    std::vector<std::string>& cont,
					    const std::string& delimiter) {
		size_t pos = 0, prev = 0;
		while ((pos = str.find(delimiter, prev)) != std::string::npos) {
			cont.emplace_back(str, prev, pos - prev);
			prev = pos + delimiter.length();
		}
		cont.emplace_back(str, prev, str.length() - prev);
	}


	void SwapChars(std::string text, const char src, const char dst) {
		std::transform(text.begin(), text.end(), text.begin(), [=](int c) { return src == c ? dst : c; });
	}
}

namespace String::Performance_Tests
{
	void SplitTest()
	{
		const std::string base = "11111111a 22222222222b 3333333333333c"
						   " 4444444444d 55555555555f 6666666666666g";
		std::vector<std::string> parts;
		constexpr size_t ITER_COUNT { 1'000'000 };

		{
			const PerfUtilities::ScopedTimer timer { "split_string_1" };
			for (size_t i = 0; i < ITER_COUNT; i++) {
				Utilities::split_string_1(base, parts);
				parts.clear();
			}
		}
		{
			const PerfUtilities::ScopedTimer timer { "split_string_2" };
			for (size_t i = 0; i < ITER_COUNT; i++) {
				Utilities::split_string_2(base, parts, " ");
				parts.clear();
			}
		}
		{
			const PerfUtilities::ScopedTimer timer { "split_string_3" };
			for (size_t i = 0; i < ITER_COUNT; i++) {
				Utilities::split_string_3(base, parts, " ");
				parts.clear();
			}
		}
		{
			const PerfUtilities::ScopedTimer timer { "split_string_4" };
			for (size_t i = 0; i < ITER_COUNT; i++) {
				Utilities::split_string_4(base, parts, " ");
				parts.clear();
			}
		}
	}

#pragma optimize("", off)
    void Compare()
	{
        std::string a = "1111111111111111", b = "1111111111111111";
        std::string c = "1111111111111111", d = "1111111111111111";

		[[maybe_unused]]
        constexpr size_t ITER_COUNT { 1'000'000 };

        /*
        {
            const PerfUtilities::ScopedTimer timer { "Benchmark" };
            for (int i = 0; i < ITER_COUNT; i++) {
                auto x = (a == b);
            }
        }
        {
            const PerfUtilities::ScopedTimer timer { "Benchmark" };
            for (int i = 0; i < ITER_COUNT; i++) {
                auto x = a.compare(b);
            }
        }
        */
    }
#pragma optimize("", on)
}

namespace String::Conversations {

	void From_Chars() {

		const char* str = "12 monkeys";
		int value;
		if (auto[ptr, error_code] = std::from_chars(str, str + 10, value);
            std::errc{} != error_code) {
			std::cout << "Errror" << std::endl;
		}

		std::cout << "Result: " << value << std::endl;
	}

	void From_Chars_Array() {

		std::array<char, 10> str{ "42 xyz " };
		int value;
		if (auto [ptr, error_code] = std::from_chars(str.data(), str.data() + str.size(), value);
			std::errc{} != error_code) {
			std::cout << "Errror" << std::endl;
		}

		std::cout << "Result: " << value << std::endl;
	}

	void From_Chars_StringView() {

		std::string_view str{ "24 abc " };
		int value;
		if (auto [ptr, error_code] = std::from_chars(str.data(), str.data() + str.size(), value);
			std::errc{} != error_code) {
			std::cout << "Errror" << std::endl;
		}

		std::cout << "Result: " << value << std::endl;
	}

	void From_Chars_ERROR()
	{
		const char* str = "#$#$onkeys";
		int value = 0;
		if (auto[ptr, ec] = std::from_chars(str, str + 10, value); ec != std::errc{}) {
			std::cout << "Errror" << std::endl;
		}

		std::cout << "Result: " << value << std::endl;
	}

	void To_Chars() {
		int value = 42;
		char str[10];
		std::to_chars_result res = std::to_chars(str, str + 9, value);
		*res.ptr = '\0'; // ensure a trailing null character is behind

		std::cout << res.ptr << std::endl;
	}

	void To_Chars_2() {
		char buffer[128]{};
		double value1{ 0.314 };

		std::string out(buffer, ' '); // A string of BufferSize space characters.
		auto [ptr1, error1] { std::to_chars(out.data(), out.data() + out.size(), value1) };
		if (error1 == std::errc{}) { /* Conversion successful. */
			std::cout << out << std::endl;
		}
	}

	void To_Upper() {
		std::string src{ "abcdefg" }, dst;
		constexpr int offset = 'A' - 'a';

		dst.assign(src);
		for (size_t i = 0; i < src.size(); ++i)
			dst[i] = static_cast<char>(src[i] + offset);

		std::cout << dst << std::endl;
	}

	void To_Lower() {
		std::string str{ "AAAbbbCCCCdddddEEEE" };
		std::for_each(str.begin(), str.end(), [](auto& c) { c = std::tolower(c); });
		std::cout << str << std::endl;
	}
}

//---------------------------------------------------------------------------------------------------//

namespace String::RegEx {
	using namespace std::string_literals;

	// ^                - Start of string
	// [A-Z0-9._%+-]+   - At least one character in the range A-Z, 0-9, or one of -, %, + or - that represents the local part of the email address
	// @                - Character @
	// [A-Z0-9.-]+      - At least one character in the range A-Z, 0-9, or one of -, %, + or - that represents the hostname of the domain part
	// \.               - A dot that separates the domain hostname and label
	// [A-Z]{2,}        - The DNS label of a domain that can have between 2 and 63 characters
	// $                - End of the string

	bool is_valid_email_format_case_sensetive(std::string const & email) {
		constexpr auto pattern { R"(^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,}$)" };
		const auto rx = std::regex{ pattern , std::regex_constants::icase };
		return std::regex_match(email, rx);
	}

	bool is_valid_email_format(std::string const & email) {
		constexpr auto pattern{ R"(^[A-Z0-9._%+-]+@[A-Z0-9.-]+\.[A-Z]{2,}$)" };
		const auto rx = std::regex{ pattern};
		return std::regex_match(email, rx);
	}

	void run_test(const std::string& email) {
		std::cout << "Email: " << std::setw(25) << std::left << email << "  Match: "
			<< (is_valid_email_format_case_sensetive(email) ? "Valid" : "Invalid") << " (Case sensitive)" << std::endl;
		std::cout << "Email: " << std::setw(25) << std::left << email << "  Match: "
		    	  << (is_valid_email_format(email) ? "Valid" : "Invalid") << std::endl;
	}

	void EMail_Validator() {
		const std::vector<std::string> emailList{
			"JOHN.DOE@DOMAIN.COM",       // valid format
			"JOHNDOE@DOMAIL.CO.UK",      // valid format
			"JOHNDOE@DOMAIL.INFO",       // valid format
			"J.O.H.N_D.O.E@DOMAIN.INFO", // valid format
			"ROOT@LOCALHOST",            // invalid format
			"john.doe@domain.com"        // valid format
		};

		for (const auto& email : emailList)
			run_test(email);
	}

	//---------------------------------------------------//	

	const auto config { 
R"(
#remove # to uncomment the following lines
timeout = 120
server = 127.0.0.1
#retrycount=3
)"s };

	void Parsing_Test() {
		const auto pattern{ R"(^(?!#)(\w+)\s*=\s*([\w\d]+[\w\d._,\-:]*)$)"s };
		const auto rx = std::regex {pattern};

		auto match = std::smatch{};
		if (std::regex_search(config, match, rx)) {
			std::cout << match[1] << '=' << match[2] << std::endl;
		}
	}

	void Regex_Iterator() {
		const auto pattern{ R"(^(?!#)(\w+)\s*=\s*([\w\d]+[\w\d._,\-:]*)$)"s };
		const auto rx = std::regex{ pattern };

		auto end = std::sregex_iterator{};
		for (auto it = std::sregex_iterator{ std::begin(config), std::end(config), rx }; it != end; ++it){
			std::cout << "'"<< (*it)[1] << "' = '" << (*it)[2] << "'" << std::endl;
		}
	}

	void Regex_Token_Iterator() {
		const auto pattern{ R"(^(?!#)(\w+)\s*=\s*([\w\d]+[\w\d._,\-:]*)$)"s };
		const auto rx = std::regex{ pattern };

		auto end = std::sregex_token_iterator{};
		for (auto it = std::sregex_token_iterator{std::begin(config), std::end(config), rx };it != end; ++it) {
			std::cout << *it << std::endl;
		}
	}
}

namespace String::Format {
	void Test() {

	 
	}
}

namespace String::WString {

	void Create_Print() {
		const std::wstring szName = L"Global\\MyFileMappingObject";

		std::wcout << szName << std::endl;
	}
}


namespace String::TESTS {

	void Long_MultiLine_String() {
		std::string s = "("
			"field1=value1"
			")"
			" or "
			"("
			"("
			"field6=value2"
			" or "
			"field2=value3"
			" or "
			"field3=value4"
			")"
			" and "
			"("
			"field1=value2"
			")"
			")";
		
		std::cout << s << "\n";
	}

	void Accumulate_String_JOIN() {
		const auto magic_function = [](std::string res, std::string res1)->std::string {
			if (false == res.empty())
				return res.append(",").append(res1);
			return res.append(res1);
		};


		const std::array<std::string, 5> words{"One", "Two", "Three", "Four", "Five"};
		const std::string result = std::accumulate(words.cbegin(), words.cend(), std::string(), magic_function);
		std::cout << result << std::endl;
	}
}

namespace String::Performance
{

	void Assign_Substr_vs_Iterators()
	{
		constexpr size_t startPos = 3, endPos = 27;
		constexpr int TESTS_COUNT {100'000'000};
		const std::string src = "1111111111111111111111111111111111111111111111111111111111111111111111111111111";
		std::string dest;

		{
			const PerfUtilities::ScopedTimer timer { "assign 1" };
			for (int i = 0; i < TESTS_COUNT; ++i) {
				dest.assign(src.substr(startPos, endPos - startPos));
			}
		}
		{
			const PerfUtilities::ScopedTimer timer { "assign 2" };
			for (int i = 0; i < TESTS_COUNT; ++i) {
				dest.assign(src.begin() + startPos, src.begin() + endPos);
			}
		}
		{
			const PerfUtilities::ScopedTimer timer { "insert" };
			for (int i = 0; i < TESTS_COUNT; ++i) {
				dest.clear();
				dest.insert(dest.begin(), src.begin() + startPos, src.begin() + endPos);
			}
		}
	}

	__attribute__((optimize("O0")))
	void Find_ByString_vs_StringView()
	{
		const std::string HTTP_VERSION_SEPARATOR = R"( HTTP)";
		constexpr std::string_view HTTP_VERSION_SEPARATOR_STR = R"( HTTP)";
		const std::string src = "11111111111111111111111111111111111111111111111111111111 "
						  "HTTP 11111111111 HTTP 111111111111";
		constexpr int TESTS_COUNT{ 100'000'000 };

		{
			const PerfUtilities::ScopedTimer timer { "Benchmark" };
			for (int i = 0; i < TESTS_COUNT; ++i) {
				const auto _ = src.find(HTTP_VERSION_SEPARATOR_STR);
			}
		}
		{
			const PerfUtilities::ScopedTimer timer { "Benchmark" };
			for (int i = 0; i < TESTS_COUNT; ++i) {
				auto _ = src.find(HTTP_VERSION_SEPARATOR);
			}
		}	
	}

	using namespace std::literals;

	// #pragma optimize( "", off )
	void Compare_Part_Of_String()
	{
		const std::string str1 = "SomeRandomText_1234";
		const std::string str2 = "SomeRandomText_4321";
		constexpr size_t offset = ("SomeRandomText_"sv).size();
		constexpr size_t TESTS_COUNT = 100'000'000;

		{
			const PerfUtilities::ScopedTimer timer { "substr (string)" };
			for (size_t i = 0; i < TESTS_COUNT; ++i)
			{
				const auto x1 = str1.substr(offset, str1.length() - offset);
				const auto x2 = str2.substr(offset, str1.length() - offset);
				[[maybe_unused]]
				const auto result = x1.compare(x2);
			}
		}
		{
			const PerfUtilities::ScopedTimer timer { "substr (string_view)" };
			for (size_t i = 0; i < TESTS_COUNT; ++i)
			{
				const auto x1 = std::string_view(str1).substr(offset, str1.length() - offset);
				const auto x2 = std::string_view(str2).substr(offset, str1.length() - offset);
				[[maybe_unused]]
				const auto result = x1.compare(x2);
			}
		}
	}
	// #pragma optimize( "", on )
}


namespace VariousTests {

	void Swap_String_Parts() {

		std::string str1 {"123456789"};
		std::string str2{ "" };

		str2.assign(str1.erase(0, 3));

		std::cout << "str1 = " << str1 << std::endl;
		std::cout << "str2 = " << str2 << std::endl;
	}
}


namespace String::Application_Examples
{

    std::vector<std::string> Split1(const std::string& text,
                                    std::string_view delimiter = " ") {
        std::vector<std::string> parts;
        size_t pos = 0, prev = 0;
        while ((pos = text.find(delimiter, prev)) != std::string::npos) {
            parts.emplace_back(text, prev, pos - prev);
            prev = pos + delimiter.length();
        }
        parts.emplace_back(text, prev, text.length() - prev);
        return parts;
    }

    std::vector<std::string> Split2(std::string_view input,
                                    std::string_view delimiters = " ") {
        std::vector<std::string> output;
        for (size_t first = 0;first < input.size(); ) {
            const auto second = input.find_first_of(delimiters, first);
            if (first != second)
                output.emplace_back(input.substr(first, second - first));
            if (second == std::string_view::npos)
                break;
            first = second + 1;
        }
        return output;
    }


    void Split_String_Tests()
    {
        const std::string text = "One Two Three Four Five";

        for (const auto& parts = Split2(text, " "); const auto &s: parts)
            std::cout << s << " ";
        std::cout << std::endl;

        for (const auto& parts = Split2(text, " "); const auto &s: parts)
            std::cout << s << " ";
        std::cout << std::endl;
    }

    //-----------------------------------------------------------------


    std::string trimLeft(const std::string &s) {
        auto temp = s;
        temp.erase(std::begin(temp), std::find_if_not(std::begin(temp), std::end(temp), isspace));
        return temp;
    }


    std::string trimRight(const std::string &s) {
        auto temp = s;
        temp.erase(std::find_if_not(std::rbegin(temp), std::rend(temp), isspace).base(), std::end(temp));
        return temp;
    }

    void TrimString() {
        std::string str { "   abc   "};
        std::cout << "[" << str << "] = [" << trimRight(trimLeft(str)) << "]" << std::endl;
    }

    //---------------------------------------------------------------------------------

    bool _is_palindrome(const std::string& str) {
        return std::equal(str.begin(),
                          str.begin() + std::ssize(str)/ 2,
                          str.rbegin());
    }

    template<class It>
    bool _is_palindrome_2(It first, It last) {
        return std::equal( first, last, std::reverse_iterator<It>(last));
    }

    bool _is_reversed_equals(const std::string& string1, const std::string& string2) {
        return std::equal(string1.begin(),
                          string1.end(),
                          string2.rbegin());
    }

    void IsPalindrome()
    {
        auto palindromeTest = [](const std::string& str) {
            std::cout << std::boolalpha << _is_palindrome("radar") << "  "
                      << std::boolalpha << _is_palindrome_2(str.begin(), str.end())
                      <<  std::endl;
        };


        palindromeTest("radar");
        palindromeTest("rada1r");
    }
}

// #define SSO_STR_ALLOC_TEST

#ifdef SSO_STR_ALLOC_TEST
    void *operator new(size_t sz) {
        std::cout << "[allocating " << sz << " bytes]\n";
        return std::malloc(sz);
    }
#endif

namespace String::SSO
{

    void Test()
    {
        std::string text{""};
        for (int i = 0; i < 20; ++i) {
            text.append("X");
            std::cout << text.size() << "   " << text.capacity() << std::endl;
        }
    }


    void AllocationTest()
    {
        std::cout << "----------------------------------------------------------------\n";
        std::cout << "\tDo not forget uncomment: 'void *operator new(size_t sz)'\n";
        std::cout << "----------------------------------------------------------------\n";

        std::string s1 { "0123456789" };
        std::string s2 { "01234567890123456789"};
    }
}


namespace String::CheckSymbols
{
    void IsDigit()
    {
        constexpr std::string_view text { "qwert123456"};

        for (char c: text)
        {
            std::cout << "isdigit(" << c << ") = " << std::isdigit(c) << std::endl;
            std::cout << "isalnum(" << c << ") = " << std::isalnum(c) << std::endl;
            std::cout << "isalpha(" << c << ") = " << std::isalpha(c) << std::endl;
            std::cout << std::endl;
        }
    }

}


namespace String::ParsingTests
{

    void GetNumericPart()
    {
        const std::string text { "    1234  ;"};

        const size_t start = text.find_first_not_of(' ');
        const auto end = std::find_if_not(text.cbegin() + start, text.cend(), [](char c) {
            return std::isdigit(c);
        });

        std::cout << "'" << std::string_view {text.begin() + start, end} << "'\n";

    }
}


namespace CompileTimeValidation
{
    constexpr bool is_valid(char c)
    {
        constexpr std::string_view valid = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        return valid.find(c) != std::string_view::npos;
    }

    template<size_t N>
    struct Format
    {
        constexpr Format(const char (&str)[N]) {
            std::copy_n(str, N, value);
        }

        [[nodiscard]]
        constexpr bool isValid() const
        {
            if constexpr (N == 0){
                return false;
            }

            for (char ch : value) {
                if(ch != '\0' && !is_valid(ch)) {
                    return false;
                }
            }

            return true;
        }

        [[nodiscard]]
        constexpr std::string_view format() const {
            return value;
        }

        char value[N]{};
    };

    // Function template with a non-type template parameter
    template <Format Fmt>
    void processString()
    {
        static_assert(Fmt.isValid(), "String format is invalid!");
        std::cout << "Processing string: " << Fmt.format() << std::endl;
    }

    void compileTimeValidation()
    {
        processString<"Valid123">();  // This compiles successfully
        // processString<"Invalid@Char">();  // This would fail at compile-time
    }
}

namespace String::Resize_String
{
	void stringInto(const std::string& str)
	{
		std::cout << "content: " << str << " | size: " << str.size() << " | capacity: " << str.capacity() << std::endl;
	}

	void Resize()
	{
		std::string text {"abcdefghijkabcdefghijkabcdefghijk"};
		stringInto(text);
		text.resize(12);
		stringInto(text);

		/**
		 * content: abcdefghijkabcdefghijkabcdefghijk | size: 33 | capacity: 33
		 * content: abcdefghijka | size: 12 | capacity: 33
		 **/
	}

	void Resize_and_Overwrite()
	{
		std::string text {"abcdefghijkabcdefghijkabcdefghijk"};
		stringInto(text);

		text.resize_and_overwrite(5, [](char* buf, std::size_t n) {
			for (std::size_t i = 2; i < n; ++i) {
				buf[i] = '0' + i;
			}
			return n;
		});

		stringInto(text);

		/**
		 * content: abcdefghijkabcdefghijkabcdefghijk | size: 33 | capacity: 33
		 * content: ab234 | size: 5 | capacity: 33
		 **/
	}
}

void String::TestAll()
{
    // CompileTimeValidation::compileTimeValidation();


	// Create_Test();
	// Create_FromStrings();
	// Create_FromStrings2();
	// Vector_of_String();

	// Itearate();
	// ItearateBackward_RbeginRend();

	// Append();
	// Append2();

	// StartsWith();

	// Resize_String::Resize();
	// Resize_String::Resize_and_Overwrite();

	// Crbegin_Crend_Test();
	// PushBack();
	// PopBack();
	// max_size();

	// Replace();
	// Replace2();

	// Remove();
	// Swap_First_and_Last_Chars();

	// Find_Tests();
	// Find_FirstOf();

	// ConcatStrings();

	// SubString();
	// SubString_Move();

	// Const_Lambda_Create();

	// Insert();

	// Assign__Substr_vs_Iterators();


	// Trim();
	Trim_Performance();

	// Size_Storage();

	// Literals::R_String();
	// Literals::R_String_2();
	// Literals::R_String_ConstExpr();
	// Literals::L_String();

	// Data_Tests();

	// Reserve();

	// Sprintf::Test();
	// Sprintf::Format_String_1();
	// Sprintf::Format_String_2();

	// Compare_String();

	// Split_String();
	// Split_String2();
	// Split_String3();

	// Performance_Tests::SplitTest();
	// Performance_Tests::SplitTest();
	// Performance_Tests::Compare();

	// Find_Middle();

	// Conversations::From_Chars();
	// Conversations::From_Chars_Array();
	// Conversations::From_Chars_StringView();

	// Conversations::From_Chars_ERROR();
	// Conversations::To_Chars();
	// Conversations::To_Chars_2();
	// Conversations::To_Upper();
	// Conversations::To_Lower();

	// RegEx::EMail_Validator();
	// RegEx::Parsing_Test();
	// RegEx::Regex_Iterator();
	// RegEx::Regex_Token_Iterator();

	// Format::Test();

	// SSO::Test();
	// SSO::AllocationTest();

	// WString::Create_Print();

    // CheckSymbols::IsDigit();


	// TESTS::Long_MultiLine_String();
	// TESTS::Accumulate_String_JOIN();	
	
	
	// Performance::Assign_Substr_vs_Iterators();
	// Performance::Find_ByString_vs_StringView();
	// Performance::Compare_Part_Of_String();

	// VariousTests::Swap_String_Parts();


    // Application_Examples::Split_String_Tests();
    // Application_Examples::TrimString();
    // Application_Examples::IsPalindrome();

    // ParsingTests::GetNumericPart();
};