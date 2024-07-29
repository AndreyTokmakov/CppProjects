//============================================================================
// Name        : Set_Testing.cpp
// Created on  : 16.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ STL Set container testing
//============================================================================

#include <algorithm>
#include <cassert>
#include <iostream>
#include <ostream>
#include <iterator>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#include "../Helpers/Integer.h"
#include "Set.h"

namespace Set {

	/** User class: **/
	class User {
	private:
		std::string name;

	public:
		User(std::string s) : name(std::move(s)) {
			std::cout << "User::User(" << name << ")" << std::endl;
		}
		virtual ~User() {
			std::cout << "User::~User(" << name << ")" << std::endl;
		}
		User(const User& u) : name(u.name) {
			std::cout << "User::User(copy, " << name << ")" << std::endl;
		}

		void setName(std::string n) {
			this->name = n;
		}

		inline std::string getName() const noexcept {
			return name;
		}

		// Here goes operator overrides:
	public:
		friend bool operator<(const User& u1, const User& u2) {
			return u1.name < u2.name;
		}

		friend std::ostream& operator<<(std::ostream& os, const User& user) {
			os << user.name;
			return os;
		}
	};


	/** Object class: **/
	class Object {
	private:
		std::string name;

	public:
		Object(std::string s) : name(std::move(s)) {
			//std::cout << "User::User(" << name << ")" << std::endl;
		}
		virtual ~Object() {
			//std::cout << "User::~User(" << name << ")" << std::endl;
		}
		Object(const Object& u) : name(u.name) {
			//std::cout << "User::User(copy, " << name << ")" << std::endl;
		}

		inline std::string getName() const noexcept {
			return name;
		}

		friend bool operator<(const Object& u1, const Object& u2) {
			return u1.name < u2.name;
		}
	};

	/** BadObject class: **/
	class BadObject {
	private:
		std::string name;

	public:
		BadObject(std::string s) : name(std::move(s)) {
			//std::cout << "User::User(" << name << ")" << std::endl;
		}
		virtual ~BadObject() = default;

		BadObject(const BadObject& u) : name(u.name) {
			//std::cout << "User::User(copy, " << name << ")" << std::endl;
		}

		BadObject(BadObject&& obj) : name(std::move(obj.name)) {
			//std::cout << "User::User(copy, " << name << ")" << std::endl;
		}

		inline std::string getName() const noexcept {
			return name;
		}

		/*
		friend bool operator<(const BadObject& u1, const BadObject& u2) {
			return u1.name < u2.name;
		}*/
	};
}

namespace Set {
	auto printer_coma = [](const auto& val) {
		std::cout << val << ", ";
	};

	auto print_set = [](const auto& set) {
		std::for_each(set.begin(), set.end(), printer_coma);
	};

	auto print_set_ex = [](const auto& set,
		std::string_view text,
		std::string_view postfix = "\n") {
		std::cout << text;
		print_set(set);
		std::cout << postfix << std::endl;
	};

	void InitTest() {
		std::set<std::string> string_set{ "Value1", "Value2", "Value3", "Value4", "Value5", "Value5" };
		print_set_ex(string_set, "Set contains:");

		std::set<int> int_set{ 1,2,3,4,5,6,7,8,9 };
		print_set_ex(int_set, "Set contains:");


		std::set<int> int_set1{ 33,12 };
		print_set_ex(int_set1, "Set contains:");
	}


	void InitTest_Bad() {
#if 0
		std::set<BadObject> bad_set;
		bad_set.emplace("Test");
#endif
	}

	void InsertTests() {
		std::set<User> users;

		users.insert(User("John"));
		users.insert(User("Alex"));
		users.insert(User("Jack"));
		users.insert(User("Nick"));

		std::cout << "Try insert duplicates" << std::endl;

		users.insert(User("Nick"));
		users.insert(User("Nick"));

		std::cout << "\nUsers : " << std::endl;
		std::for_each(users.begin(), users.end(), [](const User& u) { std::cout << u.getName() << std::endl;  });
		std::cout << std::endl;
	}

