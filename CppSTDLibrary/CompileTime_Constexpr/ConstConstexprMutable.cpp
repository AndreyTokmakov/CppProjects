//============================================================================
// Name        : ConstConstexprMutable.cpp
// Created on  : 09.08.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Const && Constexpr && Mutable src
//============================================================================

#include "ConstConstexprMutable.hpp"

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
#include <vector>
#include <random>

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

namespace ConstConstexprMutable::ConstexprFuncs
{

	constexpr int constexpr_sum(int a, int b) {
		return a + b;
	}

	void ConstexprSum()
	{
		[[maybe_unused]]
		constexpr auto sq = []<typename T>(T x) constexpr -> T { return x * x; };

		[[maybe_unused]]
		constexpr auto sq_bad = [](int x) { return x * x; };

		constexpr int a1 = constexpr_sum(5, 12);
		std::cout << "a1 = " << a1 << std::endl;

		constexpr int a2 = sq(5);
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

namespace ConstConstexprMutable::MutableTests
{

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


namespace ConstConstexprMutable::CompileTimeComputations
{
    template <ino64_t N>
    struct Factorial
    {
        enum : ino64_t { value = Factorial<N-1>::value * N };

        static void create_sequence(std::vector<ino64_t>& v) {
            Factorial<N-1>::create_sequence(v);
            v.emplace_back(value);
        }
    };

    template<>
    struct Factorial<0LL>
    {
        enum : ino64_t { value = 1 };

        static void create_sequence(std::vector<ino64_t>& v) {
            v.emplace_back(value);
        }
    };

    void FibonacciSequence_ToVector()
    {
        std::vector<ino64_t> sequence;
        sequence.reserve(5);
        Factorial<5>::create_sequence(sequence);

        for (const auto& v : sequence)
            std::cout << v << " ";
        std::cout << "\n";                         /// >> 1 1 2 6 24 120

        std::cout << Factorial<10>::value << "\n"; /// >> 3628800
    }
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

    template<uint32_t N, class ... Nums>
    consteval uint32_t factorial(Nums ... values)
    {
        if constexpr (sizeof ... (values) == N)
        {
            return (values * ... * 1);
        }
        else
        {
            return factorial<N>(sizeof ... (values) + 1, values...);
        }
    }

    void Factorial_Test()
    {
        static_assert(1 == factorial<0>());
        static_assert(1 == factorial<1>());
        static_assert(6 == factorial<3>());
        static_assert(720 == factorial<6>());
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

		[[maybe_unused]]
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


namespace Tests
{

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

    	[[maybe_unused]]
        constexpr std::array f = func(std::integral_constant<int, 42>{});
    }
}

namespace Constexpr_Consteval
{
    std::string random_string_run_time(size_t size = 16)
    {
        std::random_device rd{};
        std::mt19937 generator = std::mt19937 {rd()};
        auto ud = std::uniform_int_distribution<> {(int)'a', (int)'z'};

        std::string str;
        str.reserve(size);
        while (size-- > 0)
            str.push_back(static_cast<char>(ud(generator)));
        return str;
    }

    size_t get_number_run_time()
    {
        std::random_device randomDevice {};
        std::mt19937 generator = std::mt19937 { randomDevice() };
        return std::uniform_int_distribution<int>{0, 100}(generator);
    }

    constexpr int divide(int a, int b)
    {
        if (b == 0) {
            printf("Divide by zero\n");
            return 0;
        } else {
            return a / b;
        }
    }

    /** This is a pure compile-time function. **/
    consteval size_t strlen_ct(const char* s) {
        size_t n = 0;
        for (; s[n] != '\0'; ++n);
        return n;
    }


    void Constexpr_Static_Vs_RunTime()
    {
        static_assert(divide(6, 3) == 2);  // Ok
        static_assert(divide(0, 3) == 0);  // Ok

        /** Error: call to runtime function `printf` **/
        // static_assert( divide(0, 0) == 0 );
    }

    void Call_Consteval_RunTime(const std::string& str)
    {
        strlen_ct(""); // OK. Allowed but only if all arguments are constant expressions.

        /** Call to consteval function 'Constexpr_Consteval::strlen_ct' is not a constant expression **/
        // strlen_ct(str.data());   // Compile error
    }
}

namespace ConstConstexprMutable::UndefinedBehaviour
{
    /**
     *  Compile-time constant expressions are not permitted to invoke undefined behaviour.
     *  This includes constexpr functions that are evaluated at compile-time.
     *  This property can be used to statically test code, ensuring that the code doesn't invoke undefined behaviour.
    **/

    constexpr int midpoint(int a, int b)
    {
        return (a + b)/2; // can overflow, int overlow is UB
    }

    constexpr int generate()
    {
        std::vector<int> data = {1};
        auto it = data.begin();
        for (int i = 0; i < 10; i++)
            data.push_back(i); // invalidates it
        return *it;            // accessing invalid iterator
        /** Local variable 'it' may point to invalidated memory **/
    }

    constexpr int process()
    {
        int* buffer = new int[10];
        for (int i = 0; i < 10; i++)
            buffer[i] = i;
        int sum = 0;
        for (int i = 0; i < 10; i++)
            sum += i;
        return sum; // we memory leak buffer
        /** Check that the noun 'memory' after the pronoun 'we' is correct.
         * It's possible that you may need to switch to a possessive pronoun, or use another part of speech **/
    }

    constexpr int cnt_space(const char* str, size_t sz)
    {
        int cnt = 0;
        for (size_t i = 0; i < sz ; ++i) {
            if (str[i] == ' ')
                ++cnt; // out-of-bounds
        }
        return cnt;

        /** Read of de-referenced one-past-the-end pointer is not allowed in a constant
         * expression static assertion expression is not an integral constant expression **/
    }

    void TryInvokeUB()
    {
        constexpr int a = std::numeric_limits<int>::max();

    	[[maybe_unused]]
        constexpr int b = a - 2;

    	[[maybe_unused]]
        constexpr int c = a - 1;


        // Wouldn't compile: "overflow in constant expression"
        // static_assert(midpoint(a, b) == c);

        // Wouldn't compile "use of storage after deallocation"
        // static_assert(generate() == 1);

        // Wouldn't compile "storage has not been deallocated"
        // static_assert(process() == 45);

        // Wouldn't compile "array subscript value '8' is outside the bounds of array type 'const char [8]'"
        // static_assert(cnt_space("a b c d", 9) == 3);
    }
}

void ConstConstexprMutable::TestAll()
{

	//------------------------------------------------------------------------------------//

	// ConstexprFuncs::ConstexprSum();
	// ConstexprFuncs::Factorial_Test();
	// ConstexprFuncs::Check_IF_Constexpr();

	// Constexpr_Tests::Test_Array_Sum();

    // CompileTimeComputations::FibonacciSequence_ToVector();

	//-------------------------------------------------------------------------------------//


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

	// Constexpr_STL_Containers::Test_Vector();

	// Consteval::GetConstString();
	// Consteval::SimpleTests();
	// Consteval::Factorial_Test();

    // Constexpr_Consteval::Call_Consteval_RunTime("");
    // Constexpr_Consteval::Constexpr_Static_Vs_RunTime();

	// As_Const::String_As_Const();

	// Tests::ForLoop();
	// Tests::Test_Constexpr_Integer();

    // Returning_ConstExpr_Array_SizeAsParameter::createArray();

    UndefinedBehaviour::TryInvokeUB();
};
