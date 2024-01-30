//============================================================================
// Name        : TypeTraits.cpp
// Created on  : 01.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Test support classes and templates
//============================================================================

#include <iostream>
#include <string>
#include <array>
#include <functional>
#include <vector>
#include <utility>
#include <cstdint>
#include <typeindex>
#include "../TestSupport/TestSupport.h"
#include "../TypeTraits/TypeTraits.h"

using String = std::string;
using CString = const String&;

namespace TypeTraits::IsClass {
	struct A {
	};

	class B {
	};

	union C { 
		int i; float f; 
	};

	enum class D {
		x, y, z
	};

	void Test() {
		std::cout << std::boolalpha;
		std::cout << "is_class:" << std::endl;
		std::cout << "A: " << std::is_class<A>::value << std::endl;
		std::cout << "B: " << std::is_class<B>::value << std::endl;
		std::cout << "C: " << std::is_class<C>::value << std::endl;
		std::cout << "D: " << std::is_class<D>::value << std::endl;
	}
};

namespace TypeTraits::IsArray {
	void Test() {
		std::cout << std::boolalpha;
		std::cout << "is_array:" << std::endl;
		std::cout << "int: " << std::is_array<int>::value << std::endl;                         // false
		std::cout << "int[3]: " << std::is_array<int[3]>::value << std::endl;                   // true
		std::cout << "array<int,3>: " << std::is_array<std::array<int, 3>>::value << std::endl; // false
		std::cout << "string: " << std::is_array<std::string>::value << std::endl;              // false
		std::cout << "string[3]: " << std::is_array<std::string[3]>::value << std::endl;        // true
	}

	void Enable_If_Test() {

		int array[] = { 11, 12, 13, 14, 15 };
		using T = decltype(array);

		std::cout << "Type name: "<<  typeid(std::enable_if<std::is_array<T>::value, T>::type).name() << std::endl;

		std::cout << std::boolalpha << std::is_array<T>::value << std::endl;
		std::cout << std::boolalpha << std::is_array_v<T> << std::endl;

		std::cout << std::endl;

		std::cout << typeid(std::enable_if<std::is_array<T>::value, T>::type).name() << std::endl;
		std::cout << typeid(std::enable_if<std::is_array_v<T>, T>::type).name() << std::endl;

		std::cout << std::endl;

		std::cout << typeid(std::enable_if_t<std::is_array<T>::value, T>).name() << std::endl;
		std::cout << typeid(std::enable_if_t<std::is_array_v<T>, T>).name() << std::endl;
	}
};

namespace TypeTraits::Is_Base_Of {
	struct A { };
	struct B : A {};

	void Test() {
		std::cout << std::boolalpha;
		std::cout << "is_base_of:" << std::endl;
		std::cout << "int, int: " << std::is_base_of<int, int>::value << std::endl;
		std::cout << "A, A: " << std::is_base_of<A, A>::value << std::endl;
		std::cout << "A, B: " << std::is_base_of<A, B>::value << std::endl;
		std::cout << "A, const B: " << std::is_base_of<A, const B>::value << std::endl;
		std::cout << "A&, B&: " << std::is_base_of<A&, B&>::value << std::endl;
		std::cout << "B, A: " << std::is_base_of<B, A>::value << std::endl;
	}
}

namespace TypeTraits::Is_Function {
	// function
	int Func(int i) {
		return i;
	}
	// pointer to function
	int(*b)(int) = Func;

	// function-like class:
	struct C {
		int operator()(int i) {
			return i;
		}
	} c;

	void Test() {

		const auto lambda = [](void)->void { std::cout << "Im the lambda" << std::endl; };

		std::cout << std::boolalpha << " ****** is_function: *******\n" << std::endl;
		std::cout << "decltype(a): " << std::is_function<decltype(Func)>::value << std::endl;
		std::cout << "decltype(b): " << std::is_function<decltype(b)>::value << std::endl;
		std::cout << "decltype(c): " << std::is_function<decltype(c)>::value << std::endl;
		std::cout << "C: "           << std::is_function<C>::value << std::endl;
		std::cout << "int(int): "    << std::is_function<int(int)>::value << std::endl;
		std::cout << "int(*)(int): " << std::is_function<int(*)(int)>::value << std::endl;
		std::cout << "lambda: " << std::is_function<decltype(lambda)>::value << std::endl;
	}
}