	void EmplaceTest() {
		std::set<Integer> ints;

		std::pair<std::set<Integer>::const_iterator, bool> result;

		result = ints.emplace(111);
		assert(result.second);
		result = ints.emplace(222);
		assert(result.second);
		assert(ints.emplace(333).second);

		std::cout << "\n         Try emplace duplicates 1: \n" << std::endl;
		result = ints.emplace(333);
		assert(false == result.second);

		std::cout << "\n         Try emplace duplicates 2: \n" << std::endl;
		result = ints.emplace(Integer(333));
		assert(false == result.second);

		std::cout << "\nIntegers : " << std::endl;
		print_set_ex(ints, "Integers:");
		std::cout << std::endl;
	}

	void EmplaceTest1() {
		std::set<User> users;

		users.emplace(User("John"));
		users.emplace(User("Alex"));
		users.emplace(User("Jack"));
		users.emplace(User("Nick"));

		std::cout << "Try emplace duplicates" << std::endl;

		users.emplace(User("Nick"));
		users.emplace(User("Nick"));

		std::cout << "\nUsers : " << std::endl;
		print_set_ex(users, "Users:");
		std::cout << std::endl;
	}

	void FindTest() {
		std::set<int> users = { 1, 3, 45, 55, 555 };
		std::set<int>::const_iterator result = users.end();

		result = users.find(55);
		if (users.end() == result) {
			std::cout << "Not found" << std::endl;
			return;
		}
		std::cout << *result << std::endl;
	}

	void FindTest_Class() {
		std::set<Object> objects;
		objects.emplace("John");
		objects.emplace("Alex");
		objects.emplace("Jack");
		objects.emplace("Nick");


		std::set<Object>::const_iterator result = objects.end();
		std::vector<Object> objsToFind{ Object("John"), Object("Max") };

		for (const auto& obj : objsToFind) {
			result = objects.find(obj);
			if (objects.end() == result) {
				std::cout << "Not found" << std::endl;
			}
			else {
				std::cout << result->getName() << std::endl;
			}
		}
	}

	void ExtractTests() {
		std::set<User> outSet, setNames;
		setNames.emplace("John");
		setNames.emplace("Alex");

		print_set_ex(setNames, "  >>  setNames: ");
		print_set_ex(setNames, "  >>  outSet: ");

		std::cout << "\nmove John..." << std::endl;
		// move John to the outSet
		auto john = setNames.extract(User("John"));
		outSet.insert(std::move(john));

		print_set_ex(setNames, "  >>  setNames: ");
		print_set_ex(setNames, "  >>  outSet: ");

		std::cout << "\ncleanup..." << std::endl;
	}

	void ExtractTests1() {
		std::set<Integer> outSet, setNames;
		setNames.emplace(111);
		setNames.emplace(222);

		print_set_ex(setNames, "  >>  setNames: ");
		print_set_ex(setNames, "  >>  outSet: ");

		std::cout << "\nmove Integer(222)..." << std::endl;
		// move John to the outSet
		auto john = setNames.extract(Integer(222));
		outSet.insert(std::move(john));

		print_set_ex(setNames, "  >>  setNames: ");
		print_set_ex(setNames, "  >>  outSet: ");

		std::cout << "\ncleanup..." << std::endl;
	}

	void Erase_Odd_Numbers()
	{
		std::set<int> numbers = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
		print_set_ex(numbers, "Before:  ");
		for (auto it = numbers.begin(); it != numbers.end(); ) {
			if (*it % 2 == 1)
				it = numbers.erase(it);
			else
				++it;
		}
		print_set_ex(numbers, "After:  ");
	}

	void Tests() {
		std::set<int> int_set{ 1,2,3,4,5,6,7,8,9 };
		print_set_ex(int_set, "Set contains:");
	}

	void Set_OF_Pairs() {
		std::set<std::pair<int, int>> pairs;

		pairs.insert({ 1,2 });
		pairs.insert({ 2,3 });
		pairs.insert({ 1,2 });

		for (const auto& entry : pairs)
			std::cout << entry.first << " " << entry.second << std::endl;
	}

	//----------------------------------------------------------------------------

	struct Widget {
		int value;
		std::string name;

