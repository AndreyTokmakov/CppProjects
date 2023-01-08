//============================================================================
// Name        : Utilities.h
// Created on  : 10.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Utilities libraries src
//============================================================================

#include <iostream>
#include <string>
#include <functional>

#include "Utilities.h"

using String = std::string;
using CString = const String&;

namespace Utilities::ApplyTests {

	int add(int first, int second) {
		return first + second;
	}

	void Apply_Sum_Test()
	{
		std::cout << std::apply(add, std::make_pair(1, 2)) << std::endl;
	}

	void Apply_Sum_Lambda_Test()
	{
		auto add_lambda = [](auto first, auto second) { return first + second; };
		std::cout << std::apply(add_lambda, std::make_pair(2.0f, 3.0f)) << std::endl;
	}

	void Apply_Sum_Tuple()
	{
		std::tuple<int, int, int>  tup(1, 2, 3);
		auto summ = [](auto a, auto b, auto c) { return a + b + c; };
		std::cout << std::apply(summ, tup) << std::endl;
	}
}

namespace Utilities::Invoke_Tests {

	class Utilities {
	public:
		int value;

	public:
		Utilities(int val) : value(val) {
		}

		void add_and_print(const std::string& text) const {
			std::cout << text << std::endl;
		}

		int getValue() {
			return this->value;
		}

		void printValue() const {
			std::cout << "Value :" << this->value << std::endl;
		}
	};

	class Functor {
	public:
		void operator() (int a) {
			std::cout << a << std::endl;
		}
	};

	///////////////////////////////////////////////////////////////////////////

	void Call_Class_Method() {
		const Utilities utils(314159);

		utils.printValue();
		std::invoke(&Utilities::add_and_print, utils, "Some_Text");
		utils.printValue();
	}

	void Invote_Functor() {
		const auto sum = [](int a, int b)-> int { return a + b; };

		std::cout << "is_invocable<Functor(): "<< std::boolalpha << std::is_invocable<Functor()>::value << std::endl;
		std::cout << std::invoke(sum, 10, 20) << std::endl;
		std::invoke(Functor(), 42);
		std::invoke([]() { std::cout << "hello" << std::endl;; });
	}

	void Access_Member() {
		const Utilities utils(12345);
		// invoke (access) a data member
		std::cout << "Value : " << std::invoke(&Utilities::value, utils) << std::endl;
	}

	void Is_Invocable() {
		const auto sum = [](int a, int b)-> int { return a + b; };

		std::cout << std::boolalpha << std::is_invocable<decltype(sum)>::value << std::endl;
		std::cout << std::boolalpha << std::is_invocable<int>::value << std::endl;
	}
}

namespace Utilities::Make_Tuples {

	struct Foo {
		Foo(int first, float second, int third) {
			std::cout << first << ", " << second << ", " << third << std::endl;
		}
	};

	class Object {
	private:
		std::string str1;
		std::string str2;

	public:
		Object(const std::string& s1, const std::string& s2 ): str1(s1), str2(s2) {
		}

		std::string toString() const noexcept {
			return "{" + str1 + "," + str2 + "}";
		}
	};

	void Test()
	{
		auto tuple = std::make_tuple(42, 3.14f, 0);
		auto obj = std::make_from_tuple<Foo>(std::move(tuple));
	}

	void Test2()
	{
		auto tuple = std::make_tuple("val1", "val2");
		Object obj = std::make_from_tuple<Object>(std::move(tuple));
		std::cout << obj.toString() << std::endl;
	}
}

namespace Utilities::Utilities_Library {

	void Ptrdiff_t()
	{
		int data[] = { 0,1,2,3,4,5,6,7,8,9 };
		for (std::ptrdiff_t i = 0; i < std::size(data); i++)
			std::cout << data[i] << " ";
		std::cout << std::endl;
	}
}

namespace Integer_Comparison_Functions {

	consteval int getIntSigned() {
		return -1;
	}

	consteval unsigned int getIntUnsigned() {
		return 1;
	}

	void Compare_Greater_Bad() {
		const bool res = (getIntUnsigned() > getIntSigned());
		std::cout << std::boolalpha << res << std::endl;
	}

	void Compare_Greater_Better() {
		const bool res = (static_cast<int>(getIntUnsigned()) > getIntSigned());
		std::cout << std::boolalpha << res << std::endl;
	}

	void Compare_Greater_Perfect() {
		constexpr bool res = std::cmp_greater(getIntUnsigned(), getIntSigned());
		std::cout << std::boolalpha << res << std::endl;
	}

	void Tests() {
		static_assert(std::cmp_equal(1, 1));
		static_assert(std::cmp_not_equal(2, 1));
	}
}

namespace Utilities
{
    void In_Range()
    {
        std::cout << std::boolalpha;

        std::cout << std::in_range<std::size_t>(-1) << '\n';
        std::cout << std::in_range<std::size_t>(42) << '\n';
    }

}

void Utilities::TestAll()
{
	// ApplyTests::Apply_Sum_Test();
	// ApplyTests::Apply_Sum_Lambda_Test();
	// ApplyTests::Apply_Sum_Tuple();

	// Invoke_Tests::Access_Member();
	// Invoke_Tests::Call_Class_Method();
	// Invoke_Tests::Is_Invocable();
	// Invoke_Tests::Invote_Functor();

	// Make_Tuples::Test();
	// Make_Tuples::Test2();

	// Utilities_Library::Ptrdiff_t();

    In_Range();

    /*
	Integer_Comparison_Functions::Compare_Greater_Bad();
	Integer_Comparison_Functions::Compare_Greater_Better();
	Integer_Comparison_Functions::Compare_Greater_Perfect();
	Integer_Comparison_Functions::Tests();
    */
};