namespace TypeTraits::Is_Lvalue_Reference {
	void Test() {
		std::cout << std::boolalpha << " ****** is_lvalue_reference: *******\n" << std::endl;
		std::cout << "int: " << std::is_lvalue_reference<int>::value << std::endl;
		std::cout << "int&: " << std::is_lvalue_reference<int&>::value << std::endl;
		std::cout << "int&&: " << std::is_lvalue_reference<int&&>::value << std::endl;
	}
}

namespace TypeTraits::Is_Member_Function_Pointer {
	struct A { void fn() {}; };

	void Test() {
		void(A::*pt)() = &A::fn;
		std::cout << std::boolalpha << " ****** is_member_function_pointer: *******\n" << std::endl;

		std::cout << "A*: " << std::is_member_function_pointer<A*>::value << std::endl;
		std::cout << "void(A::*)(): " << std::is_member_function_pointer<void(A::*)()>::value << std::endl;
		std::cout << "decltype(pt): " << std::is_member_function_pointer<decltype(pt)>::value << std::endl;
	}
}

namespace TypeTraits::Is_Abstract {
	struct A { };
	struct B {
		virtual void fn() = 0;  // pure virtual function
	};
	struct C : B { };
	struct D : C { virtual void fn() {} };
	void Test() {
		std::cout << std::boolalpha << " ****** is_abstract: *******\n" << std::endl;

		std::cout << "A: " << std::is_abstract<A>::value << std::endl;
		std::cout << "B: " << std::is_abstract<B>::value << std::endl;
		std::cout << "C: " << std::is_abstract<C>::value << std::endl;
		std::cout << "D: " << std::is_abstract<D>::value << std::endl;
	}
}

namespace TypeTraits::Is_Same{

	class A {
	public:
		virtual void info() const noexcept {
			std::cout << __FUNCTION__ << std::endl;
		}
	};

	class B : public A {
	public:
		virtual void info() const noexcept override {
			std::cout << __FUNCTION__ << std::endl;
		}
	};

	class C {
	public:
		virtual void info() const noexcept {
			std::cout << __FUNCTION__ << std::endl;
		}
	};

	void Test() {
		std::cout << std::boolalpha;

		std::cout << "is_same<int, int32_t> = " << std::is_same<int, int32_t>::value << std::endl;
		std::cout << "is_same<int, int64_t> = " << std::is_same<int, int64_t>::value << std::endl;
		std::cout << "is_same<float, int32_t> = " << std::is_same<float, int32_t>::value << std::endl;

		std::cout << "----------------" << std::endl;

		std::cout << "is_same<int, int> = " << std::is_same<int, int>::value << std::endl;
		std::cout << "is_same<int, unsigned int> = " << std::is_same<int, unsigned int>::value << std::endl;
		std::cout << "is_same<int, signed int> = " << std::is_same<int, signed int>::value << std::endl;

		std::cout << "----------------" << std::endl;

		std::cout << "is_same<char, char> = " << std::is_same<char, char>::value << std::endl;
		std::cout << "is_same<char, unsigned char> = " << std::is_same<char, unsigned char>::value << std::endl;
		std::cout << "is_same<char, signed char> = " << std::is_same<char, signed char>::value << std::endl;

		std::cout << "----------------" << std::endl;

		std::cout << "is_same<long, long> = " << std::is_same<long, long>() << std::endl;
		std::cout << "is_same<long, int> = " << std::is_same<char, int>() << std::endl;

		std::cout << "----------------" << std::endl;

		std::cout << "is_same<A, A> = " << std::is_same<A, A>() << std::endl;
		std::cout << "is_same<A, B> = " << std::is_same<A, B>() << std::endl;

		{
			std::cout << "\n---------------- STRING ----------------\n" << std::endl;
			std::string text = "";
			std::cout << "is_same<\"Text\", std::string> = " << std::is_same<decltype(text), std::string>() << std::endl;
		}

		{
			std::cout << "\n---------------- A & A  ----------------\n" << std::endl;
			A a;
			A a1;
			std::cout << "is_same<A var, A var> = " << std::is_same<decltype(a), decltype(a1)>() << std::endl;
		}

		{
			std::cout << "\n---------------- A & B  ----------------\n" << std::endl;
			A a;
			B b;
			std::cout << "is_same<A var, B var> = " << std::is_same<decltype(a), decltype(b)>() << std::endl;
		}
	}
}

