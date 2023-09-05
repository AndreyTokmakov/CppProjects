//============================================================================
// Name        : ConstConstexprMutable.cpp
// Created on  : 09.08.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Const && Constexpr && Mutable src
//============================================================================

#include "ConstConstexprMutable.h"

#include <array>
#include <algorithm>
#include <string_view>
#include <stdexcept>
#include <numeric>
#include <iostream>
#include <string>
#include <cassert>
#include <utility>
#include <cstdint>

namespace ConstConstexprMutable::Compile_Time_IF {

	template <typename T>
	auto GetValue(T t) {
		if constexpr (std::is_pointer<T>::value) {
			return *t;
		}
		else {
			return t;
		}
	}

	void GetValueFromPointer() {
		int value = 123;
		int* prtVal = &value;

		std::cout << GetValue(value) << std::endl;
		std::cout << GetValue(prtVal) << std::endl;
	}

	//----------------------------------------------------------------------//

	template<typename T>
	typename std::enable_if<std::is_pointer<T>::value, std::remove_pointer_t<T>>::type GetValue2(T t) {
		return *t;
	}

	template<typename T>
	typename std::enable_if<!std::is_pointer<T>::value, T>::type GetValue2(T t) {
		return t;
	}

	void GetValueFromPointer2() {
		int value = 123;
		int* prtVal = &value;

		std::cout << GetValue2(value) << std::endl;
		std::cout << GetValue2(prtVal) << std::endl;
	}

	//----------------------------------------------------------------------//

	template<int N>
	constexpr bool is_negative() {
		if constexpr (N >= 0)
			return false;
		else
			return true;
	}

	template<typename T>
	constexpr T transform(T a) {
		return a * 2;
	}

	void IsNegative() {
		constexpr bool negative = is_negative<1>();
		std::cout << std::boolalpha << negative << std::endl;

		constexpr bool negative1 = is_negative<-1>();
		std::cout << std::boolalpha << negative1 << std::endl;

		static_assert(is_negative<-1>(), "OK");
		// static_assert(is_negative<1>(), "NOT NEGATIVE");
	}

	//----------------------------------------------------------------------//

	template <typename T>
	std::string toString(const T& x) {
		if constexpr (std::is_same_v<T, std::string>) {
			return x; // statement invalid, if no conversion to string
		}
		else if constexpr (std::is_arithmetic_v<T>) {
			return std::to_string(x); // statement invalid, if x is not numeric
		}
		else {
			return std::string(x); // statement invalid, if no conversion to string
		}
	}

	void ToStringTest() {
		{
			std::string value = "SomeText";
			std::string result = toString(value);
			std::cout << value << " -> " << result << std::endl;
		}
		{
			int value = 12345;
			std::string result = toString(value);
			std::cout << value << " -> " << result << std::endl;
		}
		{
			char value[] = "Some pain text";
			std::string result = toString(value);
			std::cout << value << " -> " << result << std::endl;
		}
	}

	template<typename T>
	void check_X(const T x)
	{
		if constexpr (auto obj = transform(x); std::is_same_v<decltype(obj), T>) {
			std::cout << "transform(x) yields same type\n";
		}
		else {
			// static_assert(false, "transform(x) yields different type");
		}
	}

	void CompileTimeIf_WithInitialization() {
		
		constexpr int x = 123;
		check_X(x);
	}
}

//**************************************************************************//

namespace ConstConstexprMutable::ConstexprFuncs {

	constexpr int _constexpr_sum(int a, int b) {
		return a + b;
	}

	void ConstexprSum() {
		constexpr auto __sq = [](auto x) constexpr -> decltype(x) { return x * x; };
		constexpr auto __sq_bad = [](int x) { return x * x; };

		constexpr int a1 = _constexpr_sum(5, 12);
		std::cout << "a1 = " << a1 << std::endl;

		constexpr int a2 = __sq(5);
		std::cout << "a2 = " << a2 << std::endl;

		// constexpr int s2 = __sq_bad(5);
	}

	//-------------------------------------------------------------------------//

	template <int N>
	struct Factorial {
		static constexpr int value = N * Factorial<N - 1>::value;
	};

	template <>
	struct Factorial<0> {
		static constexpr int value = 1;
	};

	void Factorial_Test() {
		std::cout << Factorial<5>::value << std::endl;
	}

	//-------------------------------------------------------------------------//

	constexpr auto sum( int a, int b) {
		constexpr auto result = std::is_constant_evaluated();
		return result;
	}

