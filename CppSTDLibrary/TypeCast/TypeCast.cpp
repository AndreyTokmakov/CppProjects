//============================================================================
// Name        : TypeCastTests.cpp
// Created on  : 
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Type casts src
//============================================================================

#include <cassert>
#include <iostream>
#include <string>
#include <string_view>

#include <charconv>
#include <memory>
#include <functional>

#include <list>
#include <vector>

#include "../Helpers/Helpers.h"
#include "TypeCast.h"

using String = std::string;
using CString = const String&;

namespace TypeCast
{
	class Base {
	public:
		virtual void info() const noexcept {
			std::cout << "Base::info()" << std::endl;
		}
	};


	class Derived: public Base {
	public:
		virtual void info() const noexcept override {
			std::cout << "Deriverd::info()" << std::endl;
		}
	};


	class Parent {
	protected:
		int value;

	public:
		Parent(int val): value(val) {
			// TODO
		}
		virtual ~Parent() {}

	public:
		virtual void info() const noexcept {
			std::cout << "Parent::info()" << std::endl;
		}
	};


	class Child : public Parent {
	protected:
		std::string name;

	public:
		Child(int value, const std::string& name) : Parent(value), name(name) {
			// TODO
		}

		const std::string& getName() noexcept { 
			return this->name; 
		}

	public:
		virtual void info() const noexcept override {
			std::cout << "Child::info()" << std::endl;
		}
	};


	class TheType {
	public:
		int i;

		TheType() : i(3) {
		}

		void Const_Set_Value(int v) const {
			// this->i = v;                 // compile error: this is a pointer to const
			const_cast<TheType*>(this)->i = v; // OK as long as the type object isn't const
		}
	};


	Parent* getChild() {
		return new Child(1, "Banana");
	}

	Parent* getParent() {
		return new Parent(1);
	}

	std::shared_ptr<Parent> getObjectEx() {
		return std::make_shared<Child>(1, "Banana");
	}
};


namespace TypeCast
{

	void DynamicCastTests_0() {
		Parent* parent = new Parent(1);
		parent->info();

		if (Parent* ptr = dynamic_cast<Parent*>(new Child(1, "Banana")); nullptr != ptr) {
			ptr->info();
		}

		if (Parent* ptr = dynamic_cast<Parent*>(new Derived()); nullptr != ptr) {
			ptr->info();
		}
		else {
			std::cout << "Failed to cast ptr Derived() --> ptr Parent()\n";
		}
	}

	void DynamicCastTests() {
		// Old style:
		Child* obj = (Child*)getChild();
		obj->info();

		// C++ style:
		Parent *p = getChild();
		Child *ch = dynamic_cast<Child*>(p);
		ch->info();

		// C++ style:
		Parent *p1 = getChild();
		p1->info();
	}

	void DynamicCastTests_2() {
		// Old style:
		Child* obj = (Child*)getChild();
		obj->info();

		// C++ style:
		Parent *p = getChild();
		Child *ch = dynamic_cast<Child*>(p);
		ch->info();
	}

	void DynamicCastTests2() {
		if (Child *ch = dynamic_cast<Child*>(getChild())) {
			ch->info();
		} else {
			std::cout << "Failed to cast getChild() to Child* type" << std::endl;
		}

		if (Child *ch = dynamic_cast<Child*>(getParent())) {
			ch->info();
		} else {
			std::cout << "Failed to cast getParent() to Child* type" << std::endl;
		}
	}

	void StaticCast()
	{
		int i = 3;                 // i is not declared const
		const int& rci = i;
		const_cast<int&>(rci) = 431; // OK: modifies i
		std::cout << "i = " << i << std::endl;

		TheType t; // if this was const type t, then t.f(4) would be undefined behavior
		t.Const_Set_Value(4);
		std::cout << "type::i = " << t.i << std::endl;

		const int j = 3; // j is declared const
		int* pj = const_cast<int*>(&j);
		*pj = 4;      // undefined behavior
		std::cout << j << std::endl;

		void (TheType::* pmf)(int) const = &TheType::Const_Set_Value; // pointer to member function
		// const_cast<void(type::*)(int)>(pmf);   // compile error: const_cast does
												  // not work on function pointers
	}