namespace CustomTraits {

	template< typename T >
	struct is_void {
		static const bool value = false;
	};
	
	// Add to that a specialisation for void:
	template<>
	struct is_void< void > {
		static const bool value = true;
	};



	template <typename T>
	struct is_pointer {
		static const bool value = false;
	};

	// And a partial specialisation for all pointer types is added :
	template <typename T>
	struct is_pointer< T* > {
		static const bool value = true;
	};

	//////////////////////////////////////////////////////

	enum class Color { 
		red, 
		green,
		orange 
	};

	enum class Fruit { 
		apple, 
		orange, 
		pear 
	};

	template <typename T>
	class Traits {
	public:
		static const std::string name(size_t index) {
			return "";
		}
	};

	template <>
	class Traits<Color> {
	public:
		static const std::string name(size_t index) {
			switch (index) {
				case static_cast<int>(Color::red) :
					return "red";
				case static_cast<int>(Color::green) :
					return "green";
				case static_cast<int>(Color::orange) :
					return "orange";
				default:
					return "unknown";
			}
		}
	};

	template <>
	class Traits<Fruit> {
	public:
		static const std::string name(size_t index) {
			switch (index)
			{
				case static_cast<int>(Fruit::apple) :
					return "apple";
				case static_cast<int>(Fruit::orange) :
					return "orange";
				case static_cast<int>(Fruit::pear) :
					return "pear";
				default:
					return "unknown";
			}
		}
	};

	//////////////////////////////////////////////////////////////////////////////

	void Void_Test() {
		std::cout << "is Void: " << std::boolalpha << is_void<int>::value << std::endl;
		std::cout << "is Void: " << std::boolalpha << is_void<void>::value << std::endl;
	}

	void Is_Pointer() {
		std::cout << "is Pointer: " << std::boolalpha << is_pointer<int>::value << std::endl;
		std::cout << "is Pointer: " << std::boolalpha << is_pointer<int*>::value << std::endl;
	}

	void Color_Test() {
		std::cout << Traits<Color>::name(1) << std::endl;
	}
}

namespace TypeTraits::Decltype_and_Declval
{
	struct Default
    {
		[[nodiscard]]
        int foo() const {
			return 1; 
		} 
	};

	struct NonDefault
    {
		NonDefault() = delete;

		[[nodiscard]]
        int foo() const {
			return 1; 
		}
	};

	void Test()
    {
		std::vector<int> v;
		std::cout << "'v' variable type is: " << typeid(v).name() << std::endl;

		decltype(v)::value_type i = 0;  
		std::cout << "'i' variable type is: "<<  typeid(i).name() << std::endl;
	}

	void Test2() {

		int some_int;
		decltype(some_int) other_integer_variable = 5;
	}

    void BasicTests()
    {
        std::vector<int> data{1,2,3,4,5};
        const struct X {
            int x = 42;
        } x;

        // lvalue expression -> T&
        static_assert(std::is_same_v<decltype(data[2]), int&>);
        static_assert(std::is_same_v<decltype(std::as_const(data)[2]), const int&>);

        // prvalue expression -> T
        static_assert(std::is_same_v<decltype(1+2), int>);

        // xvalue expression -> T&&
        static_assert(std::is_same_v<decltype(std::move(data)), std::vector<int>&&>);


        // id or member -> the declared type
        static_assert(std::is_same_v<decltype(x), const X>);
        static_assert(std::is_same_v<decltype(x.x), int>);

        // we can treat ids/members as expressions
        static_assert(std::is_same_v<decltype((x)), const X&>);
        static_assert(std::is_same_v<decltype((x.x)), const int&>);
    }

