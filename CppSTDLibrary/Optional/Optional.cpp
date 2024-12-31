/**============================================================================
Name        : Optional.cpp
Created on  : 19.08.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <string>
#include <cassert>
#include <complex>
#include <list>
#include <utility>
#include <vector>
#include <optional>
#include <utility>
#include <functional>
#include <algorithm>
#include <format>

#include "../Integer/Integer.h"
#include "Optional.h"

namespace Optional
{
	template<typename T>
	std::ostream& operator<<(std::ostream& stream, const std::vector<T>& vect)
	{
		for (const auto& v : vect)
			stream << v << " ";
		return stream;
	}

	class UserName
	{
		std::string mName;

	public:
		explicit UserName(std::string str) : mName(std::move(str)) {
			std::cout << "UserName::UserName(\'";
			std::cout << mName << "\')\n";
		}

		~UserName() {
			std::cout << "UserName::~UserName(\'";
			std::cout << mName << "\')\n";
		}
	};


	struct UserRecord
	{
		UserRecord(std::string  name,
			const std::optional<std::string>& nick,
			const std::optional<int> age) : mName{std::move( name )}, mNick{ nick }, mAge{ age } {
		}

		friend std::ostream& operator<< (std::ostream& stream, const UserRecord& user);

	private:
		std::string mName;
		std::optional<std::string> mNick;
		std::optional<int> mAge;
	};

	std::ostream& operator << (std::ostream& stream, const UserRecord& user)
	{
		stream << user.mName << ' ';
		if (user.mNick) {
			stream << *user.mNick << ' ';
		}
		if (user.mAge)
			stream << "age of " << *user.mAge;
		return stream;
	}

	/////////////////////////////////////////////////////////////////////////////////

	std::optional<std::string> optional_from_string(const std::string& value) {
		return (false == value.empty()) ? std::optional<std::string>{value} : std::nullopt;
	}

	/** std::reference_wrapper may be used to return a reference. **/
	std::optional<std::reference_wrapper<std::string>> optional_reference_from_string(const std::string& value) {
		static std::string tmp = value;
		return (false == value.empty()) ? std::optional<std::reference_wrapper<std::string>>{tmp} : std::nullopt;
	}

	void OptionalCreation()
	{
		// empty:
		std::optional<int> oEmpty;
		constexpr std::optional<float> optional_float = std::nullopt;

		std::cout << "optional_float = " << optional_float.value_or(0) << std::endl;

		// direct:
		constexpr std::optional<int> optional_int(10);
		constexpr std::optional optional_int_deduced(10); // deduction guides


		std::cout << "optional_int = " << optional_int.value() << std::endl;
		std::cout << "optional_int_deduced = " << optional_int_deduced.value() << std::endl;


		// make_optional
		constexpr auto optional_double = std::make_optional(3.0);
		const auto optional_complex = std::make_optional<std::complex<double>>(3.0, 4.0);

		std::cout << "optional_double = " << optional_double.value() << std::endl;
		std::cout << "optional_complex = " << optional_complex.value() << std::endl;

		// in_place
		const std::optional<std::complex<double>> optional_complex_double{ std::in_place, 3.0, 4.0 };
		std::cout << "optional_complex_double = " << optional_complex_double.value() << std::endl;

		// will call vector with direct init of {1, 2, 3}
		std::optional<std::vector<int>> oVec(std::in_place, { 1, 2, 3 });

		// copy/assign:
		constexpr auto optional_int_copy = optional_int;
		std::cout << "optional_int_copy = " << optional_int_copy.value() << std::endl;
	}


	void Create_In_Place()
	{
		const std::optional<std::vector<int>> v( { 1, 2, 3 });
		std::cout << v.value() << std::endl;
	}

	std::optional<const char*> maybe_getenv(const std::string& variable)
	{
        if (const char* env_p = std::getenv(variable.data()))
            return env_p;
		return std::nullopt;
	}

	void ValueOR_Tests() {
		std::cout << maybe_getenv("PWD").value_or("(none)") << std::endl;
		std::cout << maybe_getenv("JAVA_HOME").value_or("(none)") << std::endl;
	}

	void OptionalCreation_Test2() {
		std::cout << "optional_from_string('') returned " << optional_from_string("").value_or("empty") << std::endl;
		std::cout << "optional_from_string('Some_Value') returned " << optional_from_string("Some_Value").value_or("empty") << '\n' << std::endl;

		// optional-returning factory functions are usable as conditions of while and if
		if (const auto str = optional_from_string("Some_Existing_Test_Value")) {
			std::cout << "optional_from_string('Some_Existing_Test_Value') returned " << *str << std::endl;
		}

		if (const auto str = optional_from_string("")) {
			std::cout << "optional_from_string('') returned " << *str << std::endl;
		}
		else {
			std::cout << "optional_from_string('') returned " << str.value_or("std::nullopt") << '\n' << std::endl;
		}

		if (const auto str = optional_reference_from_string("Srting_Value_(Reference)")) {
			// using get() to access the reference_wrapper's value
			std::cout << "optional_reference_from_string('Srting_Value_(Reference)') returned " << str->get() << std::endl;
			str->get() = "Mothra";
			std::cout << "modifying it changed...." << std::endl;
			std::cout << "The NEW value is " << str->get() << std::endl;
		}

		std::cout << std::endl;

		auto opt_var = optional_from_string("Some_new_test_string_value");
		std::cout << "opt_var = " << opt_var.value() << ".   Updating to NEW_VALUE" << std::endl;
		opt_var.emplace("NEW_VALUE");
		std::cout << "opt_var = " << opt_var.value() << std::endl;
	}

	void GetParamValue() {
		// by operator*
		std::optional<int> optional_int = 12;
		std::cout << "optional_int " << *optional_int << std::endl;

		// by value()
		std::optional<std::string> optional_string_ok("hello");
		std::optional<std::string> optional_string_null = std::nullopt;

		try {
			std::cout << "option_string_ok = " << optional_string_ok.value() << std::endl;
		}
		catch (const std::bad_optional_access& exc) {
			std::cout << exc.what() << std::endl;
		}
		try {
			std::cout << "optional_string_null = " << optional_string_null.value() << std::endl;
		}
		catch (const std::bad_optional_access& exc) {
			std::cout << exc.what() << std::endl;
		}

		// by value_or()
		constexpr std::optional<double> optional_double; // empty
		std::cout << "option_double = " << optional_double.value_or(10.3) << std::endl;


		std::cout << "Is 'optional_string_null' has real value : " << optional_string_null.has_value() << std::endl;
	}


	std::optional<std::string> getOptString(const bool ok = false, const std::string& value = "") {
		return true == ok ? std::make_optional<std::string>(value) : std::nullopt;
	}
	
	void GetParamValue_ByPtr() {
		if (const auto result = getOptString(true, "SomeTestValue"); result) {
			std::cout << *result << std::endl;
		} else {
			std::cout << "Value is no set" << std::endl;
		}

		if (const auto result = getOptString(); result) {
			std::cout << *result << std::endl;
		} else {
			std::cout << "Value is no set" << std::endl;
		}
	}

	void CheckParamValue() {
		const std::list<std::string> values = { "SOME_VALUE", "" };

		std::cout << "New style: " << std::endl;

		for (const std::string& val : values) {
			if (auto ostr = optional_from_string(val); ostr)
				std::cout << "ostr " << *ostr << std::endl;
			else
				std::cout << "ostr is null" << std::endl;
		}

		// Old Style:

		std::optional<std::string> ostr;
		for (const std::string& val : values) {
			if (std::nullopt != (ostr = optional_from_string(val)))
				std::cout << "ostr " << *ostr << std::endl;
			else
				std::cout << "ostr is null" << std::endl;
		}
	}

	void ChangeOptParamValue() {
		std::optional<UserName> oEmpty;

		std::cout << "\n>>    emplace('Steve')" << std::endl;
		oEmpty.emplace("Steve");

		std::cout << "\n>>    emplace('Mark')" << std::endl;
		oEmpty.emplace("Mark");

		std::cout << "\n>>    reset()" << std::endl;
		oEmpty.reset(); // ~Mark() invoked
		// Same as oEmpty = std::nullopt;

		std::cout << "\n>>    emplace('Fred')" << std::endl;
		oEmpty.emplace("Fred");


		std::cout << "\n>>    oEmpty = UserName('Joe')" << std::endl;
		oEmpty = UserName("Joe");
	}

	void CompareValues()
	{
		constexpr std::optional<int> oEmpty;
		constexpr std::optional<int> oTwo(2);
		constexpr std::optional<int> oTen(10);

		std::cout << std::boolalpha;
		std::cout << (oTen > oTwo) << std::endl;
		std::cout << (oTen < oTwo) << std::endl;
		std::cout << (oEmpty < oTwo) << std::endl;
		std::cout << (oEmpty == std::nullopt) << std::endl;
		std::cout << (oTen == 10) << std::endl;

		// We should get crashed here
		assert(oTen != 10);
	}


	void UserWithOptionalName()
	{
		const UserRecord tim{ "Tim", "SuperTim", 16 };
		const UserRecord nano{ "Nathan", std::nullopt, std::nullopt };

		std::cout << tim << "\n";
		std::cout << nano << "\n";
	}


	std::optional<int> ParseInt(const std::string_view str)
	{
		try {
			return std::stoi(str.data());
		}
		catch (...) {
			std::cout << "cannot convert '" << str << "' to int!" << std::endl;
		}
		return std::nullopt;
	}

	void Options_ParseIntTest()
	{
		using namespace std::string_view_literals;

		for (const auto& [first, second]: std::array<std::pair<std::string_view, std::string_view>, 2>{
			std::pair{"2"sv, "3"sv}, std::pair{"2"sv, "A"sv}})
		{
			const std::optional<int> oFirst = ParseInt(first);
			if (const std::optional<int> oSecond = ParseInt(second); oFirst && oSecond)
			{
				std::cout << *oFirst << " + " << *oSecond << " = " << *oFirst + *oSecond << "\n";
			}
		}
	}

	void ChangeValues()
	{
		std::cout << "----------------------- Test #1 ------------------------\n" << std::endl;
		{
			std::optional<Integer> optInteger = std::make_optional<Integer>(111);
		}

		std::cout << "\n----------------------- Test #2 ------------------------\n" << std::endl;
		{
			std::optional<Integer> optInteger = std::make_optional<Integer>(Integer(222));
		}

		std::cout << "----------------------- Test #3 (Emplace) ------------------------\n" << std::endl;
		{
			std::optional<Integer> optInteger = std::make_optional<Integer>(111);
			optInteger.emplace(222);
		}

		std::cout << "----------------------- Test #4 (Swap) ------------------------\n" << std::endl;
		{
			std::optional<Integer> optInteger1 = std::make_optional<Integer>(111);
			std::optional<Integer> optInteger2 = std::make_optional<Integer>(222);
			optInteger1.swap(optInteger2);

		}
	}

	std::optional<std::reference_wrapper<std::string>> GetOptStr() {
		std::string test = "123456789";
		std::optional<std::reference_wrapper<std::string>> result;
		result.emplace(test);

		std::cout << "has values: " << std::boolalpha << result.has_value() << std::endl;
		if (true == result.has_value()) {
			std::cout << "Value: " << result.value().get() << std::endl;
		}

		return result;
	}

	void Optional_Reference_Wrapper()
	{
		const std::optional<std::reference_wrapper<std::string>> result = GetOptStr();
		std::cout << "has values: " << std::boolalpha << result.has_value() << std::endl;
		if (true == result.has_value()) {
			std::cout << "Value: " << result.value().get() << std::endl;
		}
	}

	////////////////////////////////////////////////////////////////////////


	struct Point
	{
		Point(const int a, const int b) : x(a), y(b) { }
		int x { 0 };
		int y { 0 };
	};


	void Construct_IN_PLACE()
	{
		std::optional<Point> opt1{ std::in_place, 0, 0 };
		std::optional<Point> opt = std::make_optional<Point>(0, 0);
	}

	////////////////////////////////////////////////////////////////////////


	void VariousTests() {
		
		/*
		if (auto result = getOptString(true); result) {
			std::cout << *result << std::endl;
		}
		*/

		{
			std::optional<Integer> ineger (1);
		}
	}
}