		Widget(int v, std::string s) :
			value(std::move(v)), name(std::move(s)) {
		}
	};


	void Cumstom_Comparer_Type() {
		using WSet = std::set < Widget, decltype([](const Widget& w1, const Widget& w2) { 
			return w1.value < w2.value; 
		})> ;
		WSet widgets;

		widgets.emplace(1, "WidgetOne");
		widgets.emplace(5, "WidgetFive");
		widgets.emplace(3, "WidgetThree");
		widgets.emplace(4, "WidgetFour");
		widgets.emplace(2, "WidgetTwo");

		for (const Widget& w : widgets)
			std::cout << "{" << w.name << " : " << w.value << "}\n";
	}


	void Extract()
	{
		std::set<User> setNames;
		setNames.emplace("John");
		setNames.emplace("Alex");
		setNames.emplace("Bartek");

		std::set<User> outSet;
		std::cout << "move John...\n";

		// move John to the outSet
		auto handle = setNames.extract(User("John"));
		handle.value().setName("2222");
		outSet.insert(std::move(handle));

		for (auto& elem : setNames)
			std::cout << elem.getName() << '\n';

		std::cout << "cleanup...\n";
	}
}

namespace Set::TypeComparators {

	struct StringPtrLess {
		std::size_t operator()(const std::string* ps1, const std::string* ps2) const noexcept {
			return *ps1 < *ps2;
		}
	};

	struct Dereference {
		template<typename T>
		const T& operator()(const T* ptr) const {
			return *ptr;
		}
	};

	struct DereferenceLess {
		template <typename PtrType>
		bool operator()(PtrType pt1, PtrType pt2) const {
			return *pt1 < *pt2;
		}
	};

	void Test1() {
		std::set<std::string*> ssp;

		ssp.emplace(new std::string("Anteater"));
		ssp.emplace(new std::string("Wombat"));
		ssp.emplace(new std::string("Lemur"));
		ssp.emplace(new std::string("Penguin"));

		std::cout << "Wrong values:" << std::endl;
		for (std::set<std::string*>::const_iterator i = ssp.begin(); i != ssp.end(); ++i)
			std::cout << *i << std::endl;

		std::cout << "\nWrong values (2):" << std::endl;
		copy(ssp.begin(), ssp.end(), std::ostream_iterator<std::string*>(std::cout, "\n"));

		std::cout << "\nNormal values (1):" << std::endl;
		for (auto str : ssp)
			std::cout << *str << std::endl;

		std::cout << "\nNormal values (2):" << std::endl;
		transform(ssp.begin(), ssp.end(), std::ostream_iterator<std::string>(std::cout, "\n"), Dereference());
	}

	void Test2() {
		typedef std::set<std::string*, StringPtrLess> StringPtrSet;
		StringPtrSet ssp;

		ssp.emplace(new std::string("Anteater"));
		ssp.emplace(new std::string("Wombat"));
		ssp.emplace(new std::string("Lemur"));
		ssp.emplace(new std::string("Penguin"));

		for (StringPtrSet::const_iterator i = ssp.begin(); i != ssp.end(); ++i)
			std::cout << **i << std::endl;
	}

	void Test3() {
		typedef std::set<std::string*, DereferenceLess> StringPtrSet;
		StringPtrSet ssp;

		ssp.emplace(new std::string("Anteater"));
		ssp.emplace(new std::string("Wombat"));
		ssp.emplace(new std::string("Lemur"));
		ssp.emplace(new std::string("Penguin"));

		for (StringPtrSet::const_iterator i = ssp.begin(); i != ssp.end(); ++i)
			std::cout << **i << std::endl;

	}
};


void Set::TEST_ALL()
{
	// Erase_Odd_Numbers();

	// InitTest();
	// InitTest_Bad();

	// ExtractTests();
	// ExtractTests1();
	// InsertTests();

	// EmplaceTest();
	// EmplaceTest1();

	// FindTest();
	// FindTest_Class();

	// Set_OF_Pairs();

	// Cumstom_Comparer_Type();


	// TypeComparators::Test1();
	// TypeComparators::Test2();
	// TypeComparators::Test3();

	Extract();


	// Tests();
}