	void Check_IF_Constexpr() {
		constexpr auto result = sum(1, 2);
		std::cout << result << std::endl;
	}  



}

//**************************************************************************//

namespace ConstConstexprMutable::MutableTests {

	class ImmutableObject {
	private:
		unsigned int var1;
		mutable unsigned int var2;
		std::string info;

	public:
		void Increment_InConst_Method1() const {
#if 0
			var1++;
#endif
		}

		void Increment_InConst_Method2() const {
			var2++;
		}

		const std::string& getInfo_Const() {
			return this->info;
		}

		std::string& getInfo() {
			return this->info;
		}

	public:
		ImmutableObject() : ImmutableObject(0, 0, "TestInfo") {
		}

		ImmutableObject(unsigned int v1, 
			            unsigned int v2,
					    const std::string& text) : var1(v1), var2(v2), info(text) {
		}

		friend std::ostream& operator<< (std::ostream& stream, ImmutableObject& integer);
	};

	std::ostream& operator<< (std::ostream& stream, ImmutableObject& obj) {
		stream << "[var1 = " << obj.var1 << ", var2 = " << obj.var2 << ", Info: " << obj.info << "]";
		return stream;
	}

	void Update_ImmutableObject() {
		ImmutableObject obj;
		std::cout << obj << std::endl;

		obj.Increment_InConst_Method2();
		std::cout << obj << std::endl;

		obj.getInfo() = "New_Value_1";
		std::cout << obj << std::endl;
#if 0
		obj.getInfo_Const() = "New_Value_1";
		std::cout << obj << std::endl;
#endif
	}
}

//**************************************************************************//

namespace ConstConstexprMutable::Const_Pointers {

	/* Non const teste */
	void NonConstTest() { 
		int int_var = 123;
		int* ptr1 = &int_var;

		std::cout << *ptr1 << std::endl;
		int_var = 222;
		std::cout << *ptr1 << std::endl;
	}

	/* Non const test. */
	void ConstPointerValue() {
		const int  var = 123;
		const int* ptr = &var;

		std::cout << "Value: " <<  *ptr << ". Address: " << ptr << std::endl;
#if 0
		// ERROR - can not change 'var' value
		*ptr1 = 222; 
#endif

		std::cout << "Value: " << *ptr << ". Address: " << ptr << std::endl;
		ptr++; // ITS ALLOWED
		std::cout << "Value: " << *ptr << ". Address: " << ptr << std::endl;
	}

	void ConstPointer() {
		int  var = 123;
		int* const ptr = &var;

		std::cout << "Value: " << *ptr << ". Address: " << ptr << std::endl;
		*ptr = 222; // ITS OK
		std::cout << "Value: " << *ptr << ". Address: " << ptr << std::endl;
#if 0
		// ERROR: you cannot assign to a variable that is const
		// ptr -> is const we cant change it

		// ptr1++;  
#endif

#if 0
		// ERROR: you cannot assign to a variable that is const
		int var2 = 123;
		ptr = &var2;
#endif
	}
}

namespace ConstConstexprMutable::ConstexprMap {
	

	template <typename K, typename V, std::size_t Size>
	class Map {
	public:
		std::array<std::pair<K, V>, Size> data;

		[[nodiscard]]
		constexpr V at(const K &key) const {
			const auto res = std::find_if(data.begin(), data.end(), [&key](const auto &v) {
				return v.first == key;
			});
			if (data.end() != res) {
				return res->second;
			}
			else {
				throw std::range_error("Not Found");
			}
		}
	};

	using namespace std::literals::string_view_literals;
	static constexpr std::array<std::pair<std::string_view, int>, 8> color_values {
		{{"black"sv, 7},
		 {"blue"sv, 3},
		 {"cyan"sv, 5},
		 {"green"sv, 2},
		 {"magenta"sv, 6},
		 {"red"sv, 1},
		 {"white"sv, 8},
		 {"yellow"sv, 4}} };


	int lookup_value(const std::string_view sv) {
		//static const auto map = std::map<std::string_view, int>{color_values.begin(), color_values.end()};
		static constexpr auto map = Map<std::string_view, int, color_values.size()>{ {color_values} };

		return map.at(sv);
	}

	void Test() {
		static constexpr auto testMap = Map<std::string_view, int, color_values.size()>{ {color_values} };
		auto val = testMap.at("green");
		std::cout << val << std::endl;
	}
}


namespace ConstConstexprMutable::ConstexprArray {

	using namespace std::literals::string_view_literals;

