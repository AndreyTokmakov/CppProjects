	//============================================================================
// Name        : StructuredBinding.cpp
// Created on  : 28.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Structured binding declaration src
//============================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <cassert>
#include <vector>
#include <map>
#include <array>
#include <set>

#include "StructuredBinding.h"

namespace StructuredBinding {

	struct BookInfo {
		std::string title;
		int yearPublished = 0;
	};

	BookInfo readBookInfo() {
		BookInfo info;
		info.title = "Title";
		info.yearPublished = 2017;
		return info;
	}

	///////////////////////////////////////////////////////

	void VectorTest() {
		std::vector v {std::vector{1, 2}};

		// This is vector<int>, but not vector<vector<int>>
		static_assert(std::is_same_v<std::vector<int>, decltype(v)>);

		// Size is equal 2
		assert(v.size() == 2);

		std::cout << "Done" << std::endl;
	}

	void PairTest() {
		auto pair = std::pair{ 10, "hello" };

		std::cout << pair.first << std::endl;
		std::cout << pair.second << std::endl;
	}

	void Test_Pair() {
		std::pair p = { 1, "hello" };
		auto [first, second] = p;

		std::cout << "Pair value : " << "{" << first << ", " << second << "}" << std::endl;

		std::cout << "\nModifing values." << std::endl;
		first = 0;
		second = "Bie";

		std::cout << "\nPair value : " <<  "{" << first << ", " << second << "}" << std::endl;
	}

	void Test_Array() {
		int coord[3] = { 1, 2, 3 };
		auto[x, y, z] = coord;

		std::cout << "{" << x << ", " << y << ", " << z << "}"  << std::endl;
	}

	void Test_Struct_Reference_Init() {
		struct Config {
			int			id;
			std::string name;
			std::vector<int> data;
		};
		Config cfg;

		auto&[id, n, d] = cfg;

		id = 1;
		n = "name";
		d.push_back(123);

		std::cout << cfg.id << std::endl;
		std::cout << cfg.name << std::endl;
		std::cout << cfg.data[0] << std::endl;
	}

	void Test_Struct() {
		auto[title, year] = readBookInfo();
		std::cout << title << std::endl;
		std::cout << year << std::endl;
	}

	void Test_TryEmplace_Map() {
		std::map<std::string, std::string> map;
		auto[iterator1, succeed1] = map.try_emplace("key", "abc");
		auto[iterator2, succeed2] = map.try_emplace("key", "cde");
		auto[iterator3, succeed3] = map.try_emplace("another_key", "cde");

		assert(succeed1);
		assert(!succeed2);
		assert(succeed3);

		for (auto&&[key, value] : map) {
			std::cout << key << ": " << value << "\n";
		}
	}

	void Test_Insert_Set()
	{
		{
			std::set<int> test_set;
			auto[iter, ok] = test_set.insert(42);
			std::cout << "{" << *iter << ", " << ok << "}" << std::endl;
		}

		std::cout << "\nOld style:\n" << std::endl;

		{
			std::set<int> mySet;
			std::set<int>::iterator iter;
			bool ok;
			std::tie(iter, ok) = mySet.insert(42);
			std::cout << "{" << *iter << ", " << ok << "}" << std::endl;
		}
	}


	void Discard_Param() {
		auto getPair = []() { return std::pair(441, "hello"); };

		auto&& [_, s] = getPair();

		std::cout << _ << std::endl;
	}

	std::array<int, 4> getArray() {
		return {1,2,3,4};
	}

	void Bind_Array() {
		auto[a,b,c,d] = getArray();

		std::cout << a << std::endl;
		std::cout << b << std::endl;
		std::cout << c << std::endl;
		std::cout << d << std::endl;
	}

	struct MyStruct {
		int a;
		std::string str;
	};

	void Move_Test_1() {

		MyStruct ms { 42, "Jim" };
		std::cout << "{" << ms.a << ", " << ms.str << "}\n" << std::endl;


		auto&& [v, n] = std::move(ms);
		std::cout << "{" << v << ", " << n << "}" << std::endl;
		std::cout << "{" << ms.a << ", " << ms.str << "}" << std::endl;

		n.assign("333");


		std::cout << "\n{" << v << ", " << n << "}" << std::endl;
		std::cout << "{" << ms.a << ", " << ms.str << "}" << std::endl;

	}

	void Move_Test_2() {

		MyStruct ms{ 42, "Jim" };
		std::cout << "{" << ms.a << ", " << ms.str << "}\n" << std::endl;


		auto [v, n] = std::move(ms);
		std::cout << "{" << v << ", " << n << "}" << std::endl;
		std::cout << "{" << ms.a << ", " << ms.str << "}" << std::endl;
	}

	void Test() {

		std::pair p {"One", 2};
		auto [a, b] {p};

		std::cout << a << "  " << b << std::endl;
		
	}
}

namespace StructuredBinding::CustomdBinding
{
	template<typename T1, typename T2>
	struct Data
	{
		T1 a {1};
		T2 b {2};
	};

	template<typename T1, typename T2>
	class UserEntry {
		int value {};
		std::string name {""};
		Data<T1, T2> data;

