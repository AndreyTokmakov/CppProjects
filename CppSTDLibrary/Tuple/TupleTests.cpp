//============================================================================
// Name        : TupleTests.cpp
// Created on  : 17.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : TupleTests testing 
//============================================================================

#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include "TupleTests.h"

namespace Tuple {

	using String = std::string;
	using CString = const String&;

	namespace utils {

		template<std::size_t Index, class TCallback, class ...TParams>
		struct _foreach_ {
			static void tupleForeach_(TCallback& callback,
				const std::tuple<TParams...>& tuple) {
				const std::size_t idx = sizeof...(TParams) - Index;
				callback.operator() < idx > (std::get<idx>(tuple));
				_foreach_<Index - 1, TCallback, TParams...>::tupleForeach_(callback, tuple);
			}
		};

		template<class TCallback, class ...TParams>
		struct _foreach_<0, TCallback, TParams...> {
			static void tupleForeach_(TCallback& /*callback*/,
				const std::tuple<TParams...>& /*tuple*/) {}
		};

		template<class TCallback, class ...TParams>
		void tupleForeach(TCallback& callback, const std::tuple<TParams...>& tuple) {
			_foreach_<sizeof...(TParams), TCallback, TParams...>::tupleForeach_(callback, tuple);
		}

		template<class Tuple, std::size_t N>
		struct TuplePrinter {
			static void print(const Tuple& t) {
				TuplePrinter<Tuple, N - 1>::print(t);
				std::cout << ", " << std::get<N - 1>(t);
			}
		};

		template<class Tuple>
		struct TuplePrinter<Tuple, 1> {
			static void print(const Tuple& t) {
				std::cout << std::get<0>(t);
			}
		};

		template<class... Args>
		void print(const std::tuple<Args...>& t) {
			std::cout << "(";
			TuplePrinter<decltype(t), sizeof...(Args)>::print(t);
			std::cout << ")\n";
		}

		struct ForeachCallback {
			template<std::size_t Index, class T>
			void operator()(T&& element) {
				std::cout << "( " << Index << " : " << element << " ) ";
			}
		};
	}

	std::tuple<double, char, std::string> get_student(int id)
    {
		if (id == 0) return std::make_tuple(3.8, 'A', "Lisa Simpson");
		if (id == 1) return std::make_tuple(2.9, 'C', "Milhouse Van Houten");
		if (id == 2) return std::make_tuple(1.7, 'D', "Ralph Wiggum");
		throw std::invalid_argument("id");
	}

	std::tuple<int, int> foo_tuple()
    {
		return { 1, -1 };
	}

	void CreateTupleTest()
    {
		auto T = foo_tuple();
		std::cout << "T[0] = " << std::get<0>(T) << std::endl;
		std::cout << "T[1] = " << std::get<1>(T) << std::endl;
	}

    void GetSize()
    {
        {
            const std::tuple tup = std::tuple{5, 42};
            const size_t size = std::tuple_size_v<decltype(tup)>;
            std::cout << "size: " << size << std::endl;
        }
        {
            const std::tuple tup = std::make_tuple(3.8, 'A', "Lisa Simpson");
            const size_t size = std::tuple_size_v<decltype(tup)>;
            std::cout << "size: " << size << std::endl;
        }
    }

	void TupleTest2() {
		auto[a, b, c] = std::tuple(32, "hello", 13.9);

		std::cout << "1 element (a) = " << a << std::endl;
		std::cout << "2 element (b) = " << b << std::endl;
		std::cout << "3 element (c) = " << c << std::endl;
	}

	void ChangeTuppleValue() {
		{
			std::tuple<int> t1(1);
			std::cout << std::get<0>(t1) << std::endl;

			std::get<0>(t1) = 2;
			std::cout << std::get<0>(t1) << std::endl;
		}

		{
			std::tuple<String, String> tup("Val1", "Val2");
			std::cout << std::get<0>(tup) << ", " << std::get<1>(tup) << std::endl;

			std::get<0>(tup) = "Val_new_1";
			std::cout << std::get<0>(tup) << ", " << std::get<1>(tup) << std::endl;

			std::get<1>(tup) = "Val_new_2";
			std::cout << std::get<0>(tup) << ", " << std::get<1>(tup) << std::endl;
		}
	}

	void ForeachTupple() {
		auto myTyple = std::make_tuple(42, 3.14, "boo");
		//utils::tupleForeach(utils::ForeachCallback(), myTyple);
	}

	void CreateAndGet() {
		auto student0 = get_student(0);
		std::cout << "ID: 0,  GPA: " << std::get<0>(student0) << ", grade: " << std::get<1>(student0) << ", name: " << std::get<2>(student0) << std::endl;

		double gpa1;
		char grade1;
		std::string name1;
		std::tie(gpa1, grade1, name1) = get_student(1);
		std::cout << "ID: 1, GPA: " << gpa1 << ", grade: " << grade1 << ", name: " << name1 << std::endl;

		// C++17 structured binding:
		auto[gpa2, grade2, name2] = get_student(2);
		std::cout << "ID: 2, GPA: " << gpa2 << ", grade: " << grade2 << ", name: " << name2 << std::endl;
	}