	using T = std::pair<int, std::string_view>;
	static constexpr std::array<T, 5> values{ {
		{1, "one"sv},
		{2, "two"sv},
		{3, "three"sv},
		{4, "four"sv},
		{5, "five"sv}
	} };

	consteval std::string_view get(int i) {
		return values[i].second;
	}

	void Test() {
		constexpr auto name = get(3);
		std::cout << name << std::endl;

#if 0
		constexpr auto name = get(values.size() + 1);
#endif
	}

}


namespace ConstConstexprMutable::ConstexprSwitch {

	enum class Method {
		None = 0,
		GET = 1,
		HEAD = 2,
		POST = 3,
		PUT = 4,
		DELETE = 5,
		CONNECT = 6,
		OPTIONS = 7,
		TRACE = 8,
		PATCH = 9
	};


	class Map {
	public:
		using T = std::pair<std::string_view, Method>;
		static inline constexpr std::array<T, 10> http_methods_mapping{ {
			{std::string_view("None"),    Method::None},
			{std::string_view("GET"),     Method::GET},
			{std::string_view("HEAD"),    Method::HEAD},
			{std::string_view("POST"),    Method::POST},
			{std::string_view("PUT"),     Method::PUT},
			{std::string_view("DELETE"),  Method::DELETE},
			{std::string_view("CONNECT"), Method::CONNECT},
			{std::string_view("OPTIONS"), Method::OPTIONS},
			{std::string_view("TRACE"),   Method::TRACE},
			{std::string_view("PATCH"),   Method::PATCH}
		} };

	public:
		[[nodiscard]]
		constexpr Method get_by_name(const std::string& key) const {
			const auto res = std::find_if(http_methods_mapping.begin(), http_methods_mapping.end(), [&key](const auto& v) {
				return v.first == key;
				});
			if (http_methods_mapping.end() != res) {
				return res->second;
			}
			return Method::None;
		}

		[[nodiscard]]
		constexpr Method get_by_name(std::string_view key) const {
			if (const auto res = std::find_if(http_methods_mapping.begin(), http_methods_mapping.end(), [&key](const auto& v) {
				return v.first == key;}); http_methods_mapping.end() != res) {
				return res->second;
			}
			return Method::None;
		}

		[[nodiscard]]
		constexpr std::string_view get_by_method(Method method) const {
			if (const auto res = std::find_if(http_methods_mapping.begin(), http_methods_mapping.end(), [&method](const auto& v) {
				return v.second == method; }); http_methods_mapping.end() != res) {
				return res->first;
			}
			return std::string_view("None");
		}
	};

	static constexpr auto mapping { Map() };

	Method getMetchod(const std::string& name) {
		return mapping.get_by_name(name);
	}

	consteval Method getMetchod(std::string_view name) {
		return mapping.get_by_name(name);
	}

	consteval std::string_view getName(Method method) {
		return mapping.get_by_method(method);
	}

	void TEST() {
		// constexpr Method method1 = getMetchod(std::string_view("GET"));
		// std::cout << getName(method1) << std::endl;

		constexpr auto name = getName(Method::CONNECT);
		std::cout << name << std::endl;

		constexpr auto method = getMetchod(name);
		std::cout << ((Method::CONNECT == method) ? "Method::CONNECT" : "None") << std::endl;
		
	}

}

namespace ConstConstexprMutable::ConstexprObjects {

	class Point3D {
		const int x;
		const int y;
		const int z;

	public:
		constexpr Point3D(const int x = 0,
					      const int y = 0,
			              const int z = 0) : x{ x }, y{ y }, z{ z } { }

		constexpr int getX() const { 
			return x; 
		}
		constexpr int getY() const { 
			return y; 
		}
		constexpr int getZ() const { 
			return z; 
		}

		friend std::ostream& operator<<(std::ostream& stream, const Point3D& pt);
	};

	std::ostream& operator<<(std::ostream& stream, const Point3D & pt) {
		stream << "[" << pt.x << ", " << pt.y << ", " << pt.z << "]" << std::endl;
		return stream;
	}

	void Test() {
		constexpr Point3D point(10, 10, 10);
		std::cout << point << std::endl;
	}
}

namespace ConstConstexprMutable::Strings {

	using namespace std::string_view_literals;

	void Constexpr_Strings() {
		// constexpr std::string str {"Some_String"};

		constexpr char token[]{"Some_String"};
		constexpr std::string_view str_view = "Some_String"sv;


		std::cout << token << std::endl;
		std::cout << str_view << std::endl;
	}

	/*
	consteval std::string buildString() {
		return "Some_String";
	}
	
	void Constexpr_Strings2() {
		constinit std::string str = buildString();
	}
	*/
}