	void DynamicCastTests3() {
		try {
			Base *derived = new Derived();
			Base *base = new Base();

			Derived * derived_new = nullptr;

			derived_new = dynamic_cast<Derived*>(derived);
			if (derived_new == nullptr)
				std::cout << "[Failed]:  derived_new = dynamic_cast<Derived*>(derived)" << std::endl;
			else {
				std::cout << "[OK]:  derived_new = dynamic_cast<Derived*>(derived)" << std::endl;
			}


			derived_new = dynamic_cast<Derived*>(base);
			if (derived_new == nullptr)
				std::cout << "[Failed]:  derived_new = dynamic_cast<Derived*>(base)" << std::endl;
			else {
				std::cout << "[OK]:  derived_new = dynamic_cast<Derived*>(base)" << std::endl;
			}

		}
		catch (const std::exception& e) { 
			std::cout << "Exception: " << e.what(); 
		}
	}

	void DynamicCastTests4() {
		try {
			Base *derived = new Derived();
			Base *base    = new Derived();
			Derived * derived_new = nullptr;

			derived_new = dynamic_cast<Derived*>(derived);
			if (derived_new == nullptr)
				std::cout << "[Failed]:  derived_new = dynamic_cast<Derived*>(derived)" << std::endl;
			else {
				std::cout << "[OK]:  derived_new = dynamic_cast<Derived*>(derived)" << std::endl;
			}

			derived_new = dynamic_cast<Derived*>(base);
			if (derived_new == nullptr)
				std::cout << "[Failed]:  derived_new = dynamic_cast<Derived*>(base)" << std::endl;
			else {
				std::cout << "[OK]:  derived_new = dynamic_cast<Derived*>(base)" << std::endl;
			}

		}
		catch (const std::exception& e) {
			std::cout << "Exception: " << e.what();
		}
	}

	void PtrCasts() {
		auto basePtr = std::make_shared<Base>();
		std::cout << "Base pointer says: ";
		basePtr->info();

		auto derivedPtr = std::make_shared<Derived>();
		std::cout << "Derived pointer says: ";
		derivedPtr->info();

		// static_pointer_cast to go up class hierarchy
		basePtr = std::static_pointer_cast<Base>(derivedPtr);
		std::cout << "Base pointer to derived says: ";
		basePtr->info();

		// dynamic_pointer_cast to go down/across class hierarchy
		if (auto downcastedPtr = std::dynamic_pointer_cast<Derived>(basePtr); downcastedPtr) {
			std::cout << "Downcasted pointer says: ";
			downcastedPtr->info();
		}

		// All pointers to derived share ownership
		std::cout << "Pointers to underlying derived: " << derivedPtr.use_count() << std::endl;
	}

	template<class Type>
	Type atoi_17(std::string_view str) {
		Type res{};
		std::from_chars(str.data(), str.data() + str.size(), res);
		return res;
	}

	void TypyCastingCpp17Test() {
		std::cout << "Integer value : " << atoi_17<int>("234") << std::endl;
		std::cout << "Integer value : " << atoi_17<int>("2s34") << std::endl;
		std::cout << "Integer value : " << atoi_17<int>("a234") << std::endl;

		std::cout << "Double value : " << atoi_17<double>("234.345") << std::endl;
		std::cout << "Double value : " << atoi_17<double>("234.1a2") << std::endl;
		std::cout << "Double value : " << atoi_17<double>("23a4.454") << std::endl;
	}

	void ToChars_FromChars() {
		char arr[128];
		auto res1 = std::to_chars(std::begin(arr), std::end(arr), 3.14f);
		if (res1.ec != std::errc::value_too_large) {
			std::cout << arr << std::endl;
		}

		float val;
		auto res2 = std::from_chars(std::begin(arr), std::end(arr), val);
		if (res2.ec != std::errc::invalid_argument && res2.ec != std::errc::result_out_of_range) {
			std::cout << arr << std::endl;
		}
	}



	void VectorTest() {
		std::vector v{ std::vector{1, 2} };

		// This is vector<int>, but not vector<vector<int>>
		static_assert(std::is_same_v<std::vector<int>, decltype(v)>);

		// Size is equal 2
		assert(v.size() == 2);

		std::cout << "Done" << std::endl;
	}

