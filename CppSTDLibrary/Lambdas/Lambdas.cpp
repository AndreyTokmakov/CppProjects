//============================================================================
// Name        : Lambdas.h
// Created on  : 
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Lambdas src
//============================================================================

#include <iostream>
#include <string>
#include <functional>
#include <string>
// #include <concepts>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <functional>   // std::modulus, std::bind2nd
#include <algorithm> 
#include <array>
#include <string_view> 

#include "Lambdas.h"
#include <cassert>
#include <variant>
#include <memory>

#include <type_traits>
#include "../Integer/Integer.h"


using namespace std::string_literals;


namespace Lambdas::Capture
{

    struct my_struct
    {
        my_struct() = default;
        my_struct(const my_struct &) {
            std::cout << "Copy" << std::endl;
        } // Called during this->func();

        void func() {
            [*this]() { std::cout << "* * * call copy constructor * * * " << std::endl; } ();
            [this]()  { std::cout << "* * * doesn't call copy constructor * * *" << std::endl; } ();
        }
    };

    void Capture_This()
    {
        my_struct ms;
        ms.func();
    }


    void Capture_Modes()
    {
        std::cout << "\n------------------------------------------ Test1: ------------------------\n" << std::endl;
        {
            std::string value{ "Some_Text_value" };
            auto function = [&value]() {
                value = "Some_Text_value_NEW";
                std::cout << value << std::endl;
            };
            function();
            std::cout << value << std::endl;
        }
        std::cout << "\n------------------------------------------ Test2: ------------------------\n" << std::endl;
        {
            std::string value{ "Some_Text_value" };
            auto function = [=]() {
                // value = "Some_Text_value_NEW";
                std::cout << value << std::endl;
            };
            function();
            std::cout << value << std::endl;
        }
        std::cout << "\n------------------------------------------ Test3: ------------------------\n" << std::endl;
        {
            std::string value{ "Some_Text_value" };
            auto function = [value]() {
                // value = "Some_Text_value_NEW";
                std::cout << value << std::endl;
            };
            function();
            std::cout << value << std::endl;
        }
        std::cout << "\n------------------------------------------ Test3: ------------------------\n" << std::endl;
        {
            std::unique_ptr<Integer> integer = std::make_unique<Integer>(22);
            auto function = [integer = std::move(integer)]() {
                std::cout << integer->getValue() << std::endl;
            };
            function();
        }
    }

    void Capture_Variable_Copy()
    {
        std::vector<std::function<void()>> handlers;

        {
            int value = 123;
            handlers.emplace_back([&value]() { std::cout << "Value: " << value << std::endl; });
        }

        auto func = handlers.front();
        func();
    }


    template<typename... Args>
    void _print_copy(Args&&... params)
    {
        auto printLambda = [params ...]() {
            (std::cout << ... << params) << std::endl;
        };
        printLambda();
    }

    template<typename... Args>
    void _print_move(Args... args)
    {
        auto printLambda = [...params = std::move(args)]() {
            (std::cout << ... << params) << std::endl;
        };
        printLambda();
    }

    template<typename... Args>
    void _print_ref(Args... args)
    {
        auto printLambda = [&...params = args]() {
            (std::cout << ... << params) << std::endl;
        };
        printLambda();
    }

    template<typename... Args>
    void _print_forward(Args&&... args)
    {
        auto printLambda = [...params = std::forward<Args>(args)]() {
            (std::cout << ... << params) << std::endl;
        };
        printLambda();
    }

    template<typename... Args>
    void _print_ref_forward(Args&&... args)
    {
        auto printLambda = [&...params = std::forward<Args>(args)]() {
            (std::cout << ... << params) << std::endl;
        };
        printLambda();
    }

    template <typename... Args>
    auto f(Args&&... args){
        // BY VALUE:
        return [...args = std::forward<Args>(args)] {
            // ...
        };
    }

    void myPrint(std::string_view s) {
        std::cout << s << std::endl;
    }