	public:
		UserEntry(int x, std::string y) : value(x), name(std::move(y)) {
		}

		template <std::size_t _Index>
		auto& get() { // Dispatch to the right member using if constexpr.
			if constexpr (0 == _Index) {
				return value;
			}
			else if constexpr (1 == _Index) {
				return name;
			}
			else {
				return data;
			}
		}

		template <std::size_t _Index>
		const auto& get() const {
			if constexpr (0 == _Index) {
				return value;
			} else if constexpr (1 == _Index) {
				return name;
			} else {
				return data;
			}
		}
	};

}

/*
namespace std {
	template<typename T1, typename T2>
	struct tuple_size<StructuredBinding::CustomdBinding::UserEntry<T1, T2>>:
		std::integral_constant<std::size_t, 3> {
		//  A always has 2 members
	};

	template<typename T1, typename T2>
	struct tuple_element<0, StructuredBinding::CustomdBinding::UserEntry<T1, T2>> {
		// The member at index 0 has type int.
		using type = int;
	};

	template<typename T1, typename T2>
	struct tuple_element<1, StructuredBinding::CustomdBinding::UserEntry<T1, T2>> {
		// The other one has type std::string.
		using type = std::string;
	};


	template<typename T1, typename T2>
	struct tuple_element<2, StructuredBinding::CustomdBinding::UserEntry<T1, T2>> {
		// The other one has type std::string.
		using type = StructuredBinding::CustomdBinding::Data<T1, T2>;
	};
}
*/


template<typename T1, typename T2>
struct std::tuple_size<StructuredBinding::CustomdBinding::UserEntry<T1, T2>> :
	std::integral_constant<std::size_t, 3> {
	//  A always has 2 members
};

template<typename T1, typename T2>
struct std::tuple_element<0, StructuredBinding::CustomdBinding::UserEntry<T1, T2>> {
	// The member at index 0 has type int.
	using type = int;
};

template<typename T1, typename T2>
struct std::tuple_element<1, StructuredBinding::CustomdBinding::UserEntry<T1, T2>> {
	// The other one has type std::string.
	using type = std::string;
};


template<typename T1, typename T2>
struct std::tuple_element<2, StructuredBinding::CustomdBinding::UserEntry<T1, T2>> {
	// The other one has type std::string.
	using type = StructuredBinding::CustomdBinding::Data<T1, T2>;
};

namespace StructuredBinding::CustomdBinding
{


	void Test()
	{
		UserEntry<int, int> a{ 1, "yo" };
		const auto& [x, y, z] = a;

		std::cout << x << std::endl;
		std::cout << y << std::endl;
		std::cout << "{" <<  z.a << ", " << z.b << "}" << std::endl;
	}
}


namespace StructuredBinding::If_Init_Comparison_Operator_Cpp26
{
	struct kinetic_energy_result
	{
		enum class error_code
		{
			no_error,
			mass_is_negative,
		};

		error_code errorCode;
		double result { 0.0 };

		// This will be evaluated as the `if` condition
		explicit operator bool() const
		{
			std::cout << "Calling kinetic_energy_result::operator bool()" << std::endl;
			return errorCode == error_code::no_error;
		}
	};

	using enum kinetic_energy_result::error_code;

	kinetic_energy_result kinetic_energy(const double mass, const double velocity)
	{
		if (mass < 0.0)
			return {
				.errorCode=mass_is_negative,
				.result=std::numeric_limits<double>::quiet_NaN()
			};
		return {
			.errorCode=no_error,
			.result=0.5 * mass * velocity * velocity
		};
	}

	void demo()
	{
		constexpr double velocity = 10.0;
		for(const double mass : { 10.0, -1.5, 20.0 })
		{
			// This is new in C++26 !
			if (const auto [error_code, result] = kinetic_energy(mass, velocity))
				std::cout << "The kinetic energy of a mass of " << mass << " kg moving at "
					<< velocity << "m/s is " << result << " J\n";
			else
			{
				switch(error_code)
				{
					case mass_is_negative:
						std::cout << "Error, mass is negative (with mass = {" << mass << "})\n";
						break;
					default:
						std::cout << "Unknown error.\n";
						break;
				}
			}
		}
	}

	/**
	Calling kinetic_energy_result::operator bool()
	The kinetic energy of a mass of 10 kg moving at 10m/s is 500 J
	Calling kinetic_energy_result::operator bool()
	Error, mass is negative (with mass = {-1.5})
	Calling kinetic_energy_result::operator bool()
	The kinetic energy of a mass of 20 kg moving at 10m/s is 1000 J
	**/
}


void StructuredBinding::TestAll()
{
	// Test_Struct_Reference_Init()
	// Test_Struct();

	// Test_Pair();

	// Test_Array();

	// Test_TryEmplace_Map();

	// Test_Insert_Set();

	// VectorTest();

	// Discard_Param();

	// Bind_Array();

	// Move_Test_1();
	// Move_Test_2();

	// Test();

	// CustomBinding::Test();

	// If_Init_Comparison_Operator_Cpp26::demo();
}