	void Declval_Test()
    {

		// OK: type of n1 is int
		decltype(Default().foo()) n1 {};
		std::cout << "n1 type is '" << typeid(n1).name() << "'\n";

#if 0
		// ERROR: no default constructor
		decltype(NonDefault().foo()) n2 {};             
#endif

		// OK: type of n3 is int
		decltype(std::declval<NonDefault>().foo()) n3 {};
		std::cout << "n3 type is '" << typeid(n3).name() << "'\n";
	}
}

namespace TypeTraits::Is_Signed {

	void IsSigned_V() {
		unsigned int i = 12;
		std::cout << typeid(i).name() << " = [is signed?? ] = " << std::boolalpha << std::is_signed_v<decltype(i)> << std::endl;

		int a = 12;
		std::cout << typeid(a).name() << " = [is signed?? ] = " << std::boolalpha << std::is_signed_v<decltype(a)> << std::endl;
	}

	void IsSigned_V_Constexpr() {
		unsigned int i = 12;

		if constexpr (std::is_signed_v<decltype(i)>) {
			std::cout << "i is signed" << std::endl;
		} else { 
			std::cout << "i is unsigned" << std::endl;
		}

		int a = 12;
		if constexpr (std::is_signed_v<decltype(a)>) {
			std::cout << "a is signed" << std::endl;
		} else {
			std::cout << "a is unsigned" << std::endl;
		}
	}
}


namespace TypeTraits::Is_Invocable {

	class Functor {
	public:
		void operator() (int a) {
			std::cout << a << std::endl;
		}
	};

	void Check_Is_Invocable() {
		const auto sum = [](int a, int b)-> int { return a + b; };

		std::cout << "is_invocable<Functor(): " << std::boolalpha << std::is_invocable<Functor()>::value << std::endl;
		std::cout << std::invoke(sum, 10, 20) << std::endl;
		std::invoke(Functor(), 42);
		std::invoke([]() { std::cout << "hello" << std::endl; });
	}


	void Test() {

		auto func2 = [](char c) -> int (*)() { return nullptr;};

		std::cout << std::boolalpha << std::is_invocable<int()>::value << std::endl;
		std::cout << std::boolalpha << std::is_invocable_r<int, int()>::value << std::endl;
		std::cout << std::boolalpha << std::is_invocable_r<void, void(int), int>::value << std::endl;
		std::cout << std::boolalpha << std::is_invocable_r<int(*)(), decltype(func2), char>::value << std::endl;

		std::cout  << std::endl;

		std::cout << std::boolalpha << std::is_invocable<int>::value << std::endl;
		std::cout << std::boolalpha << std::is_invocable_r<std::string, int()>::value << std::endl;
	}
}

namespace TypeTraits::Is_Enum {

	class A {};
	enum E {};
	enum class Ec : int {};

	void Test()
	{
		std::cout << std::boolalpha;
		std::cout << std::is_enum<A>::value << '\n';    // false
		std::cout << std::is_enum<E>::value << '\n';    // true
		std::cout << std::is_enum<Ec>::value << '\n';   // true
		std::cout << std::is_enum<int>::value << '\n';  // false
	}
}

namespace TypeTraits::IsMoveConstructible {

	template<typename T>
	class Cont {
	private:
		T* elems;
	public:
		template<typename D = T>
		typename std::conditional<std::is_move_constructible<D>::value, T&&, T&>::type foo() {
		}
	};

};

namespace TypeTraits::Conditional {
	void Test()
	{
		typedef std::conditional<true,  int, double>::type Type1;
		typedef std::conditional<false, int, double>::type Type2;
		typedef std::conditional<sizeof(int) >= sizeof(double), int, double>::type Type3;

		std::cout << typeid(Type1).name() << std::endl;
		std::cout << typeid(Type2).name() << std::endl;
		std::cout << typeid(Type3).name() << std::endl;
	}
};