    template<typename Callable, typename... Types>
    auto createCallable(Callable func, Types... args)
    {
        return [func, ...params = std::move(args)] () -> decltype(auto) {
            return func(params...);
        };
    }

    void Capturing_Parameter_Packs() {
        _print_copy(1, " + ", 2, " = ", 3);
        _print_move(1, " + ", 2, " = ", 3);
        _print_ref( 1, " + ", 2, " = ", 3);

        int a = 1, b = 2, c = 3;

        _print_forward( 1, " + ", 2, " = ", 3);
        _print_ref_forward( a, " + ", b, " = ", c);

        auto f = createCallable(myPrint, "Hello");
        f();
    }
}



namespace Lambdas {

	class SomeClass {
	private:
		int m_x = 0;

	public:
		void info() const {
			std::cout << "Info. X = " << m_x << std::endl;
		}
		void increment() {
			std::cout << "increment = " << m_x << std::endl;
			this->m_x++;
			std::cout << "increment = " << m_x << std::endl;
		}

		// C++14
		void Func_C14_Style() {
			// const copy of *this
			auto lambda1 = [self = *this]() mutable {
				self.info();
			};
			// non-copy of copy of *this
			auto lambda2 = [self = *this]() mutable {
				self.increment();
			};

			lambda1();
			lambda2();
			lambda1();
		}

		// C++17
		void Func_C17_Style() {
			// const copy of *this
			auto lambda1 = [*this](){ info(); };
			// non-const copy of *this
			auto lambda2 = [*this]() mutable { increment(); };
			lambda1();
			lambda2();
			lambda1();
		}


		void Func_Tests() {
			info();
			increment();
			info();
		}
	};

	void Pass_THIS_to_Lambda() {
		SomeClass T;

		T.Func_C14_Style();
		// T.Func_C17_Style();
		// T.Func_Tests();
	}

	//-------------------------------------------------------------------------------//

	void FindIF_Lambda_Test()
	{
		std::vector<int> v = { 1, 2 ,3, 4 ,5 };

		std::cout << "list items:" << std::endl;
		auto print = [](int val) { std::cout << val << std::endl; };
		std::for_each(std::begin(v), std::end(v), print);

		auto is_odd = [](int n) {return n % 2 == 1; };
		std::cout << "\nOdd items:" << std::endl;

		auto oddIter = v.begin();
		while (v.end() != (oddIter = std::find_if(oddIter, std::end(v), is_odd))) {
			std::cout << *oddIter << std::endl;
			oddIter++;
		}
	}

	void Test1()
	{
		std::vector<int> vect = { 1, 2, 3, 4, 5 };
		auto print = [](int val) { std::cout << val << std::endl; };
		std::for_each(vect.begin(), vect.end(), print);
	}




	//////////////////////

	struct {
		template<typename T, typename U>
		auto operator()(T x, U y) const { return x + y; }
	} sum{};

	void Lambda_Struct() {
		auto result = sum(1, 2);
		std::cout << result << std::endl;
	}

	/////////////////////////////////////////////////

	// cant use 'const std::string& text' herer
	const auto getFunction(const std::string text) {
		return [text](const std::string& value) { return text + ": " + value;  };
	}

	void Function_ReturnLambda() {
		auto handler = getFunction("Some text");
		std::cout << handler("Value") << std::endl;
	}

	//----------------------------------------------------------------------------------------//

	template<typename ...Args>
	void print(Args&&... args) {
		(std::cout << ... << std::forward<Args>(args)) << std::endl;
	}

	template<typename ...Args>
	auto Sum(Args ...args) {
		return (args + ...);
	}

	template<typename ...Args>
	auto Sum_RLal(Args&&... args) {
		return (std::forward<Args>(args) + ...);
	}

