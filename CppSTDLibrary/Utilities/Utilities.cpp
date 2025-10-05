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
#include <utility>
#include <memory>
#include <cassert>
#include <iomanip>
#include <queue>
#include <deque>
#include "Utilities.hpp"


namespace
{
    template<typename _Ty1, typename _Ty2>
    void assertEquals(const _Ty1& a, const _Ty2& b)
    {
        if (a != b) {
            std::cerr << "Error: " << a << " != " << b << std::endl;
            std::terminate();
        }
    }
}

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


namespace Utilities::InvokeR
{
    struct Base {};
    struct Derived : Base {};

    Base base {};
    Derived derived {};

    Base& fun(int) { return base; }
    Derived& fun(double) { return derived; }

    /** Wrapper for the overload set **/
    auto wrapped = [](auto arg) -> decltype(auto) {
        return fun(arg);
    };

    void Deduce_Invocation_Return_Type()
    {
        auto& i1 = std::invoke(wrapped, 42);           // calls fun(int)    --> Base&
        auto& i2 = std::invoke(wrapped, 4.2);        // calls fun(double) --> Derived&

        static_assert(std::is_same_v<decltype(i1), Base&>);
        static_assert(not std::is_same_v<decltype(i1), Derived&>);
        static_assert(std::is_same_v<decltype(i2), Derived&>);
        static_assert(not std::is_same_v<decltype(i2), Base&>);

        auto& i3 = std::invoke_r<Base&>(wrapped, 42);  // calls fun(int)    --> Base&
        auto& i4 = std::invoke_r<Base&>(wrapped, 4.2); // calls fun(double) --> Base&

        static_assert(std::is_same_v<decltype(i3), Base&>);
        static_assert(not std::is_same_v<decltype(i3), Derived&>);
        static_assert(std::is_same_v<decltype(i4), Base&>);
        static_assert(not std::is_same_v<decltype(i4), Derived&>);
    }
}

namespace Utilities::Make_Tuples
{
	struct Foo
    {
		Foo(int first, float second, int third) {
			std::cout << first << ", " << second << ", " << third << std::endl;
		}
	};

	class Object
    {
		std::string str1;
		std::string str2;

	public:
		Object(std::string  s1, std::string  s2 ): str1(std::move(s1)), str2(std::move(s2)) {
		}

		[[nodiscard]]
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



namespace Utilities::ToAddress
{
    void to_address_tests()
    {
        auto t1 = std::make_unique<int>(1);
        int *p1 = std::to_address(t1);
        assertEquals(p1, t1.get());

        std::unique_ptr<int> t2; /// empty smart pointer
        int *p2 = std::to_address(t2);
        assertEquals(p2, nullptr); /// p2 == nullptr

        int x = 0, *t3 = &x;
        int *p3 = std::to_address(t3);
        assertEquals(t3, p3); /// t3 == p3

        std::vector<int> rng{1,2,3}; /// Also works for contiguous iterators
        int *p4 = std::to_address(rng.begin());
        assertEquals(p4, rng.data()); /// p4 == rng.data()
    }
}

namespace Utilities::Unreachable
{
    enum class RGB {
        Red,
        Green,
        Blue,

        NotAllowed
    };

    std::string colorToStr(RGB color) {
        using
        enum RGB;
        switch (color) {
            case Red:
                return "Red";
            case Green:
                return "Green";
            case Blue:
                return "Blue";
            default:
                std::unreachable();
        }
    }

    void Test()
    {
        std::cout << colorToStr(RGB::Red) << std::endl;
        std::cout << colorToStr(RGB::Green) << std::endl;
        std::cout << colorToStr(RGB::Blue) << std::endl;
        std::cout << colorToStr(RGB::NotAllowed) << std::endl;

        std::cout << "Will not get here\n";
    }
}

namespace TypeIdentity
{
    template<typename T>
    T add_old(T a, T b)
    {
        return a + b;
    }

    template<typename T>
    T add(T a, std::type_identity_t<T> b)
    {
        return a + b;
    }

    void Test()
    {
        /** Will not compile **/
        // add_old(1, 0.5);

        add(1, 0.5);
        add(0.5, 1);
    }
}

namespace Utilities::Quoted
{
    void Quoted_Tests()
    {
        std::cout << std::quoted(R"(I say: "Hello Wordl!")") << std::endl;
        /** "I say: \"Hello World!\"" **/


        std::stringstream s(R"("I say: \"Hello World!\"")");
        std::string unescaped;
        s >> std::quoted(unescaped);


        // unescaped == I say: "Hello World!"
        std::cout << unescaped << std::endl;

    }


    void Quoted_SetQuote_Character()
    {
        {
            const std::string& src {"10 20 30"};
            std::cout << src << " -> " << std::quoted(src, '|') << std::endl;  // '|' --> is quote character
            // 10 20 30 -> |10 20 30|
        }

        {
            const std::string& src {"Some Text"};
            std::cout << src << " -> " << std::quoted(src, '*') << std::endl;  // '*' --> is quote character
            // Some Text -> *Some Text*
        }
    }
}


namespace Utilities::ForwardLike
{
#if 0
    struct adapter
    {
        std::deque<std::string> container;

        std::string operator[](this auto&& self, size_t i)
        {
            return std::forward_like<decltype(self)>(container[i]);
        }
    };
#endif

    struct Wrapper {
        int member;
    };

    void fun(const int&) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    void fun(int&) { std::cout << __PRETTY_FUNCTION__ << std::endl; }
    void fun(int&&) { std::cout << __PRETTY_FUNCTION__ << std::endl; }