namespace ConstConstexprMutable::Algoritms {

	void Accumulate() {
		constexpr std::array myArray{ 1, 2, 3, 4, 5 };     
		constexpr auto sum = std::accumulate(myArray.begin(), myArray.end(), 0);  
		std::cout << "sum: " << sum << std::endl;

		constexpr auto product = std::accumulate(myArray.begin(), myArray.end(), 1,      // (3)
			std::multiplies<int>());
		std::cout << "product: " << product << std::endl;

		constexpr auto product2 = std::accumulate(myArray.begin(), myArray.end(), 1,     // (4)
			[](auto a, auto b) { return a * b; });
		std::cout << "product2: " << product2 << std::endl;

		std::cout << std::endl;
	}

	//---------------------------------------------------------------------------

	void Sort_at_CompileTime() {
		auto sort_constexpr = [](auto data) constexpr -> decltype(data) {
			std::sort(std::begin(data), std::end(data));
			return data;
		};

		constexpr auto sorted_array = sort_constexpr(std::array<int, 5>{5, 4, 3, 2, 1});
		// constexpr auto unsorted_array = std::array<int, 5>{5,4,3,2,1};

		static_assert(std::is_sorted(std::begin(sorted_array), std::end(sorted_array)),
			"Input array is not sored.");
	}

	void Sort_at_CompileTime_Consteval() {
		auto sort_constexpr = [](auto data) consteval -> decltype(data) {
			std::sort(std::begin(data), std::end(data));
			return data;
		};

		constexpr auto sorted_array = sort_constexpr(std::array<int, 5>{5, 4, 3, 2, 1});
		// constexpr auto unsorted_array = std::array<int, 5>{5,4,3,2,1};

		static_assert(std::is_sorted(std::begin(sorted_array), std::end(sorted_array)),
			"Input array is not sored.");
	}
}


namespace ConstConstexprMutable::Consteval {

	consteval const std::string_view __buildString() {
		return "Some_String";
	}

	void GetConstString() {
		constexpr std::string_view text = __buildString();
		std::cout << text << std::endl;
	}

	//------------------------------------------------------------

	/*
	consteval int __fibonachi(const int N) {
		if constexpr (N <= 1)
			return N;
		else
			return __fibonachi(N - 1) + __fibonachi(N - 2);
	}

	template<size_t N>
	constexpr int __fibonachi_old() {
		if constexpr (N <= 1)
			return N;
		return __fibonachi_old<N - 1>() + __fibonachi_old<N - 2>();
	}

	template<int  N>
	constexpr int fibonacci() { 
		return fibonacci<N - 1>() + fibonacci<N - 2>(); 
	}

	template<>
	constexpr int fibonacci<1>() { 
		return 1; 
	}

	template<>
	constexpr int fibonacci<0>() {
		return 0; 
	}

	void Fibonachi_Old_Test() {
		constexpr int x = fibonacci<10>();
		std::cout << x << std::endl;

		constexpr int x2 = __fibonachi_old<2>();
		std::cout << x2 << std::endl;
	}
    */


    //------------------------------------------------------------


	consteval int __sqrt(int n) {
		return n * n;
	}

	void SimpleTests() {
		static_assert(__sqrt(10) == 100);
	}

	//-------------------------------------------------------------------------//

	consteval int multTwo(int a) {
		return a * 2;
	}

	void ConstInit_Block() {

		/*
		consteval {
			auto a = multTwo(123);
		}
		*/
	}
}


namespace ConstConstexprMutable::Constexpr_Tests {

	template <std::size_t N>
	constexpr int naiveSumArray() {
		std::array<int, N> data{ 0 };
		std::iota(data.begin(), data.begin() + N, 1);
		return std::accumulate(data.cbegin(), data.cend(), 0);
	}


	constexpr int smartSum(unsigned int n) {
		return (n * (1 + n)) / 2;
	}

	void Test_Array_Sum() {
		static_assert(naiveSumArray<10>() == smartSum(10));
		static_assert(naiveSumArray<11>() == smartSum(11));
		static_assert(naiveSumArray<23>() == smartSum(23));
	}

	//----------------------------------------------


	constexpr int naiveSumArray_Dynamic(unsigned int n) {
		auto p = new int[n];
		std::iota(p, p + n, 1);
		auto tmp = std::accumulate(p, p + n, 0);
		delete[] p;
		return tmp;
	}