	void Variadic_Lambdas() {

		auto foo = [](auto... param) {
			return Sum(param...);
		};

		auto foo_foo = [](auto&&... param) {
			return Sum_RLal(std::forward<decltype(param)>(param)...);
		};

		auto result1 = foo(1, 2, 3, 4, 5);
		auto result2 = foo_foo(1, 2, 3, 4, 5);

		std::cout << result1 << std::endl;
		std::cout << result2 << std::endl;
	}

	//---------------------------------------------------------------------------------------//

	void Pass_UniquePtr_2Lambda() {
		std::cout << " ------------------Pass by REF test: ------------------" << std::endl;
		{
			auto integer = std::make_unique<Integer>(12);
			auto printer = [&integer]()-> void { integer->printInfo(); };

			printer();
			integer->printInfo();

			std::cout << "Done" << std::endl;
		}
		std::cout << "\n------------------ Pass by VALUE test: ------------------" << std::endl;
		{
			auto integer = std::make_unique<Integer>(12);
			auto printer = [var = std::move(integer)]{ var->printInfo(); };

			printer();
			// integer->printInfo(); // Crush HERE!

			std::cout << "Done" << std::endl;
		}
	}

	//-------------------------------------------------------------------------------------------//

	void Statics_In_Lambda() {
		{
			auto counter = [] {
				static int count = 0;
				return ++count;
			};

			auto c1 = counter;
			auto c2 = counter;

			std::cout << c1() << "  " << c1() << "  " << c1() << std::endl;
			std::cout << c2() << "  " << c2() << "  " << c2() << std::endl;
		}

		std::cout << " ================================== Test2 =============================\n";

		{
			auto counter = [](auto a) {
				static int count = 0;
				count += a;
				return count;
			};

			auto c1 = counter;
			auto c2 = counter;

			std::cout << c1(1) << "  " << c1(1) << "  " << c1(1) << std::endl;
			std::cout << c2(1.0) << "  " << c2(1.0) << "  " << c2(1.0) << std::endl;
		}
	}

	//-------------------------------------------------------------------------------------------//

	void Statics_In_Lambda_2() {
		auto lambda = [](auto x) {
			static int y = 0;   // static local
			return ++y + x;
		};

		std::cout << lambda(1) << " " << lambda(1.0) << " " << lambda(1) << std::endl;
	}

	//----------------------------------------------------------------------------------------//

	void Lambda_With_Params_Initialization() {
		auto func = [ptr = std::make_unique<Integer>(0)]()-> void {
			ptr->increment();
			ptr->printInfo();
		};

		func();
		func();
		func();
	}

	//----------------------------------------------------------------------------------------//

	void Determine_TypeOf_VectorParameter() {
		auto func = [](const auto& vect)-> void {
			std::cout << typeid(vect.back()).name() << std::endl;

			using T = decltype(vect.back());
			T a{ 1 };

			std::cout << typeid(a).name() << std::endl;
		};


		auto func2 = [](const auto& vect)-> void {
			using T = typename std::decay_t<decltype(vect)>::value_type;

			T a{ 1 };

			std::cout << typeid(a).name() << std::endl;
			std::cout << typeid(T).name() << std::endl;
		};
		;
		const std::vector<int> numbers;

		func(numbers);
		std::cout << "\n";
		func2(numbers);
	}


	void Determine_TypeOf_VectorParameter_2_Constexpr() {
		auto getType = [](const auto& vect)-> void {
			using T = typename std::decay_t<decltype(vect)>::value_type;

			std::cout << typeid(vect).name() << std::endl;

			T a{ 1 };
			std::cout << "Element type: " << typeid(T).name() << std::endl;
		};
		

		constexpr std::array<int, 5> numbers {1,2,3,4,5};
		getType(numbers);
	}

	//---------------------------------------------------------------------------------------//

	int x = 123;

	auto func1 = []() noexcept -> int { return x + 1; };
	auto func2 = [x = x]() noexcept -> int { return x + 1; };

	void Handle_Global_Varibles() {
		x = 10;
		std::cout << "func1 = " << func1() << "   func2 = " << func2() << std::endl;
	}