    /** Only calls fun(int&) or fun(const int&) **/
    void extract1(auto&& wrapper)
    {
        fun(wrapper.member);
    }

    /** Correct, but cumbersome **/
    void extract2(auto&& wrapper)
    {
        if constexpr (std::is_rvalue_reference_v<decltype(wrapper)>)
        {
            fun(std::move(wrapper.member));
        }
        else
        {
            fun(wrapper.member);
        }
    }

    /** Using C++23 forward_like **/
    void extract3(auto&& wrapper)
    {
        fun(std::forward_like<decltype(wrapper)>(wrapper.member));
    }

    struct MyType
    {
        auto&& get(this auto&& self)
        {
            // One getter variant covering all value categories
            return std::forward_like<decltype(self)>(self.data);
        }
        int data;
    };

    void Old_Style_Forward()
    {
        Wrapper w;
        extract1(w);                      // calls fun(int&)
        extract1(std::as_const(w));  // calls fun(const int&)
        extract1(Wrapper{});         // calls fun(int&)

        extract2(w);                      // calls fun(int&)
        extract2(std::as_const(w));  // calls fun(const int&)
        extract2(Wrapper{});         // calls fun(int&&)
    }

    void Forward_Like()
    {
        Wrapper w;

        extract3(w);                     // calls fun(int&)
        extract3(std::as_const(w)); // calls fun(const int&)
        extract3(Wrapper{});        // calls fun(int&&)

        // void Utilities::ForwardLike::fun(int&)
        // void Utilities::ForwardLike::fun(const int&)
        // void Utilities::ForwardLike::fun(int&&)
    }
}


namespace Utilities::ForwardLike_Lambda
{
	struct Scheduler
	{
		std::string name;

		void submit(const std::string& str) const {
			std::cout << __PRETTY_FUNCTION__ << " | " << name << " : " <<  str << std::endl;
		}

		void submit(std::string&& str) const {
			std::cout << __PRETTY_FUNCTION__ << " | " << name << " : " <<  str << std::endl;
		}
	};

	[[nodiscard]]
	std::string get_message()
	{
		return { "notification" };
	}


	void Move_and_Copy_OldStyle()
	{
		Scheduler scheduler { "Test_Scheduler" };


		auto callback1 = [message=get_message(), &scheduler]() {
			scheduler.submit(message);
		};
		auto callback2 = [message=get_message(), &scheduler]() mutable {
			scheduler.submit(std::move(message));
		};

		/**
		 So what if we want to submit a copy of the message to the scheduler in the first case to be able to repeat the call,
		 and in the second case - move the message to the scheduler. That is, we would like to adjust the type of
		 the class field based on the type of the object reference and pass the field to internal calls.
		**/

		callback1();
		callback2();

		// void Utilities::ForwardLike_Lambda::Scheduler::submit(const std::string&) const | Test_Scheduler : notification
		// void Utilities::ForwardLike_Lambda::Scheduler::submit(std::string&&) const | Test_Scheduler : notification
	}


	void Move_and_Copy_DeducingThis()
	{
		Scheduler scheduler { "Test_Scheduler" };
		auto callback = [message=get_message(), &scheduler](this auto &&self) {
			return scheduler.submit(std::forward_like<decltype(self)>(message));
		};

		/**
		 So what if we want to submit a copy of the message to the scheduler in the first case to be able to repeat the call,
		 and in the second case - move the message to the scheduler. That is, we would like to adjust the type of
		 the class field based on the type of the object reference and pass the field to internal calls.
		**/

		callback();
		std::move(callback)();

		// void Utilities::ForwardLike_Lambda::Scheduler::submit(const std::string&) const | Test_Scheduler : notification
		// void Utilities::ForwardLike_Lambda::Scheduler::submit(std::string&&) const | Test_Scheduler : notification
	}
}

namespace Extent
{
	void getArraySize()
	{
		const int values[]{1, 2, 3, 4};
		static_assert(std::extent_v<decltype(values)> == 4);


	}
}


void Bind_Tests();
void Decay_Tests();

void Utilities::TestAll()
{
    // Bind_Tests();
    // Decay_Tests();
    // Invoke::TestAll();
	// NullPtr_T::TestAll();
	// CompilerVersion::TestAll();
	ScopeExit::TestAll();

	// Extent::getArraySize();

	// ApplyTests::Apply_Sum_Test();
	// ApplyTests::Apply_Sum_Lambda_Test();
	// ApplyTests::Apply_Sum_Tuple();

    //InvokeR::Deduce_Invocation_Return_Type();

	// Make_Tuples::Test();
	// Make_Tuples::Test2();

	// Utilities_Library::Ptrdiff_t();

    // In_Range();

    /*
	Integer_Comparison_Functions::Compare_Greater_Bad();
	Integer_Comparison_Functions::Compare_Greater_Better();
	Integer_Comparison_Functions::Compare_Greater_Perfect();
	Integer_Comparison_Functions::Tests();
    */

    // ForwardLike::Old_Style_Forward();
    // ForwardLike::Forward_Like();

    // ForwardLike_Lambda::Move_and_Copy_OldStyle();
    // ForwardLike_Lambda::Move_and_Copy_DeducingThis();


    // ToAddress::to_address_tests();

    // TypeIdentity::Test();

    // Unreachable::Test();


    // Quoted::Quoted_Tests();
    // Quoted::Quoted_SetQuote_Character();
};