namespace TypeTraits::Is_Move_Constructible_Assignable {

	struct Base { 
	};

	struct Class_NoMove_Ctor { 
		Class_NoMove_Ctor(Class_NoMove_Ctor&&) = delete;
		Class_NoMove_Ctor& operator=(Class_NoMove_Ctor&& right) noexcept {
			if (this != &right) {
				// do something
			}
			return *this;
		}
	};

	struct Class_NoMove_Assign{
		Class_NoMove_Assign(Class_NoMove_Assign&&) noexcept {
			// do something
		}
		Class_NoMove_Assign& operator=(Class_NoMove_Assign&&) = delete;
	};

	void Test() {
		std::cout << std::boolalpha << "----- Is move constructible src: -----------" << std::endl;
		std::cout << "int: " << std::is_move_constructible<int>::value << std::endl;
		std::cout << "Base: " << std::is_move_constructible<Base>::value << std::endl;
		std::cout << "Class_NoMoveCtor: " << std::is_move_constructible<Class_NoMove_Ctor>::value << std::endl;
		std::cout << "Class_NoMove_Assign: " << std::is_move_constructible<Class_NoMove_Assign>::value << std::endl;


		std::cout << std::boolalpha << "\n----- Is move assignable src: -----------" << std::endl;
		std::cout << "int: " << std::is_move_assignable<int>::value << std::endl;
		std::cout << "Base: " << std::is_move_assignable<Base>::value << std::endl;
		std::cout << "Class_NoMove_Ctor: " << std::is_move_assignable<Class_NoMove_Ctor>::value << std::endl;
		std::cout << "Class_NoMove_Assign: " << std::is_move_assignable<Class_NoMove_Assign>::value << std::endl;

	}
}

namespace TypeTraits::Common_Type {

	class Base {};
	class Derived : public Base {};

	class TestClass1 : public Derived {};

	class TestClass2 : public Derived {};
	class DerivedTestClass2 : public TestClass2 {};

	void Test() {

		// std::common_type_t< TestClass1, TestClass1>;
		// std::cout << typeid(T).name() << std::endl;
	}
}

namespace TypeTraits::Alignment_Of {

	struct A {};
	struct B {
		std::int8_t p;
		std::int16_t q;
	};

	struct C {
		char a;
		char b;
	};

	void Test()
	{
		std::cout << std::alignment_of<A>::value << '\n';
		std::cout << std::alignment_of<B>::value << '\n';
		std::cout << std::alignment_of<int>() << '\n';
		std::cout << std::alignment_of_v<double> << '\n';
		std::cout << std::alignment_of_v<C> << '\n'; 
	}
}


namespace TypeTraits::Underlying_Type {

	enum e1 {};
	enum class e2 {};
	enum class e3 : unsigned {};
	enum class e4 : int {};
	enum class e5 : uint8_t {};

	void Enum_Tests() {

		constexpr bool e1_t = std::is_same_v< std::underlying_type_t<e1>, int >;
		constexpr bool e2_t = std::is_same_v< std::underlying_type_t<e2>, int >;
		constexpr bool e3_t = std::is_same_v< std::underlying_type_t<e3>, int >;
		constexpr bool e4_t = std::is_same_v< std::underlying_type_t<e4>, int >;
		constexpr bool e5_t = std::is_same_v< std::underlying_type_t<e5>, int >;

		std::cout << "underlying type for 'e1' is " << (e1_t ? "int" : "non-int") << '\n'
			<< "underlying type for 'e2' is " << (e2_t ? "int" : "non-int") << '\n'
			<< "underlying type for 'e3' is " << (e3_t ? "int" : "non-int") << '\n'
			<< "underlying type for 'e4' is " << (e4_t ? "int" : "non-int") << '\n'
			<< "underlying type for 'e5' is " << (e5_t ? "int" : "non-int") << '\n';
	}
}