	void Handle_Global_Varibles2() {
		int x = 10;
		std::cout << "func1 = " << func1() << "   func2 = " << func2() << std::endl;
	}

	//------------------------------------------------------------------------------------------------------------------------//

	void Lambda_Collection() {
		std::vector<std::function<std::string(const std::string&)>> handlers;

		handlers.emplace_back(getFunction("Handler 1"));
		handlers.emplace_back(getFunction("Handler 2"));
		handlers.emplace_back(getFunction("Handler 3"));
		handlers.emplace_back(getFunction("Handler 4"));
		handlers.emplace_back(getFunction("Handler 5"));

		for (const auto& param : { "Value1","Value2" ,"Value2" }) {
			std::for_each(handlers.begin(), handlers.end(), [&param](const auto& func) {
				std::cout << func(param) << std::endl;
			});
			std::cout << std::endl;
		}
	}


	void Get_Lambda_Type()
	{
		auto l = [](int value)-> int { return value * 10; };
		using T = decltype(l);

		T x10;
		std::cout << x10(3) << std::endl;
	}

	//----------------------------------------------------------------------------------------------------------------------------//

	constexpr int not_lambda_Fib(const int n)
	{
		if (1 == n)
			return 0;
		else if (2 == n)
			return 1;
		else
			return not_lambda_Fib(n - 1) + not_lambda_Fib(n - 2);
	}


	void Recursive_Lambda() {

		std::function<int(int const)> lambda_Fib = [&lambda_Fib](int const n) {
			if (1 == n)
				return 0;
			else if (2 == n)
				return 1;
			else
				return lambda_Fib(n - 1) + lambda_Fib(n - 2);
		};

		for (int i = 1; i < 20; i++) {
			std::cout << i << " ==> " << not_lambda_Fib(i) << "  :  " << lambda_Fib(i) << std::endl;
		}
	}

    //---------------------------------------------------------------------------------------------------//

    int fact_std_func(int n) {
        std::function<int(int)> fact = [&](auto n) {
            if (n == 1) return 1;
            return n * fact(n-1);
        };
        return fact(n);
    }

    int fact_comb(int n) {
        auto fact = [](auto n, auto self) -> int {
            if (n == 1) return 1;
            return n * self(n-1, self);
        };
        return fact(n, fact);
    }

    void Recursive_Lambda2()
    {
        std::cout << fact_std_func(5) << std::endl;
        std::cout << fact_comb(5) << std::endl;
    }

	//---------------------------------------------------------------------------------------------------//

	void Lambda_Itialyzed_With_Another_Lambda() {
		auto l = [
			i = [] {
				struct S {
					int value = 5;
					S() { std::cout << "Constucted!!!\n"; }
					S(const S& obj) { std::cout << "Copied!!!\n"; }
					~S() { std::cout << "Destructed!!!\n"; }
				};
				return S{};
			}()
		]{
			return i;
		};

		auto x = l().value;

		std::cout << x << std::endl;
	}

	//----------------------------------------------------------------------------------------------------------------------------//

	void LambdasWithDestructors() {
		auto l = [i = [] {return 5; }]{
			return i;
		};

		auto x = l()();
		std::cout << x << std::endl;
	}

	//----------------------------------------------------------------------------------------------------------------------------//

	template<typename L1, typename L2>
	struct S : L1, L2 {
		S(L1 l1, L2 l2) : L1(std::move(l1)), L2(std::move(l2)) { }
		using L1::operator();
		using L2::operator();
	};


	void Inheriting_From_Lambdas()
    {
		auto f1 = []()-> std::string {
            return "Hello world!";
        };
		auto f2 = [](const std::string& text)-> void {
            std::cout << "Input text: " << text << std::endl;
        };
		auto compose = S(f1, f2);


		//compose("123");
		auto result = compose();

        std::cout << result << std::endl;
	}

    //----------------------------------------------------------------------------------------------------------------------------//