	void Test_Array_Sum_Dynamic() {
#if 0   // Only with C++20
		static_assert(naiveSumArray_Dynamic<10>() == smartSum(10));
		static_assert(naiveSumArray_Dynamic<11>() == smartSum(11));
		static_assert(naiveSumArray_Dynamic<23>() == smartSum(23));
#endif
	}
}

namespace As_Const {

	void String_As_Const() 
	{
		std::string mutableString = "Hello World!";
		const std::string& constRef = std::as_const(mutableString);

		assert(&constRef == &mutableString);
		assert(&std::as_const(mutableString) == &mutableString);

		// ERROR
		// constRef[1] = 's';

		// ERROR
		// constRef.clear();
	}
}

namespace Constexpr_STL_Containers {

    /*
	constexpr bool testVector(int n)
    {
		std::vector<int> vec(n, 1);

		int sum = 0;
		for (auto& elem : vec)
			sum += elem;

		return n == sum;
	}

	void Test_Vector() {
		static_assert(testVector(10));
	}*/

	//-------------------------------------------------------------------

	void Test_Strings() {
		// constexpr std::string text ("dd");
	}
}


namespace Tests {

	constexpr int getValue() {
		return 1;
	}

	void ForLoop() {
		if constexpr (constexpr auto v = getValue(); v > 0) {
			std::cout << "OK" << std::endl;
		}
	}

	//----------------------------------------------------------------------------

	template <typename K = std::string_view, typename V = int, std::size_t Size = 8>
	class Map {
	public:
		static constexpr std::array<std::pair<K, V>, Size> data{ {
			{"black", 7},
			{"blue", 3},
			{"cyan", 5},
			{"green", 2},
			{"magenta", 6},
			{"red", 1},
			{"white", 8},
			{"yellow", 4}
		} };

		[[nodiscard]]
		constexpr V get(const K& key) const {
			constexpr auto res = std::find_if(
				data.begin(), data.end(), [&](const auto& v) {return v.first == key; }
			);

			if constexpr (data.end() != res) {
				return res->second;
			}
			else {
				static_assert(data.end() != res);
			}

			return 0;
		}
	};

	using namespace std::literals::string_view_literals;

	void TestMap()
	{
		// constexpr Map map;
		// constexpr auto v = map.get("green"sv);
	}

	//-------------------------- Objects -------------------------------//


	struct Integer
	{
		int v{ 0 };

		constexpr Integer(int val = 0) : v{ val } {
		}
	};

	void Test_Constexpr_Integer() {

		constexpr Integer v;
	}
}

namespace ConstConstexprMutable::Returning_ConstExpr_Array_SizeAsParameter
{
    void createArray()
    {
        constexpr auto func = [](auto size)
        {
            return std::array<int, size> {};
        };

        /** Will not compile **/
        // constexpr std::array f = func(42);

        constexpr std::array f = func(std::integral_constant<int, 42>{});
    }
}

void ConstConstexprMutable::TestAll()
{

    // ConstexprMap::Test();

	// ConstexprArray::Test();

	// ConstexprSwitch::TEST();
	
	//ConstexprObjects::Test();

	//------------------------------------------------------------------------------------//

	// ConstexprFuncs::ConstexprSum();
	// ConstexprFuncs::Factorial_Test();
	// ConstexprFuncs::Check_IF_Constexpr();


	// Constexpr_Tests::Test_Array_Sum();

	//-------------------------------------------------------------------------------------//

	// Strings::Constexpr_Strings();

	// Algoritms::Accumulate();
	// Algoritms::Sort_at_CompileTime();
	// Algoritms::Sort_at_CompileTime_Consteval();

	// Compile_Time_IF::GetValueFromPointer();
	// Compile_Time_IF::GetValueFromPointer2();
	// Compile_Time_IF::IsNegative();
	// Compile_Time_IF::ToStringTest();
	// Compile_Time_IF::CompileTimeIf_WithInitialization();
	  
	// MutableTests::Update_ImmutableObject();

	// Const_Pointers::NonConstTest();
	// Const_Pointers::ConstPointer();
	// Const_Pointers::ConstPointerValue();

	//---------------------------------- STL Constexpr Containers ----------------------------------//

	// Constexpr_STL_Containers::Test_Vector();


	//---------------------------------- ConstEval: ------------------------------------------------//

	// Consteval::GetConstString();
	// Consteval::SimpleTests();
	// Consteval::Fibonachi_Old_Test();


	// As_Const::String_As_Const();


	// Tests::ForLoop();
	// Tests::Test_Constexpr_Integer();


    Returning_ConstExpr_Array_SizeAsParameter::createArray();
};