	void TupleCat_Test() {
		std::tuple<int, std::string, float> t1(10, "Test", 3.14f);
		int n = 7;

		auto t2 = std::tuple_cat(t1, std::make_pair("Foo", "bar"), t1, std::tie(n));

		utils::print(t2);

		n = 10;

		utils::print(t2);
	}

	void TupleTypeCastError() {
		std::tuple<int, double, std::string> PI(3, 3.14, "Pi = 3.14");

		int piInt = std::get<0>(PI);
		double piDouble = std::get<1>(PI);
		std::string piStr = std::get<2>(PI);

		std::cout << "Pi (Int) = " << piInt << ", Pi (Double) = " << piDouble << ", Pi (String) = '" << piStr << "' " << std::endl;

		// 
		//auto piUnknown = std::get<3>(PI); // Compile error

	}

	void TupleType_OutOfRangeError() {

		std::tuple<int, double, std::string> PI(3, 3.14, "Pi = 3.14");

		auto piUnknown = std::get<2>(PI); // Compile OK

		//int val = std::get<2>(PI); // Compile error
	}
}

namespace Tuple::Make_Tuples
{
	class Foo
	{
	private:
		int first;
		float second;
		int third;

	public:
		Foo(int first, float second, int third): first(first), second(second), third(third) {
			std::cout << "Foo(" << first << ", " << second << ", " << third << ")" << std::endl;
		}

		Foo(const Foo &foo) {
			this->first = foo.first;
			this->second = foo.second;
			this->third = foo.third;
			std::cout << "Copy constructor Foo(" << first << ", " << second << ", " << third << ")" << std::endl;
		}

		Foo& operator=(const Foo& right) {
			if (this != &right) {
				this->first = right.first;
				this->second = right.second;
				this->third = right.third;
			}
			std::cout << "Copy assignment Foo(" << first << ", " << second << ", " << third << ")" << std::endl;
			return *this;
		}

		Foo(Foo&& foo) noexcept {
			this->first = std::exchange(foo.first, 0);
			this->second = std::exchange(foo.second, 0.0f);
			this->third = std::exchange(foo.third, 0);
			std::cout << "Move constructor Foo(" << first << ", " << second << ", " << third << ")" << std::endl;
		}

		Foo& operator=(Foo&& foo) noexcept {
			if (this != &foo) {
				this->first = std::exchange(foo.first, 0);
				this->second = std::exchange(foo.second, 0.0f);
				this->third = std::exchange(foo.third, 0);
			}
			std::cout << "Mpve assignment Foo(" << first << ", " << second << ", " << third << ")" << std::endl;
			return *this;
		}

		~Foo() {
			std::cout << "~Foo(" << first << ", " << second << ", " << third << ")" << std::endl;
		}

		friend std::ostream& operator<<(std::ostream& stream, Foo& foo) {
			stream << "Foo(" << foo.first << ", " << foo.second << ", " << foo.third << ")";
			return stream;
		}

		void info() const noexcept {
			std::cout << "INFO : Foo(" << first << ", " << second << ", " << third << ")" << std::endl;
		}
	};

	class Object {
	private:
		std::string str1;
		std::string str2;

	public:
		Object(const std::string& s1, const std::string& s2) : str1(s1), str2(s2) {
		}

		std::string toString() const noexcept {
			return "{" + str1 + "," + str2 + "}";
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////

	void Test()
	{
		std::tuple<int, float, int> tuple = std::make_tuple(42, 3.14f, false);
		Foo obj = std::make_from_tuple<Foo>(std::move(tuple));
		obj.info();
	
	}

	void Test2()
	{
		auto tuple = std::make_tuple("val1", "val2");
		Object obj = std::make_from_tuple<Object>(std::move(tuple));
		std::cout << obj.toString() << std::endl;
	}
}


namespace Tuple::PrintValues
{
	template<auto ...P>
	struct Printer {
		inline static std::tuple data = std::tuple(P...);

		template<int idx>
		static auto get() {
			return std::get<idx>(data);
		}
	};

	void Funny_Tuple_Test ()
	{
		static char str1[] = "answer1";
		static char str2[] = "answer2";
		static char str3[] = {'A', 'B', 0};

		auto x = Printer<str1, str2, str3>();

		std::cout << x.get<0>() << std::endl;
		std::cout << x.get<1>() << std::endl;
		std::cout << x.get<2>() << std::endl;
	}
}

namespace Tuple::IterateValues
{
	template <size_t Index, typename Tuple, typename Functor>
	constexpr void tuple_at(const Tuple& tpl, const Functor& func) {
		const auto& v = std::get<Index>(tpl);
		func(v);
	}

	template<typename Tuple, typename Functor, size_t Index = 0>
	auto tuple_for_each(const Tuple &tpl, const Functor &f) -> void {
		constexpr auto tuple_size = std::tuple_size_v<Tuple>;
		if constexpr(Index < tuple_size) {
			tuple_at<Index>(tpl, f);
			tuple_for_each<Tuple, Functor, Index + 1>(tpl, f);
		}
	}