    template <class ... Ts>
    struct Overloader: Ts... {
        /*
        template <class ... Types>
        overload(Types&& ... params) : Ts{ std::forward<Types>(params) }... {
            // Impl
        }
        */

        using Ts::operator()...;
    };


    void Overload_Example()
    {
        auto F = Overloader {
            [](int a) { std::cout << "INT: " << a << std::endl; },
            [](float f) { std::cout << "Float: " << f << std::endl; },
            [](std::string str) { std::cout << "String: " << str << std::endl; }
        };


        // std::variant<int, float, std::string> var = 2;
        // std::visit(F, var);

        F(10);
        F(10.0f);
        F("10");
    }

    //----------------------------------------------------------------------------------------------------------------------------//

	template<typename... Args>
	void printer(Args&&... args) {
		std::cout << "Args count = " << sizeof ... (args) << std::endl;
		(std::cout << ... << std::forward<Args>(args)) << std::endl;
	}

	void Template_Lambda()
	{
		// generic lambda, operator() is a template with two parameters
		auto glambda_auto = [](auto a, auto&& b) {
			return a < b;
		};

		// generic lambda, operator() is a template with two parameters
		auto glambda_template = []<class T>(T a, auto && b) {
			return a < b;
		};

		auto printer_lambda = []<typename ...Ts>(Ts&& ...ts) {
			return printer(std::forward<Ts>(ts)...);
		};

		//------------------------------------- Tests ----------------------------------------/

		{
			bool b = glambda_auto(3, 3.14); // ok
			std::cout << "3 < 3.14 == " << std::boolalpha << b << std::endl;
		}

		{
			bool b = glambda_template(3, 3.14); // ok
			std::cout << "3 < 3.14 == " << std::boolalpha << b << std::endl;
		}

		{
			printer_lambda(1, "One", 2.44);
		}
	}

	void Template_Lambda_Default()
	{
		auto print1 = []<typename T> (T a) -> void {
			std::cout << typeid(T).name() << ". Value = " << a << std::endl;
		};

		print1(1);

		auto print2 = [x = 10]<typename T = int> (T a = 123) -> void {
			std::cout << typeid(T).name() << " " << typeid(x).name() << ". Value = " << a << std::endl;
		};

		print2();
		print2(321);
	}


	void Call_Lambda_InPlace() {
		[n = 10, text = "Some string"s] () {
			std::cout << text << ' ' << n << std::endl;
		} ();
	}

	void Invoke_Lambda() {

		auto call = [](auto callback, auto&& ... params) {
			return callback(std::forward<decltype(params)>(params)...);
		};

		auto print = []<typename ... Args>(Args&& ... params) {
			auto add_space = []<typename T>(const T & arg) {
				std::cout << ' ';
				return arg;
			};
			(std::cout << ... << add_space(std::forward<Args>(params))) << std::endl;
		};

		call(print, "ONE");
		call(print, "ONE", "Two");
		call(print, "ONE", "Two", 3);
	}

	//-----------------------------------------------------------------------------------------------


	template<typename F, typename... Args>
	auto call(F&& f, Args&&... args) {

		const auto typeName = typeid(decltype(f(std::forward<Args>(args)...))).name();

		//const auto typeName = typeid(decltype(F(std::forward<Args>(args)...))).name();
		std::cout << typeName << std::endl;
		return f(std::forward<Args>(args)...);
	}

	void Get_Lambda_Return_Type() {

		auto task1 = []() {
		};

		auto task2 = [](auto&& ... params) -> std::string {
			return "";
		};


		call(task1);
		call(task2, "");
	}
};

namespace Lambdas::Lambda_With_Concepts
{

    template <typename T>
    concept PlusOperator = requires(T type) {
        { type + type };
    };

