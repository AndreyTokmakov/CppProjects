/**============================================================================
Name        : FunctionObjects.cpp
Created on  : 24.07.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Function objects src
============================================================================**/

#include <iostream>
#include <string>
#include <functional>
#include <chrono>
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <array>
#include <functional>   // std::modulus, std::bind2nd
#include <algorithm> 
#include <string_view> 
#include <array>
#include <type_traits>
#include <memory>
#include <cassert>

#include "../Helpers/Helpers.h"
#include "FunctionObjects.hpp"

namespace FunctionObjects {

	class SumFunctor {
	public:
		int operator()(int a, int b) {
			return a + b;
		}
	};


	void Vector_Of_Functions()
	{
		const std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7 };
		std::vector<std::function<int(int)>> handlers;

		handlers.emplace_back([](int value) { return value * 2; });
		handlers.emplace_back([](int value) { return value * 4; });
		handlers.emplace_back([](int value) { return value * 10; });

		for (const auto& func : handlers) {
			for (int v : numbers) {
				std::cout << func(v) << " ";
			}
			std::cout << std::endl;
		}
	}


	void FunctorTest() {
		SumFunctor sum_func;
		int result = sum_func(1, 3);
		std::cout << result << std::endl;
	}


	bool isvowel(char c) {
		return std::string("aeoiuAEIOU").find(c) != std::string::npos;
	}


	////////////////////////////////////////////////////////////////////////////////

	namespace Less {
		template <typename A, typename B, typename U = std::less<>>
		bool compare(A a, B b, U comparer = U())
		{
			return comparer(a, b);
		}

		void Test() {
			std::cout << std::boolalpha;
			std::cout << compare(5, 20) << std::endl;
			std::cout << compare(100, 10) << std::endl;
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	namespace Greater {
		template <typename A, typename B, typename U = std::greater<>>
		bool compare(A a, B b, U comparer = U())
		{
			return comparer(a, b);
		}

		void Test() {
			std::cout << std::boolalpha;
			std::cout << compare(5, 20) << std::endl;
			std::cout << compare(100, 10) << std::endl;
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	namespace Multiplies {
		template <typename T, typename U = std::multiplies<>>
		T mult(T a, T b, U func = U())
		{
			return func(a, b);
		}

		void Test() {
			std::cout << mult(5, 20) << std::endl;
			std::cout << mult(100, 10) << std::endl;
		}

		void Test2() {
			int factorials[9], numbers[9] = { 1,2,3,4,5,6,7,8,9 };
			std::partial_sum(numbers, numbers + 9, factorials, std::multiplies<int>());
			for (int i = 0; i < 9; i++)
				std::cout << numbers[i] << "! is " << factorials[i] << std::endl;
		}
	}

	////////////////////////////////////////////////////////////////////////////////

	namespace Negate {
		template <typename T, typename U = std::negate<>>
		T __negate(T a, U func = U()) {
			return func(a);
		}

		void Test() {
			std::cout << __negate(-2) << std::endl;
			std::cout << __negate(92) << std::endl;
		}

		void Test2() {
			int numbers[] = { 1, -2, 3 };
			std::transform(numbers, numbers + 3, numbers, std::negate<int>());
			for (int i : numbers)
				std::cout << i << ' ';
			std::cout << std::endl;
		}
	}

	////////////////////////////////////////////////////////////////////////////////
	namespace Modulus {

		template <typename T>
		T modulus(T a, T b) {
			return a % b;
		}

		//template <typename T = int>
		int modulus2(int x, int y) {
			return x % y;
		}

		
		void Test() {
			std::cout << Modulus::modulus(5, 2) << std::endl;
		}

		
		void Test2() {
	
			{
				int numbers[] = { 1,2,3,4,5 };
				int remainders[5];
				std::transform(numbers, numbers + 5, remainders, std::bind(modulus2, std::placeholders::_1, 2));
				for (int i = 0; i < 5; i++)
					std::cout << numbers[i] << " is " << (remainders[i] == 0 ? "even" : "odd") << '\n';
			}

			std::cout << "\nTest2:\n" << std::endl;
			{
				int numbers[] = { 1,2,3,4,5 };
				// int remainders[5];
				const auto is_odd = [](int v) { if (0 == modulus(v, 2)) std::cout << "even" << std::endl; else std::cout << "odd" << std::endl; };
				std::for_each(numbers, numbers + 5, is_odd);
			}

		}
	}
}

namespace FunctionObjects::BindTests {

	class Utilities {
	public:

		void Info() {
			std::cout << "Utilities::Info()" << std::endl;
		}

		void printer(const std::string& text) {
			std::cout << __FUNCTION__ << ". Text: " << text << std::endl;
		}
		void printer_two_params(std::string_view prefix, std::string_view text) {
			std::cout << prefix << " : " << text << std::endl;
		}
	};


	double my_divide(double x, double y) {
		return x / y;
	}

	void printer(const std::string& text) {
		std::cout << "Input text 2: " << text << std::endl;
	}

	class Object {
	public:
		void info(const std::string& text) {
			std::cout << "Input text: " << text << std::endl;
		}
	};

	struct Foo {
		Foo(int num) : num_(num) {}
		void print_add(int i) const {
			std::cout << num_ + i << '\n';
		}
		int num_;
	};

	void printe_ext(const std::string& text)
	{
		std::cout << __FUNCTION__ << ". Text: " << text << std::endl;
	}


	//////////////////////////////// TESTS ///////////////////////////////////////////////

	void FuncPtr_Tests() {

		void(*ptrFunc1)(const std::string& str);
		ptrFunc1 = &printe_ext;
		ptrFunc1("Old style test 1");

		auto prtFunc2 = &printe_ext;
		ptrFunc1("Old style test 2");

		using prtFunc3 = void (Utilities::*)(const std::string& str);
		prtFunc3 ptr3 = &Utilities::printer;

		Utilities utils;
		(utils.*ptr3)("Old style test 3");

		const auto auto_printer = [](const std::string& text1, const std::string& text2)-> void {
			std::cout << text1 << " . " << text2 << std::endl;
		};

		std::function<void()> func1 = std::bind(auto_printer, "std::function", "Test1");
		func1();

		std::function<void(const std::string&)> func2 = std::bind(auto_printer, "std::function", std::placeholders::_1);
		func2("Test2");


		std::function<void(const std::string&)> func3 = std::bind(&Utilities::printer, new Utilities(), std::placeholders::_1);
		func3("new Utilities()->printer() called.");


		auto print = std::mem_fn(&Utilities::printer);
		print(utils, "The TEST 123");
	}

	void Bind_Test_1() {

		auto fn_five = std::bind(my_divide, 10, 2);
		std::cout << fn_five() << std::endl;

		auto fn_half = std::bind(my_divide, std::placeholders::_1, 5);
		std::cout << fn_half(10) << std::endl;
	}

	void Bind_Test_2() {
		std::unique_ptr<Object> object = std::unique_ptr<Object>(new Object());

		auto func = std::bind(&Object::info, object.get(), std::placeholders::_1);
		func("World");

		auto func1 = std::bind(&Object::info, object.get(), "Predefined Text");
		func1();

		auto func2 = std::bind(&printer, "Text for printer");
		func2();
	}

	void Bind_Test_3() {
		const Foo foo(314159);

		std::function<void(int)> f_add_display2 = std::bind(&Foo::print_add, &foo, std::placeholders::_1);
		f_add_display2(2);
	}

	void Bind_Test_4() {
		Utilities utilities;
		{
			auto printer = std::bind(&Utilities::printer_two_params, &utilities, "Binded_Prefix", std::placeholders::_1);
			printer("Tes3");
		}
		std::cout << "\n-------------------------  Test2: -----------------------------\n" << std::endl;
		{
			std::vector<std::string> strings = { "Value1", "Value2", "Value3", "Value4", "Value5" };
			auto printer = std::bind(&Utilities::printer_two_params, &utilities, "Binded_Prefix", std::placeholders::_1);
			std::for_each(strings.begin(), strings.end(), printer);
		}
	}

	void Bind_Class_Method() {
		Utilities utils;

		auto info_func = std::bind(&Utilities::Info, &utils);
		info_func();

		auto func1 = std::bind(&Utilities::printer_two_params, &utils, std::placeholders::_1, std::placeholders::_2);
		func1("PARAM_1", "PARAM_2");

		auto func2 = std::bind(&Utilities::printer_two_params, &utils, std::placeholders::_2, std::placeholders::_1);
		func2("PARAM_1", "PARAM_2");

	}

	void Bind_In_Bind() {
		constexpr auto printer = [](const std::string& str1, const std::string& str2)-> void {
			std::cout << "printer() called with params: {" << str1 << ", " << str2 << "}" << std::endl;
			std::cout << str1 << " " << str2 << std::endl;
		};
		constexpr auto getter = [](const std::string& str)-> std::string {
			std::cout << "getter() called with params: {" << str << "}" << std::endl;
			return "[" + str + "]";
		};

		auto callable = std::bind(printer, std::bind(getter, "Test_Prefix"), std::placeholders::_1);
		callable("Input value");
	}

	int minus(int a, int b) {
		return a - b;
	}

	void Bind_Front_Test() {
		auto fifty_minus = std::bind_front(minus, 50);
		std::cout << fifty_minus(3) << "\n";
	}

	/***********************************************************************************************************/

	class Base {
	public:
		virtual void print(std::string_view text) const noexcept = 0;
	};

	class Worker1 : public Base {
		virtual void print(std::string_view text) const noexcept override {
			std::cout << "Worker1::print(" << text << ") called." << std::endl;
		}
	};

	class Worker2 : public Base {
		virtual void print(std::string_view text) const noexcept override {
			std::cout << "Worker2::print(" << text << ") called." << std::endl;
		}
	};

	void Bind_CallDifferentClassMethods() {
		std::cout << " ------------------------------------------ Using Bind ----------------------------------------" << std::endl;
		{
			auto callable = std::bind(&Base::print, std::placeholders::_1, "Some_Test_Value");
			std::vector<std::shared_ptr<Base>> objects = { std::make_shared<Worker1>(), std::make_shared<Worker2>() };
			std::for_each(objects.begin(), objects.end(), callable);
		}
		std::cout << " ------------------------------------------ Using Lambda ----------------------------------------" << std::endl;
		{
			auto callable = [](auto obj) { obj->print("Some_Test_Value"); };
			std::vector<std::shared_ptr<Base>> objects = { std::make_shared<Worker1>(), std::make_shared<Worker2>() };
			std::for_each(objects.begin(), objects.end(), callable);
		}
	}
}


namespace FunctionObjects::Auto {

	template<typename T>
	auto get_as_int(T val)-> int {
		return val;
	}

	template<typename T>
	auto get(T val) // -> decltype(typeid(T)) {
	{
		std::cout << typeid(T).name() << std::endl;
		return val;
	}

	void Return_Type_Hint() {

		std::cout << get_as_int(12) << std::endl;
		std::cout << get_as_int(1.232) << std::endl;

		std::cout << std::endl;

		std::cout << get(12) << std::endl;
		std::cout << get(1.232) << std::endl;
	}

}

namespace FunctionObjects::CallBack_Tracker {

	template<typename CallBack>
	class CountCalls {
	private:
		/** CallBack to call. **/
		CallBack callback;

		/** counter for calls. **/
		unsigned long calls = 0; 

	public:
		CountCalls(CallBack cb) : callback(cb) {
		}

		template<typename... Args>
		auto operator() (Args&&... args) {
			++calls;
			return callback(std::forward<Args>(args)...);
		}

		long count() const {
			return calls;
		}
	};

	///////////////////////////////////////////////////////

	void Test() {

		std::cout << "\n------------------------------ Sort TEST ------------------------------\n" << std::endl;

		{
			CountCalls tracker([](auto x, auto y) {return x > y; });

			std::array<int, 10> numbers{ 9,8,7,6,5,4,3,2,1,0 };
			std::sort(numbers.begin(), numbers.end(), std::ref(tracker));

			std::cout << "Sorting array took " << tracker.count() << " calls\n";
		}

		std::cout << "\n------------------------------ For_Each TEST ------------------------------\n" << std::endl;

		{
			CountCalls tracker([](auto x) { return true; });

			constexpr unsigned short count = 10;
			std::array<int, count> numbers{ 9,8,7,6,5,4,3,2,1,0 };

			std::for_each(numbers.begin(), numbers.end(), std::ref(tracker));

			std::cout << "ForEach took " << tracker.count() << " calls\n";
			assert(count == tracker.count());
		}
	}
}

namespace FunctionObjects::HigherOrderFunctions
{

	template <typename ... Types>
	void foo(auto callback, Types&& ... params) {
		callback(std::forward<Types>(params)...);
	}

	void bar(int i, const std::string& s) {
		std::cout << "{" << i << ", " << s << "}\n";
	}

	void Function_Accepting_Function()
    {
		foo(bar, 1, "Str");
	}

	//---------------------------------------------------------------//

	auto greater_than(int threshold) {
		return [threshold](int x) -> bool {
			return x > threshold;
		};
	}

	template<size_t v> 
	inline bool greater_than_2(int x) {
		return x > v;
	}


	void Algoritms_Test1() {
		std::array<int, 5> numbers{ 1,2,3,4,5 };

		{
			auto count = std::count_if(numbers.begin(), numbers.end(), greater_than(3)); // HERE IS EXAMPLE
			std::cout << count << std::endl;
		}
		{

			auto count = std::count_if(numbers.begin(), numbers.end(), greater_than_2<3>);
			std::cout << count << std::endl;
		}
	}
}

namespace FunctionObjects::Function {

	void Vector_Of_Functions()
	{
		const std::vector<int> numbers = { 1, 2, 3, 4, 5, 6, 7 };
		const std::vector<std::function<int(int)>> callbacks {
			[](int value) { return value * 2; },
			[](int value) { return value * 4; },
			[](int value) { return value * 10; }
		};

		std::for_each(callbacks.begin(), callbacks.end(), [&](auto&& func) {
			for (int v : numbers) 
				std::cout << func(v) << " ";
			std::cout << "\n";
		});
	}

	void Swap() {
		auto l1 = [](std::string&& text) { return text.append(" World");};
		auto l2 = [](std::string&& text) { return text.append(" New World"); };

		std::function<std::string(std::string&& text)> func1{ l1 };
		std::function<std::string(std::string&& text)> func2{ l2 };

		std::cout << func1("Hello") << std::endl;

		func1.swap(func2);

		std::cout << func1("Hello") << std::endl;
	}

	int f(int a) { return -a; }
	void g(double) {}

	void TargetType()
	{
		std::function<int(int)> fn1 { f };
		std::function<int(int)> fn2{ [](int a) {
			return -a;
		}};

		std::cout << fn1.target_type().name() << '\n' << fn2.target_type().name() << '\n';

		// since C++17 deduction guides (CTAD) can avail
		std::cout << std::function{ g }.target_type().name() << '\n';
	}

	void Lambda_To_Function() {
		std::function<bool(void)> func = []() {
			std::cout << "Return true" << std::endl;
			return true; 
		};

		bool result = func();
		std::cout << std::boolalpha << std::endl;
	}


    constexpr size_t TESTS_SIZE {100'000'000};

    auto test_direct_lambda()
    {
        auto lbd = [](int v) {
            return v * 3;
        };

        using L = decltype(lbd);
        auto fs = std::vector<L>{};
        fs.resize(TESTS_SIZE, lbd);
        auto res = int{0};

        for (const auto& f: fs) {
            res = f(res);
        }

        return res;
    }


    auto test_std_function() {
        auto lbd = [](int v) {
            return v * 3;
        };

        using F = std::function<int(int)>;
        auto fs = std::vector<F>{};
        fs.resize(TESTS_SIZE, lbd);
        auto res = int{0};

        for (const auto& f: fs) {
            res = f(res);
        }
        return res;
    }

    void Function_VS_Lambda_Performance() {
        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            test_direct_lambda();

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds. LAMBDA\n";
        }

        {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            test_std_function();

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> time_span = duration_cast<std::chrono::duration<double>>(end - start);
            std::cout << "It took me " << time_span.count() << " seconds. FUNCTION\n";
        }

    }
}



namespace FunctionObjects::PassToFunction {


	void call(std::function<void(void)> func)
	{
		func();
	}


	void PassLambdaAsInput() 
	{
		call([] {std::cout << "Hey. Im called!\n"; });
	}

	void PassObject() 
	{
		struct MyStruct
		{
			void operator()() {
				std::cout << "Hey. Im called too!\n";
			}
		};

		call(MyStruct());
	}
}

namespace FunctionObjects::Callable_Interfaces
{

    template<typename Ret, typename... Types>
    using callback_func_type = Ret (*)(Types &&...);

    template<typename Ret, typename... Types>
    using callback_func_type_const = Ret (*)(const Types &&...);

    template <typename Ret, typename ObjType, typename... Types>
    using callback_method_type = Ret (ObjType::*)(const Types&&...) const;


    template<typename T, typename... Types>
    auto invokeFunction(T callback, Types &&... args)
    {
        return std::invoke(callback, std::forward<Types>(args)...);
    }

    template<typename T, typename ObjType, typename... Types>
    auto invokeMethod(T callback, ObjType& object, Types &&... args)
    {
        return std::invoke(callback, object, std::forward<Types>(args)...);
    }


    std::string sumStrings(const std::string& a, const std::string& b)
    {
        return a + b;
    }

    struct Utils {
        [[nodiscard]]
        std::string sumStrings(const std::string& a, const std::string& b) const {
            return std::string{"["}.append(a).append(b).append(1, ']');
        }
    };

    void invokeFunctionTest()
    {
        const auto result = invokeFunction(sumStrings, "Hello ", "world!");
        std::cout << result << std::endl;
    }

    void invokeMethodTest()
    {
        Utils utils;
        const auto result = invokeMethod(&Utils::sumStrings, utils, "Hello ", "world!");
        std::cout << result << std::endl;
    }
}

void FunctionObjects::TestAll()
{
    // MemberFunctionPointer::TestAll();
    // CallbackTests::TestAll();
    // MoveOnlyFunction::TestAll();
	// PassCallableToClass::TestAll();
	// MemberFunctionPointer::TestAll();
	Static_FunctionCall_Operator::TestAll();

	// Auto::Return_Type_Hint();


	// BindTests::FuncPtr_Tests();
	// BindTests::Bind_Test_1();
	// BindTests::Bind_Test_2();
	// BindTests::Bind_Test_3();
	// BindTests::Bind_Test_4();
	// BindTests::Bind_In_Bind();
	// BindTests::Bind_CallDifferentClassMethods();
	// BindTests::Bind_Class_Method();
	// BindTests::Bind_Front_Test();

	// Vector_Of_Functions();
	// FunctorTest();

	// Less::Test();
	// Greater::Test();

	// Multiplies::Test();
	// Multiplies::Test2();

	// Negate::Test();
	// Negate::Test2();

	// Modulus::Test();
	// Modulus::Test2();

	// CallBack_Tracker::Test();

	// HigherOrderFunctions::Function_Accepting_Function();
	// HigherOrderFunctions::Algoritms_Test1();


    // Callable_Interfaces::invokeFunctionTest();
    // Callable_Interfaces::invokeMethodTest();

	// Function::Vector_Of_Functions();
	// Function::Swap();
	// Function::TargetType();
	// Function::Lambda_To_Function();
	// Function::Function_VS_Lambda_Performance();

	// PassToFunction::PassLambdaAsInput();
	// PassToFunction::PassObject();
}
