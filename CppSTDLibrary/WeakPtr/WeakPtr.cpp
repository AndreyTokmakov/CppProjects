//============================================================================
// Name        : WeakPtrTests.h
// Created on  : 23.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Weak PTR C++  src
//============================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <memory>

#include "../Helpers/Helpers.h"
#include "WeakPtr.h"
#include "WeakPtr_MakeShared.h"

namespace {
	using Helpers::Integer;
}


namespace WeakPtr {

	class Human {
	private:
		std::string m_name;

		// std::shared_ptr<Human> m_partner; // If uncommentes all will go wrong
		 std::weak_ptr<Human> m_partner;

	public:
		Human(const std::string &name) : m_name(name) {
			std::cout << m_name << " created" << std::endl;
		}

		~Human() {
			std::cout << m_name << " destroyed" << std::endl;
		}

		friend void partnerUp(std::shared_ptr<Human>& h1, std::shared_ptr<Human> &h2) noexcept {
			if (h1 && h2) {
				h1->m_partner = h2;
				h2->m_partner = h1;
				std::cout << h1->m_name << " is now partnered with " << h2->m_name << std::endl;
			}
		}
	};
};

namespace WeakPtr {

	void SimpleTest()
	{
		auto anton = std::make_shared<Human>("Anton");
		auto ivan = std::make_shared<Human>("Ivan");
		partnerUp(anton, ivan);
	}

	void GetControlledObject()
	{
		auto sharedInteger = std::make_shared<Integer>(12345);
		std::weak_ptr<Integer> weak = sharedInteger;

		std::cout << "Weak prt's <T> object value = " << weak.lock()->getValue() << std::endl;
	}

	void Expired() {
		std::shared_ptr<Integer> integer = std::make_shared<Integer>(123);
		std::weak_ptr<Integer> weak(integer);

		std::cout << "1. weak " << (weak.expired() ? "is" : "is not") << " expired" << std::endl;
		integer.reset();
		std::cout << "2. weak " << (weak.expired() ? "is" : "is not") << " expired" << std::endl;
	}

	void Expired2() {
		std::weak_ptr<Integer> weak;
		const auto is_expired = [](const std::weak_ptr<Integer> ptr) {
			if (true == ptr.expired())
				std::cout << "WeakPtr has expired." << std::endl;
			else
				std::cout << "WeakPtr is actual." << std::endl;
		};

		{
			std::shared_ptr<Integer> integer = std::make_shared<Integer>(111);
			weak = integer;
			is_expired(weak);
		}
		is_expired(weak);

		{
			std::shared_ptr<Integer> integer = std::make_shared<Integer>(222);
			weak = integer;
			is_expired(weak);
		}
		is_expired(weak);
	}


	void Lock() {
		const auto observe = [] (std::weak_ptr<int> weak)-> void {
			if (std::shared_ptr<int> observe = weak.lock()) {
				std::cout << "\tobserve() able to lock weak_ptr<>, value=" << *observe << std::endl;
			} else {
				std::cout << "\tobserve() unable to lock weak_ptr<>." << std::endl;
			}
		};

		std::weak_ptr<int> weak;
		std::cout << "weak_ptr<> not yet initialized." << std::endl;
		observe(weak);

		{
			auto shared = std::make_shared<int>(42);
			weak = shared;
			std::cout << "weak_ptr<> initialized with shared_ptr." << std::endl;
			observe(weak);
		}

		std::cout << "shared_ptr<> has been destructed due to scope exit." << std::endl;
		observe(weak);
	}

	//-------------------------------------------------------------------------//

	void use(std::shared_ptr<Integer> integer) {
		integer->printInfo();
	}

	void getSharedPointer(std::weak_ptr<Integer> wptr) {
		// if we had the Weap ptr and we want to see if the object is there
		std::shared_ptr<Integer> sptr = wptr.lock();
		if (nullptr != sptr) {
			use(sptr);
		}
	}