    void PlusFunction()
    {
        using namespace std::string_literals;
        auto plus = [] <PlusOperator T, PlusOperator ...Args> (T first, Args ...args)
                requires ( std::is_same_v<T, Args> && ... ) {
            return (first + ... + args);
        };

        std::cout << plus(1) << std::endl; // Prints: 1
        std::cout << plus(1, 4, 5) << std::endl; // Prints: 10
        std::cout << plus(2.3, 4.5, 5.6) << std::endl; // Prints: 12.4
        std::cout << plus("Template"s, " "s, "Lambda"s, " "s, "Expression"s) << std::endl;
    }
}


namespace Lambdas::High_Order_Function {

	template <typename... Predicates>
	auto when_all(Predicates&& ... funcs) {
		return [=](auto const& x) {
			return (funcs(x) && ...);
		};
	}

	void PredicateComposition_WhenAll() {
		auto is_positive = [](int x)-> bool { return x > 0; };
		auto is_even = [](int x)-> bool { return 0 == x % 2; };

		auto positive_even = when_all(is_positive, is_even);

		std::cout << 4 << " == " << std::boolalpha << positive_even(4) << std::endl;
		std::cout << 3 << " == " << std::boolalpha << positive_even(3) << std::endl;
		std::cout << -1 << " == " << std::boolalpha << positive_even(-1) << std::endl;
		std::cout << 24 << " == " << std::boolalpha << positive_even(24) << std::endl;
	}

	//----------------------------------------------------------------//


    /*
	template <typename... Predicates>
	auto when_all_ex(Predicates ... funcs)
		requires (std::predicate<Predicates, int> && ...)
	{
		return [=](auto const& x) {
			return (funcs(x) && ...);
		};
	}
    */

	void PredicateComposition_WhenAll_Concepts() 
	{
		auto is_positive = [](int x)-> bool { return x > 0; };
		auto is_even = [](int x)-> bool { return 0 == x % 2; };
		auto some_lambda = [](int x)-> std::string { return {}; };

        /*
		auto positive_even = when_all_ex(is_positive, is_even);

		// TODO: Compile error
		// auto positive_even = when_all_ex(is_positive, is_even, some_lambda);
		
		for (const int v : {3, 4, -1}) {
			std::cout << v << ": " << std::boolalpha << positive_even(v) << std::endl;
		}
        */
	}

	//----------------------------------------------------------------//

	template<class Func, class ... _Types>
	auto create_task(Func&& func, _Types&& ... params) {
		return[_Func = std::forward<Func>(func), params ...]() mutable {
			return std::forward<Func>(_Func)(std::forward<_Types>(params) ...);
		};
	}

	template<class Func, class ... _Types>
	auto create_task_Ex(Func&& func, _Types&& ... params) {
		return[_Func = std::forward<Func>(func), ...params = std::forward<_Types>(params)]() mutable {
			return std::forward<Func>(_Func)(std::forward<_Types>(params) ...);
		};
	}

	void Task_Factory() {
		auto print = [](const auto& ... args) {((std::cout << args), ...); };

		auto printer_task = create_task(print, std::string("One"));


		auto printer_task_ex = create_task_Ex(print, std::string("One"));

		printer_task();
		printer_task_ex();
	}
}

namespace Lambdas::Tests {



	void _TEST_() {
		{
			[n = 10, text = std::string("Some string")] () {
				std::cout << text << ' ' << n << std::endl;
			} ();


			auto lambdaVectorIntegral = []<std::integral T>(const std::vector<T>&vec) {
				return vec.size();
			};
		}

		std::cout << "------------------------------------------- Modify captured by ref --------------------------------------\n";

		{
			int var = 10;
			std::cout << var << std::endl;
			[&var]() { var = 20; }();
			std::cout << var << std::endl;
		}

		std::cout << "------------------------------------------- Modify captured by val --------------------------------------\n";

		{
			int var = 10;
			std::cout << var << std::endl;
			[var]() mutable { var = 20; }();
			std::cout << var << std::endl;
		}
	}