	void Is_Const()
	{
		std::cout << std::boolalpha;
		std::cout << "std::is_const<int>::value   ===>  "        << std::is_const<int>::value << std::endl; // false
		std::cout << "std::is_const<const int>::value  ===>  "   << std::is_const<const int>::value << std::endl; // true
		std::cout << "std::is_const<const int*>::value   ===>  " << std::is_const<const int*>::value << std::endl; // false
		std::cout << "std::is_const<int* const>::value   ===>  " << std::is_const<int* const>::value << std::endl; // true
		std::cout << "std::is_const<const int&>::value   ===>  " << std::is_const<const int&>::value << std::endl; // false
	}

	void As_Const()
	{
		std::string mutableString = "Hello World!";
		const std::string& constView = std::as_const(mutableString);
		const std::string& notConstView  = mutableString;

		// assert(&constView == &mutableString);
		// assert(&std::as_const(mutableString) == &mutableString);

		using WhatTypeIsIt = std::remove_reference_t<decltype(std::as_const(mutableString))>;

		if (false == std::is_same<std::remove_const_t<WhatTypeIsIt>, std::string>::value) {
			std::cout << "WhatTypeIsIt should be some kind of string." << std::endl;
		}
		else {
			std::cout << "WhatTypeIsIt is same string." << std::endl;
		}

		if (true == std::is_same<std::remove_const_t<WhatTypeIsIt>, std::string>::value) {
			std::cout << "WhatTypeIsIt shouldn't be a mutable string." << std::endl;
		}
		else {
			std::cout << "WhatTypeIsIt is not same as mutable string." << std::endl;
		}

		// static_assert(std::is_same<std::remove_const_t<WhatTypeIsIt>, std::string>::value, "WhatTypeIsIt should be some kind of string.");
		// static_assert(!std::is_same< WhatTypeIsIt, std::string >::value, "WhatTypeIsIt shouldn't be a mutable string.");
	}
};

namespace TypeCast::StaticCastTests
{
	class Base {};

	class Derived : public Base {};

	class MyClass {};

	void Test() {
		Derived* d = new Derived;
		Base* b = static_cast<Base*>(d);        // this line will work properly
		//            MyClass* x = static_cast<MyClass*>(d);   // ERROR will be generated during
	}
};


namespace TypeCast::FromChars_Tests
{
    void ConstExprTests()
    {
        constexpr std::string_view text { "a123a456" };
        int dest {0};

        const auto result  = std::from_chars(text.begin(), text.end(), dest);
        // static_assert(result.ec != std::errc());
        if (result.ec == std::errc()) {
            std::cout << "dest = " << dest << std::endl;
        }
    }


    void InvalidNumericString()
    {
        constexpr std::string_view text { "123a456" };
        int dest {0};

        const auto [ptr, errCode]  = std::from_chars(text.begin(), text.end(), dest);
        if (errCode == std::errc()) {
            std::cout << "dest = " << dest << std::endl;
        }

        std::cout << dest << std::endl;

        /*
        if (errCode != std::errc::invalid_argument && errCode != std::errc::result_out_of_range) {
            std::cout << "ERR" << std::endl;
        }
        */
    }
};


namespace TypeCast::ReturnTypeCast
{
    struct Value
    {
        std::string value{"123.456"};

        operator short() {
            std::cout << "Value::operator short()\n";
            return 123;
        }

        operator unsigned int() {
            std::cout << "Value::operator unsigned int()\n";
            return 123;
        }

        explicit operator int() {
            std::cout << "Value::operator int()\n";
            return 123;
        }

        operator double() {
            std::cout << "Value::operator double()\n";
            return 123.456f;
        }
    };

    void tests()
    {
        Value val;

        short _ = val;
        unsigned int _ = val;
        int _ = val;
        // float float_val = val;
        double _ = val;
    }
}



void TypeCast::TestAll()
{
	// DynamicCastTests_0();
	// DynamicCastTests();
	// DynamicCastTests2();
	// DynamicCastTests3();
	// DynamicCastTests4();

    ReturnTypeCast::tests();

	// PtrCasts();

	// StaticCastTests::Test();
	// StaticCast();

	// VectorTest();

	// TypyCastingCpp17Test();
	// ToChars_FromChars();

	// Is_Const();
	// As_Const();

    // FromChars_Tests::ConstExprTests();
    // FromChars_Tests::InvalidNumericString();
}