	void IterateTest()
	{
		auto tpl = std::make_tuple(1, true, std::string{"Jedi"});
		tuple_for_each(tpl, [](const auto& v) {
			std::cout << v << " ";
		});
	}
}

namespace Tuple::IterateValues2
{
	template<typename Tuple, typename Functor, size_t Index = 0>
	constexpr void tuple_for_each(const Tuple &tpl, const Functor &func) {
		constexpr size_t tuple_size = std::tuple_size_v<Tuple>;
		if constexpr(Index < tuple_size) {
			//func(std::get<Index>(tpl));
			func(std::forward<decltype(std::get<Index>(tpl))>(std::get<Index>(tpl)));
			tuple_for_each<Tuple, Functor, Index + 1>(tpl, func);
		}
	}

	void IterateTest()
	{
		std::tuple tpl = std::make_tuple(1, true, std::string{"Jedi"});
		tuple_for_each(tpl, [](const auto& v) {
			std::cout << v << " ";
		});
	}
}


namespace Tuple::Apply
{
    void Sum_Tuple()
    {
        std::tuple<int, int, int>  tup(1, 2, 3);
        auto sum = [](auto a, auto b, auto c) { return a + b + c; };
        std::cout << std::apply(sum, tup) << std::endl;
    }


    void PrintTuple()
    {
        std::apply([](const auto&... args){
            (std::cout << ... << std::forward<decltype(args)>(args)) << std::endl;
        }, std::make_tuple<int, char, std::string>(1, '=', "One"));
    }
}

namespace Tuple::Apply_ForEach
{
	template <typename TupleT, typename Fn>
	void for_each(TupleT&& tp, Fn&& fn)
	{
		std::apply([&fn]<typename ...T>(T&& ...args){
				(fn(std::forward<T>(args)), ...);
		}, std::forward<TupleT>(tp));
	}

	void test()
	{
		std::tuple<int, std::string, std::string> t = std::make_tuple(1, "Two", "III");
		for_each(t, [](auto x){
			std::cout << x << " ";
		});

		// 1 Two III
	}
}


namespace Tuple::Lambda_Tuple
{
    auto printTuple = [](const auto& tuple) constexpr
    {
        auto impl = []<size_t idx>(this const auto& self, const auto& t) constexpr {
            if constexpr (idx < std::tuple_size_v<std::decay_t<decltype(t)>>) {
                std::cout << std::get<idx>(t) << " ";
                self.template operator()<idx+1>(t); // Рекурсивный вызов
            }
        };
        impl.template operator()<0>(tuple);
    };


    void PrintTuple()
    {
        std::tuple<int, double, std::string> tp{1, 2.0, "qwe"};
        printTuple(tp);
    }

}



namespace Tuple::Reference_Wrapper
{
    void Create_Tuple_with_Ref()
    {
        int value = 10;
        std::string name = "SomeName";

        std::tuple<int&, std::string> tup = std::make_tuple(std::ref(value), name);

        std::cout << "value: " << value << ", name: " << name << std::endl;
        std::get<0>(tup) = 123;
        std::cout << "value: " << value << ", name: " << name << std::endl;
    }

    void Create_Tuple_with_ConstRef()
    {
        int value = 10;
        std::string name = "SomeName";

        std::tuple<int&, const std::string&> tup = std::make_tuple(std::ref(value), std::cref(name));

        std::cout << "value: " << value << ", name: " << name << std::endl;
        std::get<0>(tup) = 123;
        // std::get<1>(tup) = "12323";   // INFO: Will not compile
        std::cout << "value: " << value << ", name: " << name << std::endl;
    }
}


namespace PrintTupleTests
{
    template <size_t N = 0, typename... Ts>
    constexpr void print(std::tuple<Ts...> tup)
    {
        if constexpr (N < sizeof...(Ts)) {
            std::cout << get<N>(tup) << ' ';
            print<N+1>(tup);
        }
    }

    void PrintTestTuple()
    {
        std::tuple<int, char, std::string> tup = std::make_tuple<int, char, std::string>(1, 'c', "qweertt");
        print(tup);
    }
}



void Tuple::TestAll()
{
	// CreateTupleTest();
	// GetSize();

	// TupleTest2();

	// ChangeTuppleValue();

	// CreateAndGet();

	// TupleCat_Test();

	// TupleTypeCastError();

	// ForeachTupple();

	// Make_Tuples::Test();
	// Make_Tuples::Test2();

	// PrintValues::Funny_Tuple_Test();

	// IterateValues::IterateTest();
	// IterateValues2::IterateTest();

    // Apply::Sum_Tuple();
    // Apply::PrintTuple();
    Apply_ForEach::test();
    // Lambda_Tuple::PrintTuple();

    // Reference_Wrapper::Create_Tuple_with_Ref();
    // Reference_Wrapper::Create_Tuple_with_ConstRef();

    // PrintTupleTests::PrintTestTuple();
};