	void TYPE_TEST_() {


		auto l1 = [](int value)-> int { return value * 10; };
		using T1 = decltype(l1);
		auto type1 = typeid(T1).name();


		auto l2 = [](int value)-> int { return value * 10; };
		using T2 = decltype(l2);
		auto type2 = typeid(T2).name();

		std::cout << type1 << '\n' << type2 << std::endl;

		assert(typeid(T1) == typeid(T2));
	}

	void VECTOR_OF_LAMBDAS() {

		const std::vector<std::function<void()>> callbacks {
			[] {std::cout << "Lambda_1() called\n"; },
			[] {std::cout << "Lambda_2() called\n"; },
			[] {std::cout << "Lambda_3() called\n"; }
		};

		for (const auto& func : callbacks) {
			func();
		}
	}


	void LAMBDA_CONCEPT() {

		auto sum1 = []<std::integral T> (T a, T b) -> T { return a + b; };
		// auto sum2 = [](T a, T b)  requires std::integral<T> { return a + b; };

		auto a = sum1(1, 2);
		std::cout << a << std::endl;
#if 0
		auto a = sum1(1.3, 2);
#endif


	}
}

namespace Lambdas::Constexpr_Constevel_Lambda {

	void ConstexprLambda() {

		auto x2 = [](int x) constexpr -> int {
			return x * 2;
		};

		auto _x2 = [](int x) -> int {
			return x * 2;
		};

		static_assert(2 == x2(1));
		static_assert(2 == _x2(1));

	}

	void Constevel_Lambda() {

		auto getValue = [] () consteval ->  int {
			return 101;
		};

		static_assert(101 == getValue());
		// static_assert(1021 == getValue());
	}
}

void Lambdas::TestAll()
{
	// Lambdas::FindIF_Lambda_Test();
	// Lambdas::Test1();
	// Lambdas::Pass_THIS_to_Lambda();

	// Capture::Capture_Modes();
    // Capture::Capture_Variable_Copy();
	Capture::Capturing_Parameter_Packs();  // By REF, MOVE and COPY
    // Capture::Capture_This();

	// Lambdas::Lambda_With_Params_Initialization();
	// Lambdas::Lambda_Struct();
	// Lambdas::Lambda_Collection();
	// Lambdas::Function_ReturnLambda();
	// Lambdas::Variadic_Lambdas();
	// Lambdas::Pass_UniquePtr_2Lambda();
	
	// Lambdas::Statics_In_Lambda();
	// Lambdas::Statics_In_Lambda_2();
	 
	// Lambdas::Handle_Global_Varibles();
	// Lambdas::Handle_Global_Varibles2();

	// Lambdas::Determine_TypeOf_VectorParameter();
	// Lambdas::Determine_TypeOf_VectorParameter_2_Constexpr();

	// Lambdas::Lambda_Itialyzed_With_Another_Lambda();
	// Lambdas::LambdasWithDestructors();
	// Lambdas::Inheriting_From_Lambdas();
	// Lambdas::Overload_Example();

	// Lambdas::Template_Lambda();
	// Lambdas::Template_Lambda_Default();
	// Lambdas::Call_Lambda_InPlace();

	// Lambdas::Invoke_Lambda();

	// Lambdas::Get_Lambda_Return_Type();

	// Lambdas::Recursive_Lambda();
	// Lambdas::Recursive_Lambda2();

	// Lambdas::Get_Lambda_Type();

	// Tests::_TEST_();
	// Tests::TYPE_TEST_();
	// Tests::VECTOR_OF_LAMBDAS();
	// Tests::LAMBDA_CONCEPT();

	// High_Order_Function::PredicateComposition_WhenAll();
	// High_Order_Function::PredicateComposition_WhenAll_Concepts();
	// High_Order_Function::Task_Factory();

	// Constexpr_Constevel_Lambda::ConstexprLambda();
	// Constexpr_Constevel_Lambda::Constevel_Lambda();

    // Lambda_With_Concepts::PlusFunction();
}