namespace Optional::Applications {

	std::optional<std::string> GetEnvironmentVariable(const std::string& variable) {
        return getenv(variable.data());
	}

	void ReadEnvironment() {

		for (const auto& name : {"Path", "SystemRoot", "OS", "GGGG"}) {
			std::optional<std::string> pathEnv = GetEnvironmentVariable(name);
			std::cout << name <<  " = " << pathEnv.value_or("None") << "\n" << std::endl;
		}
	}
}


namespace MonadicOperations
{
    const std::unordered_map<int, std::string> cache {
            {1, "I"},
            {2, "II"},
            {3, "III"},
            {4, "IV"},
            {5, "V"},
    };

    const std::unordered_map<int, std::string> database {
            {1, "I"},
            {2, "II"},
            {3, "III"},
            {4, "IV"},
            {5, "V"},
            {6, "VI"},
            {7, "VII"},
            {8, "VIII"},
            {9, "IX"},
            {10, "X"},
    };

	std::optional<std::string> getUserInput()
	{
		std::string input;
		std::cout << "Enter your name: ";
		std::getline(std::cin, input);
		if (input.empty())
			return std::nullopt;
		return input;
	}

    std::optional<std::string> getFromCache(const int key)
    {
	    if (const auto iter = cache.find(key); cache.end() != iter)
            return std::make_optional<std::string>(iter->second);
        return std::nullopt;
    }