	void GetOwned_SharedPointer()
    {
		std::shared_ptr<Integer> integer = std::make_shared<Integer>(123);
		std::weak_ptr<Integer> weak(integer);

		getSharedPointer(weak);
	}



	void Vector_of_WeakPtr() {
		std::vector<std::shared_ptr<Integer>> original;
		for (int i : {0, 1, 2, 3, 4, 5, 6, 7, 8, 9})
			original.push_back(std::make_shared<Integer>(i));

		std::vector<std::weak_ptr<Integer>> weakVector;
		std::for_each(original.begin(), original.end(), [&weakVector](auto ptr) { 
			weakVector.emplace_back(ptr); 
		});

		auto check = [&weakVector] {
			for (const auto& ptr : weakVector)
				std::cout << std::boolalpha << ptr.expired() << std::endl;
		};

		check();

		std::cout << "Reset: original[5].reset()" << std::endl;
		original[5].reset();

		check();
	}
};

namespace WeakPtr::WeakFromThis
{

	class TestString : public std::enable_shared_from_this<TestString> {
	private:
		std::string value;

	public:
		template<typename... T>
		explicit TestString(T&&... params) : value(std::forward<T>(params)...) {
		}
	
		TestString(const TestString &obj)  : enable_shared_from_this(obj) {
			std::cout << "SharedString::SharedString(" << this->value << "). Copy constructor" << std::endl;
			this->value = obj.value;
		}

		virtual ~TestString() {
			std::cout << "SharedString::~SharedString(" << this->value << ")" << std::endl;
		}

		std::shared_ptr<TestString> getSharedPrt() {
			return this->shared_from_this();
		}

		std::weak_ptr<TestString> getWeakPrt() {
			return this->weak_from_this();
		}
	};

	void EnableShareFromThis()
    {
		const auto check = [](auto weak)-> void {
			std::cout <<  "Expired = " << std::boolalpha << weak.expired() << std::endl;
			if (auto observe = weak.lock()) {
				std::cout << "Lock OK. Use count: " << weak.use_count() << std::endl;
			}
			else {
				std::cout << "Lock Failed" << std::endl;
			}
			std::cout << std::endl;
		};

		std::weak_ptr<TestString> weak;
		check(weak);

		{
			std::shared_ptr<TestString> sharedStrPtr = std::make_shared<TestString>("SomeTestValue");
			//std::cout << typeid(sharedStrPtr).name() << std::endl;

			weak = sharedStrPtr->getWeakPrt();
			check(weak);
		}

		check(weak);
		//std::cout << typeid(weak).name() << std::endl;
	}
}

void Use_Count()
{
    std::shared_ptr<Integer> integer = std::make_shared<Integer>(123);

    std::cout << "Use count = " << integer.use_count() << std::endl;

    std::weak_ptr<Integer> wInt = integer;

    std::cout << "Use count = " << integer.use_count() << std::endl;
    if (auto ptr = wInt.lock(); true) {
        std::cout << "Use count = " << integer.use_count() << std::endl;
    }
}

void Use_Count_1()
{

    std::weak_ptr<Integer> wInt;

	std::cout << std::string(100, '-') << std::endl;
    {
        const std::shared_ptr<Integer> integer = std::make_shared<Integer>(123);
        wInt = integer;

        std::cout << "Use count = " << wInt.use_count() << std::endl;
    }
	std::cout << std::string(100, '-') << std::endl;

    std::cout << "Use count = " << wInt.use_count() << std::endl;
     std::shared_ptr<Integer>  ptr = wInt.lock();

    if (!ptr) {
        std::cout << "Null" << std::endl;
    }

}

void WeakPtr::TestAll()
{
    // WeakPtr_MakeShared::TestAll();


	// SimpleTest();

	// GetControlledObject();

	// Expired();

	// Expired2();

	// Lock();

	// GetOwned_SharedPointer();

    // WeakFromThis::EnableShareFromThis();

	// Vector_of_WeakPtr();

    // Use_Count();
    Use_Count_1();
}