namespace TypeTraits::Invoke_Result {

	struct S {
		double operator()(std::string str) { 
			return 1.0; 
		}
		float operator()(int) { 
			return 1.0;
		}
	};

	template<typename Callable, typename Type, typename... Args>
	decltype(auto) validate_callable(Callable callback, Args&&... params)
	{
		if constexpr (std::is_same<Type, std::invoke_result_t<Callable, Args...>>::value) {
			std::cout << typeid(Callable).name() << "() produce result of '"
					 //<< typeid(Callable).name()
					  << typeid(Type).name() << "' type\n";
			// Call callback
			// return callback(std::forward<Args>(args)...));
		}
	}

	void Test() {
		std::cout << "Is S(int) call produce float result ? = " << std::boolalpha
			      << std::is_same<float, std::invoke_result_t<S, int>>::value << std::endl;

		validate_callable<S, float, int>(S(), 1);
	}
}


namespace TypeTraits::TypeID
{
    void TypeID_Tests() {

        int int_var;
        std::cout << typeid(int_var).name() << std::endl;

        TestSupport::FirstType fObj("Test");
        std::cout << typeid(fObj).name() << std::endl;

        TestSupport::FirstType* ptr = new TestSupport::FirstType("TEST");
        std::cout << typeid(ptr).name() << std::endl;

        std::cout << "\n------------------ String: -----------------------" << std::endl;
        std::string str("TEST");
        std::cout << typeid(str).name() << std::endl;
    }

    struct A
    {
        virtual void fun() {}
    };

    struct B : A {};

    std::unordered_map<std::type_index, std::string> names;

    void inspector(A& a) {
        std::cout << names[typeid(a)] << "\n";
    }

    void MapOf_TypeIDs()
    {
        names[typeid(A)] = "Base";
        names[typeid(B)] = "Derived";
        names[typeid(int)] = "int";
        names[typeid(double)] = "double";

        int x = 20;
        double y = 2.4;
        std::cout << names[typeid(x)] << "\n";    // prints: "int"
        std::cout << names[typeid(y)] << "\n";    // prints: "double"

        A a;
        B b;

        inspector(a);  // prints: "Base"
        inspector(b);  // prints: "Derived"
    }
}

namespace TypeTraits::CustomTraits
{
    template<typename T, T val>
    struct integral_constant {
        static constexpr T value { val };
    };

    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;

    template<typename T>
    struct is_pointer: false_type {};

    template<typename T>
    struct is_pointer<T*>: true_type {};

    void Is_Pointer_2()
    {
        static_assert(is_pointer<int*>::value);
        static_assert(not is_pointer<int>::value);
    }
}

void TypeTraits::TestAll()
{
	// IsClass::Test();

	// Conditional::Test();

	// IsArray::Test();
	// IsArray::Enable_If_Test();

	// TypeID::TypeID_Tests();
	// TypeID::MapOf_TypeIDs();

	// Is_Function::Test();
	// Is_Lvalue_Reference::Test();
	// Is_Member_Function_Pointer::Test();
	// Is_Abstract::Test();
	// Is_Signed::IsSigned_V();
	// Is_Signed::IsSigned_V_Constexpr();
	// Is_Enum::Test();

	// Is_Invocable::Check_Is_Invocable();
	// Is_Invocable::Test();

	// Is_Same::Test();

	// Is_Base_Of::Test();

	// Is_Move_Constructible_Assignable::Test();

	// CustomTraits::Void_Test();
	// CustomTraits::Is_Pointer();
	// CustomTraits::Is_Pointer_2();
	// CustomTraits::Color_Test();

	Decltype_and_Declval::BasicTests();
	Decltype_and_Declval::Test();
	// Decltype_and_Declval::Test2();
	// Decltype_and_Declval::Declval_Test();

	// Common_Type::Test();

	// Alignment_Of::Test();

	// Underlying_Type::Enum_Tests();

	// Invoke_Result::Test();

	// std::false_type ??????
};