    std::optional<std::string> getFromDatabase(const int key)
    {
	    if (const auto iter = database.find(key); database.end() != iter)
            return std::make_optional<std::string>(iter->second);
        return std::nullopt;
    }

    std::optional<std::string> decorate(const std::string& str)
    {
        return std::format("[{}]", str);
    }

    std::string toUpper(const std::string& str) {
        std::string tmp {str};
        std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);
        return tmp;
    }

    template<typename Ty>
    std::ostream& operator<<(std::ostream& stream, const std::optional<Ty>& opt)
    {
        if (opt.has_value())
            stream << opt.value();
        else
            stream <<  "NullOpt";
        return stream;
    }

    std::optional<int> getAge(int v)
    {
        if (100 > v)
            return std::make_optional<int>(v);
        return std::nullopt;
    }

    void OrElse_Test()
    {
        for (int val: {3, 7})
        {
            const std::optional<std::string> result = getFromCache(val)
                    .or_else([&]() { return getFromDatabase(val); });

            std::cout << val << " --> " << result << std::endl;
        }

        std::cout << std::endl;

        for (int val: {3, 7, 13})
        {
            const std::optional<std::string> result = getFromCache(val)
                    .or_else([&]() { return getFromDatabase(val); })
                    .or_else([&]() { return std::make_optional<std::string>("None"); });

            std::cout << val << " --> " << result << std::endl;
        }
    }

    void AndThen_Test()
    {
        for (const int val: {3, 7})
        {
            const std::optional<std::string> result = getFromCache(val)
                    .and_then(decorate);
            std::cout << val << " --> " << result << std::endl;
        }

        for (const int val: {3, 7})
        {
            const std::optional<std::string> result = getFromCache(val)
                    .and_then([](const std::string& str) { return std::make_optional<std::string>("OK"); });
            std::cout << val << " --> " << result << std::endl;
        }
    }

    void Transform_Test()
    {
        {
	        constexpr std::optional<int> number = 5;
            const std::optional<int> squared = number.transform([](int x) { return x * x; });
            std::cout << number << " --> " << squared << std::endl;
        }

        {
            const std::optional<std::string> text = "qwerty";
            const std::optional<std::string> upperCase = text.transform(toUpper);
            std::cout << text << " --> " << upperCase << std::endl;
        }
    }

    void OrElse_Transform()
    {
		for (const int age: {10, 56, 123})
		{
			const int value = getAge(age)
				.or_else([]() { return std::make_optional<int>(18); })
				.transform([](const int v) { return v + 1; }).value();
			std::cout << value << std::endl;
		}
    }


	void Transform_UserInput()
	{
		const std::optional<std::string> result = getUserInput()
			.transform([](std::string name) {
				std::transform(name.begin(), name.end(), name.begin(), ::toupper);
				return name;
			})
			.and_then([](const std::string& name) {
				if (name == "ADMIN")
					return std::optional<std::string>("Welcome, Admin!");
				return std::optional<std::string>("Hello, " + name + "!");
			})
			.or_else([] {
				return std::optional<std::string>("No input provided.");
			});

		std::cout << *result << "\n";
	}

}


void Optional::TestAll()
{
	// OptionalCreation();
	// OptionalCreation_Test2();
	// Create_In_Place();

	// GetParamValue();
	// GetParamValue_ByPtr();

	// CheckParamValue();
	// ChangeOptParamValue();
	// CompareValues();
	// UserWithOptionalName();
	// Options_ParseIntTest();

	// ValueOR_Tests();
	// ChangeValues();

	// Optional_Reference_Wrapper();

	// VariousTests();

	// Applications::ReadEnvironment();


    // MonadicOperations::OrElse_Test();
    // MonadicOperations::AndThen_Test();
    // MonadicOperations::Transform_Test();
    // MonadicOperations::OrElse_Transform();
    MonadicOperations::Transform_UserInput();
};

