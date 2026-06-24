//============================================================================
// Name        : MoveSemantics.cpp
// Created on  : 1.0
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Move semantics src (Rule of five)
//============================================================================

#include "MoveSemantics.h"
#include "PerfUtilities.hpp"
#include "../Helpers/Helpers.h"


#include <cassert>
#include <functional>
#include <optional>
#include <complex>
#include <chrono>
#include <thread>
#include <variant>
#include <filesystem>
#include <fstream>

namespace {
	using Helpers::Integer;
}

namespace MoveSemantics {

	class Object_NoMove {
	protected:
		std::string value;

	public:
		Object_NoMove() : value("") {
			std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
		}

		Object_NoMove(const std::string& val) : value(val) {
			std::cout << __FUNCTION__ << ". Value : " << this->value << std::endl;
		}

		Object_NoMove(const Object_NoMove &obj) {
			std::cout << __FUNCTION__ << " [Copy contructor]. Value : " << obj.value << std::endl;
			this->value = obj.value;
		}

		/*
		Object_NoMove(Object_NoMove&& obj) noexcept : value(std::move(obj.value)) {
			std::cout << __FUNCTION__ << " [Move constructor]." << std::endl;
		}*/
	
		virtual ~Object_NoMove() {
			std::cout << __FUNCTION__ << std::endl;
		}

		virtual std::string  getValue() const {
			return this->value;
		}

		virtual void printInfo() const {
			std::cout << "Info : Object_NoMove value = " << this->value << std::endl;
		}

		virtual void setValue(const std::string& val) {
			this->value = val;
		}

	public: /********************** Operators reload. **********************/
		friend std::ostream& operator<< (std::ostream& stream, 
									     const Object_NoMove& integer) {
			stream << integer.value;
			return stream;
		}

		friend const Object_NoMove operator+(const Object_NoMove& left,
											 const Object_NoMove& right) {
			std::cout << "operator+(Object_NoMove, Object_NoMove) called" << std::endl;
			return Object_NoMove(left.value + right.value);
		}

		friend Object_NoMove& operator+=(Object_NoMove& left,
										 const Object_NoMove& right) {
			std::cout << "operator+=(Object_NoMove, Object_NoMove) called" << std::endl;
			left.value += right.value;
			return left;
		}

		friend bool operator==(const Object_NoMove& left,
							   const Object_NoMove& right) {
			return left.value == right.value;
		}

		Object_NoMove& operator=(const Object_NoMove& right) {
			std::cout << "[copy assignment] called" << std::endl;
			if (this == &right) {
				return *this;
			}
			value = right.value;
			return *this;
		}

		/* 
		// move assignment
		Object_NoMove& operator=(const Object_NoMove&& right) noexcept 
		{
			value = right.value;
			return *this;
		}
		*/
	};


	class TestString {
	protected:
        std::string  value;

	public:
		TestString() : value("<empty>") {
			std::cout << "[Default constructor] (" << this->value << ")" << std::endl;
		}

		TestString(const std::string& val) : value(val) {
			std::cout << "[Base constructor] (" << this->value << ")" << std::endl;
		}

		TestString(const TestString &obj) {
			std::cout << "[Copy constructor] (" << this->value << ")" << std::endl;
			this->value = obj.value;
		}

		TestString(TestString&& obj) noexcept : value(std::move(obj.value)) {
			std::cout << "[Move constructor] (" << this->value << ")" << std::endl;
		}

		virtual ~TestString() {
			std::cout << "[Destructor] (" << this->value << ")" << std::endl;
		}

		virtual std::string  getValue() const {
			return this->value;
		}

		virtual void printInfo() const {
			std::cout << "Info : TestString value = " << this->value << std::endl;
		}

		virtual void setValue(const std::string& val) {
			this->value = val;
		}

		TestString& operator=(const TestString& right) {
			std::cout << "[Copy assignment operator] (" << this->value  << " -> " << right.value << ")" << std::endl;
			if (this != &right) 
				this->value = right.value;
			return *this;
		}

		TestString& operator=(TestString&& right) noexcept {
			std::cout << "[Move assignment operator] (" << this->value << " => " << right.value << ")" << std::endl;
			if (this != &right)
				this->value = std::move(right.value);
			return *this;
		}


	public: /** Operators reload. **/
		friend std::ostream& operator<< (std::ostream& stream, const TestString& integer);
		friend const TestString operator+(const TestString& left, const TestString& right);
		friend TestString& operator+=(TestString& left, const TestString& right);
		friend bool operator==(const TestString& left, const TestString& right);
	};

	std::ostream& operator<<(std::ostream& os, const TestString& integer) {
		os << integer.value;
		return os;
	}

	const TestString operator+(const TestString& left, const TestString& right) {
		std::cout << "operator+(TestString, TestString) called" << std::endl;
		return TestString(left.value + right.value);
	}

	TestString& operator+=(TestString& left, const TestString& right) {
		std::cout << "operator+=(TestString, TestString) called" << std::endl;
		left.value += right.value;
		return left;
	}

	bool operator==(const TestString& left, const TestString& right) {
		return left.value == right.value;
	}

	//////////////////////////////////////////////////////////////////////////////////////////

	class ATest {
	public:
		std::string s;
		int k;

	public:
		ATest() : s("test"), k(-1) {
			std::cout << "Base constructor. (s = " << this->s << ". k =" << this->k << ")" << std::endl;
		}

		ATest(const ATest& o) : s(o.s), k(o.k) {
			std::cout << "Copy constructor called. Move failed!" << std::endl;
		}

		~ATest() {
			std::cout << __FUNCTION__ << std::endl;
		}

		ATest(ATest&& o) noexcept : s(std::move(o.s)),
			k(std::exchange(o.k, 0)) {
			std::cout << __FUNCTION__ << ". Move constructor." << std::endl;
		}
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Object {
	public:
        std::string  text;
		int value;

	public:
		Object() : text("test"), value(3) {
			std::cout << "[Base constructor]. (s = " << this->text << ". k =" << this->value << ")" << std::endl;
		}

		Object(const std::string& t, int v) : text(t), value(v) {
			std::cout << "[Base constructor]. (s = " << this->text << ". k =" << this->value << ")" << std::endl;
		}

		virtual ~Object() {
			std::cout << "[Destructor]. (s = " << this->text << ". k =" << this->value << ")" << std::endl;
		}


		Object(const Object& obj) : text(obj.text),
			value(obj.value) {
			std::cout << "[Copy constructor.]" << std::endl;
		}

		Object(Object&& obj) noexcept : text(std::move(obj.text)),          // explicit move of a member of class type
			value(std::exchange(obj.value, 0))  // explicit move of a member of non-class type
		{
			std::cout << "[Move constructor.]" << std::endl;
		}

		Object& operator=(const Object& right) {
			std::cout << "[Copy assignment operator]" << std::endl;
			if (this == &right) {
				return *this;
			}
			this->value = right.value;
			this->text = right.text;
			return *this;
		}

		Object& operator=(const Object&& right) noexcept
		{
			std::cout << "[Move assignment operator]" << std::endl;
			this->text = std::move(right.text);
			value = right.value;
			return *this;
		}

		void PrintInfo() noexcept {
			std::cout << "[" << this->text << ", " << this->value << "]" << std::endl;
		}
	};

	class B : public Object {
	public:
		std::string s2;
		int n;
		// implicit move constructor B::(B&&)
		// calls A's move constructor
		// calls s2's move constructor
		// and makes a bitwise copy of n
	};

	class C : public B {
	public:
		~C() { } // destructor prevents implicit move constructor C::(C&&)
	};

	class D :public B {
	public:
		D() { }
		~D() { }          // destructor would prevent implicit move constructor D::(D&&)
		D(D&&) = default; // forces a move constructor anyway
	};



	template<typename Type>
	Type create(Type some_object) {
		std::cout << __FUNCTION__ << ". Before return" << std::endl;
		return some_object;
	}

	ATest create_test(ATest at) {
		return at;
	}

	Object_NoMove handleTestString_Ref(const Object_NoMove& str) {
		return str;
	}

	TestString handleTestString_Move(TestString str) {
		return str;
	}

	template<class T>
	void swap_move(T& x, T& y) {
		T tmp{ std::move(x) };
		x = std::move(y);
		y = std::move(tmp);
	}

	
	class Ex1 {
	public:
		/** member has a non-trivial but non-throwing move ctor. **/
		std::string str;
	};


	class Ex2 {
	public:
		int n;
		/** trivial and non-throwing. **/
		Ex2(Ex2&&) = default;
	};


	class NoMove {
	public:
		// prevents implicit declaration of default move constructor
		// however, the class is still move-constructible because its
		// copy constructor can bind to an rvalue argument
		NoMove(const NoMove&) {}
	};


	class Bad {
	public:
		Bad() {}
		Bad(Bad&&) { // may throw  
			std::cout << __FUNCTION__ << ". Throwing move constructor called" << std::endl;
		}
		Bad(const Bad&) { // may throw as well
			std::cout << __FUNCTION__ << ". Throwing copy constructor called" << std::endl;
		}
	};


	class Good {
	public:
		Good() {}
		Good(Good&&) noexcept { // will NOT throw
			std::cout << __FUNCTION__ <<  ". Non-throwing move constructor called" << std::endl;
		}
		Good(const Good&) noexcept { // will NOT throw 
			std::cout << __FUNCTION__ << ". Non-throwing copy constructor called" << std::endl;
		}
	};

}

namespace MoveSemantics {

	void Move_BaseTests() {
		std::cout << "------------- Test 1: ------------- " << std::endl;
		{
            std::string  str1("String_1_Value"), str2("String_2_Value");
			std::cout << str1 << "  " << str2 << std::endl;
			swap_move(str1, str2);
			std::cout << str1 << "  " << str2 << std::endl;
		}
		std::cout << "\n\n------------- Test 2: ------------- " << std::endl;
		{
			std::vector<std::string> vector;
			std::string str = "Bye";

			std::cout << "Copying str" << std::endl;
			vector.push_back(str);
			std::cout << "str: " << str << "\nvector: " << vector[0] << std::endl;

			vector.clear();

			std::cout << "\nMoving str" << std::endl;
			vector.push_back(std::move(str));
			std::cout << "str: " << str << "\nvector: " << vector[0] << std::endl;
		}
	}

	void RValue_Tests() {
		{
			Object obj1;
			Object& obj1_ref = obj1;  // lvalue
			std::cout << obj1_ref.value << std::endl;
		}
		
		{
			Object&& obj2_ref = Object();  // lvalue
			std::cout << obj2_ref.value << std::endl;
		}

		{
			std::string   s1 = "Hello ";
			std::string   s2 = "world";
			std::string&& s_rref = s1 + s2;    // the result of s1 + s2 is an rvalue
			s_rref += ", my friend";           // I can change the temporary string!
			std::cout << s_rref << '\n';       // prints "Hello world, my friend"
		}
	}

	void IsMovable()
	{	
		std::cout << std::boolalpha << "Ex1 is move-constructible? "
			<< std::is_move_constructible<Ex1>::value << '\n'
			<< "Ex1 is trivially move-constructible? "
			<< std::is_trivially_move_constructible<Ex1>::value << '\n'
			<< "Ex1 is nothrow move-constructible? "
			<< std::is_nothrow_move_constructible<Ex1>::value << '\n';

		std::cout << std::endl;

		std::cout << std::boolalpha << "Ex2 is move-constructible? "
			<< std::is_move_constructible<Ex2>::value << '\n'
			<< "Ex2 is trivially move-constructible? "
			<< std::is_trivially_move_constructible<Ex2>::value << '\n'
			<< "Ex2 is nothrow move-constructible? "
			<< std::is_nothrow_move_constructible<Ex2>::value << '\n';

		std::cout << std::endl;

		std::cout << std::boolalpha << "NoMove is move-constructible? "
			<< std::is_move_constructible<NoMove>::value << '\n'
			<< "NoMove is trivially move-constructible? "
			<< std::is_trivially_move_constructible<NoMove>::value << '\n'
			<< "NoMove is nothrow move-constructible? "
			<< std::is_nothrow_move_constructible<NoMove>::value << '\n';

		std::cout << std::endl;

		std::cout << std::boolalpha << "Object is move-constructible? "
			<< std::is_move_constructible<Object>::value << '\n'
			<< "Object is trivially move-constructible? "
			<< std::is_trivially_move_constructible<Object>::value << '\n'
			<< "Object is nothrow move-constructible? "
			<< std::is_nothrow_move_constructible<Object>::value << '\n';

		std::cout << std::endl;

		std::cout << std::boolalpha << "Object_NoMove is move-constructible? "
			<< std::is_move_constructible<Object_NoMove>::value << '\n'
			<< "Object_NoMove is trivially move-constructible? "
			<< std::is_trivially_move_constructible<Object_NoMove>::value << '\n'
			<< "Object_NoMove is nothrow move-constructible? "
			<< std::is_nothrow_move_constructible<Object_NoMove>::value << '\n';
		
	}

	void NoMoveObjectTest() {

	}

	void FillVector_NoCopy() {
		{
			std::cout << "--------------------------------TEST1" << std::endl;
			std::vector<Object> objects;
			objects.reserve(3);

			objects.push_back(Object("Value1", 1));
			objects.push_back(Object("Value2", 2));
			objects.push_back(Object("Value3", 3));
		}

		{
			std::cout << "\n\n--------------------------------TEST2:" << std::endl;

			std::vector<Object> objects;
			objects.reserve(3);

			Object obj1("Value1", 1);
			Object obj2("Value2", 2);
			Object obj3("Value3", 3);

			objects.push_back(obj1);
			objects.push_back(obj2);
			objects.push_back(obj3);
		}

		{
			std::cout << "\n\n--------------------------------TEST3:" << std::endl;

			Object obj("Value1", 1);
			std::vector<std::reference_wrapper<Object>> objects;
			objects.reserve(3);
			objects.push_back(obj);
		}

		{
			std::cout << "\n\n--------------------------------TEST4:" << std::endl;

			std::vector<Object> objects;
			objects.reserve(3);

			objects.emplace_back("Value1", 1);
			objects.emplace_back("Value2", 2);
			objects.emplace_back("Value3", 3);
		}
	}

	void FillVector_Copy() {
		std::vector<Object_NoMove> objects;
		objects.reserve(2);

		objects.push_back(Object_NoMove("Value1"));
		objects.push_back(Object_NoMove("Value2"));
	}

	void test_0()
	{
		{
			std::cout << "\n\n-------------------------- Test 0 -------------------------\n" << std::endl;
			TestString str = handleTestString_Move(TestString("Data"));
		}

		{
			std::cout << "\n\n-------------------------- Test 1 -------------------------\n" << std::endl;
			Object obj;
			obj = std::move(Object("SomeValue", 123));
		}

		{
			std::cout << "\n\n-------------------------- Test 2 -------------------------\n" << std::endl;
			Object obj2 = create(Object("SomeValue", 123));
		}


		{
			std::cout << "\n\n-------------------------- Test 2.1 -------------------------\n" << std::endl;
			Object obj2 = std::move(Object("SomeValue", 123));
		}

		{
			std::cout << "\n\n-------------------------- Test 3 -------------------------\n" << std::endl;
			Object obj1("SomeValue", 123);
			Object obj2 = create(obj1);
		}

		{
			std::cout << "\n\n-------------------------- Test 4 -------------------------\n" << std::endl;
			Object obj1("SomeValue", 123);
			Object obj2 = std::move(obj1);
		}

		{
			std::cout << "\n\n-------------------------- Test 5 -------------------------\n" << std::endl;
			Object obj1("SomeValue", 123);
			Object obj2 = create(std::move(obj1));
		}


	}

	void test()
	{
		std::cout << "Trying to move A" << std::endl;

		// return by value move-constructs the target from the function parameter
		//A a1 = create(std::move(A()));
		Object a1 = create(Object());

		std::cout << "\nBefore move, a1.s = " << std::quoted(a1.text) << " a1.k = " << a1.value << std::endl;
		Object a2 = std::move(a1); // move-constructs from xvalue
		std::cout << "\nAfter move, a1.s = " << std::quoted(a1.text) << " a1.k = " << a1.value << std::endl;

		std::cout << "\nTrying to move B" << std::endl;
		B b1;
		std::cout << "\nBefore move, b1.s = " << std::quoted(b1.text) << std::endl;
		B b2 = std::move(b1); // calls implicit move constructor
		std::cout << "\nAfter move, b1.s = " << std::quoted(b1.text) << std::endl;

		std::cout << "\nTrying to move C" << std::endl;
		C c1;
		C c2 = std::move(c1); // calls copy constructor

		std::cout << "\nTrying to move D" << std::endl;
		D d1;
		D d2 = std::move(d1);
	}

	void Move_If_Noexcept()
	{
		Good g;
		Bad b;
		Good _ = std::move_if_noexcept(g);
		Bad _ = std::move_if_noexcept(b);
	}

	//-------------------------------------------------------------------------------------------------//

	class Object_ToMove {
	private:
		std::string value;

	public:

		template<typename ... Args>
		explicit Object_ToMove(const std::string& str) : value(str) {
			std::cout << "[Object_ToMove constructor 1] (" << this->value << ")" << std::endl;
		}

		/*
		template<typename ... Args>
		Object_ToMove(Args&& ... params) : value(std::forward<Args>(params)...) {
			std::cout << "[Object_ToMove constructor 2] (" << this->value << ")" << std::endl;
		}*/


		Object_ToMove(const Object_ToMove &obj) {
			std::cout << "[Copy constructor] (" << this->value << ")" << std::endl;
			this->value = obj.value;
		}

		Object_ToMove& operator=(const Object_ToMove& right) {
			std::cout << "[Copy assignment operator] (" << this->value << " -> " << right.value << ")" << std::endl;
			if (this != &right)
				this->value = right.value;
			return *this;
		}

		Object_ToMove(Object_ToMove&& obj) noexcept : value(std::move(obj.value)) {
			std::cout << "[Move constructor] (" << this->value << ")" << std::endl;
		}

		Object_ToMove& operator=(Object_ToMove&& right) noexcept {
			std::cout << "[Move assignment operator] (" << this->value << " => " << right.value << ")" << std::endl;
			if (this != &right)
				this->value = std::move(right.value);
			return *this;
		}

		virtual ~Object_ToMove() {
			std::cout << "[Destructor] (" << this->value << ")" << std::endl;
		}

		inline virtual std::string getValue() const noexcept {
			return this->value;
		}

		virtual void printInfo() const noexcept {
			std::cout << "Object_ToMove = " << this->value << std::endl;
		}
	};

	void Move_If_Const() {
		{
			Object_ToMove obj{ "Test12345" };
			std::cout << "obj value: [" << obj.getValue() << "]" << std::endl;

			std::cout << std::endl;

			Object_ToMove obj2 = std::move(obj);
			std::cout << "obj value: [" << obj.getValue() << "]" << std::endl;
			std::cout << "obj1 value: [" << obj2.getValue() << "]" << std::endl;
		}

		std::cout << "------------------------------------------- Trying to move CONST object: --------------------------" << std::endl;

		{
			const Object_ToMove obj( std::string("Test12345"));
			std::cout << "obj value: [" << obj.getValue() << "]" << std::endl;

			std::cout << std::endl;

			Object_ToMove obj2 = std::move(obj);
			std::cout << "obj value: [" << obj.getValue() << "]" << std::endl;
			std::cout << "obj1 value: [" << obj2.getValue() << "]" << std::endl;
		}

	}

	void sink(std::string&& str) {
		// std::string buffer(std::move(str));
		// std::cout << "From sink() : "<< buffer << std::endl;
		std::cout << "From sink()      : " << str << std::endl;
	}

	void Move_String() {
		std::string text = "1234567888888888888888888888888888888888889";

		std::cout << "Oringinal string : " << text << std::endl;

		auto&& str = std::move(text);
		std::cout << "New str: " << str << "\nOld str : " << text << std::endl;

		str.append("_11");
		std::cout << "New str: " << str << "\nOld str : " << text << std::endl;
	}

	//--------------------------------------------------------------------------------------------------//

	void test1(){
		{
			ATest a = create_test(ATest());
		}
		std::cout << "\n" << std::endl;
		{
			Object a1 = create(Object());
		}
	}

	void Performance_Test_1()
	{
		/*
		{
			auto time_start = std::chrono::high_resolution_clock::now();

			TestString_NoMove testString("Test_String_1");

			for (int i = 0; i < 1000; i++) {
				for (int n = 0; n < 100; n++) {
					handleTestString_Ref(testString);
				}
			}

			auto time_end = std::chrono::high_resolution_clock::now();
			auto durtion = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start).count();
			std::cout << durtion << std::endl;
		}*/
		{
			TestString testString("Test_String_1");
			const PerfUtilities::ScopedTimer timer { "Benchmark" };
			for (int i = 0; i < 1000; i++) {
				for (int n = 0; n < 100; n++) {
					handleTestString_Move(testString);
				}
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                    Copy Assignment Operator                                                                  //
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace CopyAssignmentOperator {

// #define REFF_TEST
// #define CONST_TEST
// #define HAS_MOVE_ASSIGN_OPERATOR_1

		/** Base class definition: **/
		class Base {
		protected:
			std::string text;
#ifdef REFF_TEST
			int& ref_int;
#endif 
#ifdef CONST_TEST
			const int const_int = 1;
#endif 


		public:
#ifdef REFF_TEST
			Base(const std::string& str, int int_val) : text(str), ref_int(int_val) {
				std::cout << "Base: Constructor called. { " << text << " }" << std::endl;
			}
#endif 
			Base(const std::string& str) : text(str) {
				std::cout << "Base: Constructor called. { " << text << " }" << std::endl;
			}

			/*
			Base& operator=(Base& obj) noexcept {
				std::cout << "Base: Assignment operator called. Value = " << text << std::endl;
				this->text = obj.text;
				return *this;
			}
			*/

#ifdef HAS_MOVE_ASSIGN_OPERATOR_1
			Base& operator=(Base&& obj) noexcept {
				std::cout << "Base: Move assignment operator called. Value = " << text << std::endl;
				this->text = std::move(obj.text);
				return *this;
			}
#endif 

			virtual ~Base() = default;

			friend std::ostream& operator<<(std::ostream& stream, const Base& base) {
				stream << base.text;
				return stream;
			}
		};

		void Tester(Base obj) {
			std::cout << obj << std::endl;
		}

		////////////////////////////////////////////////////////////////

		void Check_CopyAssignmentOperator_IfClassHasVirtFuncs() {
#ifdef REFF_TEST
			Base obj1("Test1", 1), obj2("Test2", 2);
#endif 
			Base obj1("Test1"), obj2("Test2");
			obj2 = obj1;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                    Move Assignment Operator                                                                  //
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace MoveAssignmentOperator {

// #define MOVE_REFF_TEST
// #define MOVE_CONST_TEST
// #define HAS_MOVE_ASSIGN_OPERATOR_2

		/** Base class definition: **/
		class Base {
		protected:
			std::string text;
#ifdef MOVE_REFF_TEST
			int& ref_int;
#endif 
#ifdef MOVE_CONST_TEST
			const int cont_int = 5;
#endif 

		public:
#ifdef MOVE_REFF_TEST
			Base(const std::string& str, int int_val) : text(str), ref_int(int_val) {
				std::cout << "Base: Constructor called. { " << text << " }" << std::endl;
			}
#endif 

#ifdef MOVE_CONST_TEST
			Base(Base&& obj) noexcept : text(std::move(obj.text)) {
				std::cout << "Base: Constructor called. { " << text << " }" << std::endl;
			}
#endif 

			Base(const std::string& str) : text(str) {
				std::cout << "Base: Constructor called. { " << text << " }" << std::endl;
			}

			/*
			Base& operator=(Base& obj) noexcept {
				std::cout << "Base: Assignment operator called. Value = " << text << std::endl;
				this->text = obj.text;
				return *this;
			}
			*/

#ifdef HAS_MOVE_ASSIGN_OPERATOR_2
			Base& operator=(Base&& obj) noexcept {
				this->text = std::move(obj.text);
				std::cout << "Base: Move assignment operator called. Value = " << text << std::endl;
				return *this;
			}
#endif 
			virtual ~Base() = default;

			friend std::ostream& operator<<(std::ostream& stream, const Base& base) {
				stream << base.text;
				return stream;
			}
		};

		void Tester(Base obj) {
			std::cout << obj << std::endl;
		}

		////////////////////////////////////////////////////////////////

		void Check_MoveAssignmentOperator_IfClassHasVirtFuncs() {
#ifdef MOVE_REFF_TEST
			Base obj1("Test1", 1), obj2("Test2", 2);
#endif 

			Base obj1("Test1"), obj2("Test2");

			std::cout << obj1 << "  " << obj2 << std::endl;

			obj1 = std::move(obj2);

			std::cout << obj1 << "  " << obj2 << std::endl;
		}
	}
}

namespace MoveSemantics::PerfectForwarding {

	class TestClass1 {
	public:
		TestClass1() {}
		TestClass1(const TestClass1 &) {
			std::cout << "TestClass1 Copy constructor" << std::endl;
		}
		TestClass1(TestClass1 &&) noexcept {
			std::cout << "TestClass1 Move constructor" << std::endl;
		}
	};

	class TestClass2 {
	public:
		template<typename A, typename B>
		TestClass2(A && a, B && b) : a_{ std::forward<A>(a) }, b_{ std::forward<B>(b) } {
			std::cout << "TestClass2 Move constructor" << std::endl;
		}

		TestClass1 a_;
		TestClass1 b_;
	};

	struct MyStruct {
		MyStruct(int i, double d, std::string s) {
		}
	};

	template <typename Type, typename ... Args>
	Type create_new(Args&& ... args) {
		return Type(std::forward<Args>(args)...);
	}

	template<typename A, typename B>
	TestClass2 factory(A && a, B && b) {
		// retrieve the original value category from the factory call and pass on to X constructor
		return TestClass2(std::forward<A>(a), std::forward<B>(b));
	}

	template<class ... Args>
	TestClass2 factory2(Args ... params) {
		return TestClass2(std::forward<Args>(params) ...);
	}

	void AcceptVector(const std::vector<int>& v) {
		std::cout << "We have vector size " << v.size() << std::endl;
	}

	template<typename T>
	void Forward_Vector(T&& param)  {
		f(std::forward<T>(param)); // forward it to f
	}

	//----------------------------------------------------------------------------------------------//

	void Forward_Tests()
	{
		std::cout << std::endl;

		// Lvalues
		int const five = 5;
		int const myFive = create_new<int>(five);
		std::cout << "myFive: " << myFive << std::endl;

		std::string str{ "Lvalue" };
		std::string str2 = create_new<std::string>(str);
		std::cout << "str2: " << str2 << std::endl;

		// Rvalues
		int myFive2 = create_new<int>(5);
		std::cout << "myFive2: " << myFive2 << std::endl;

		std::string str3 = create_new<std::string>(std::string("Rvalue"));
		std::cout << "str3: " << str3 << std::endl;

		std::string str4 = create_new<std::string>(std::move(str3));
		std::cout << "str4: " << str4 << std::endl;

		// Arbitrary number of arguments
		double doub = create_new<double>();
		std::cout << "doub: " << doub << std::endl;

		MyStruct _ = create_new<MyStruct>(2011, 3.14, str4);
		std::cout << std::endl;
	}

	void Forward_Tests2() {
		{
			TestClass1 y;

			[[maybe_unused]]
			TestClass2 two = factory(std::move(y), TestClass1());
		}
		std::cout << "\n\nTest2:" << std::endl;
		{
			const TestClass1 y;

			[[maybe_unused]]
			TestClass2 two = factory(y, TestClass1());
		}
	}

	void Forward_Tests3() {
		{
			TestClass1 y;

			[[maybe_unused]]
			TestClass2 two = factory2(std::move(y), TestClass1());
		}
		std::cout << "\n\nTest2:" << std::endl;
		{
			const TestClass1 y;

			[[maybe_unused]]
			TestClass2 two = factory2(y, TestClass1());
		}
	}

	void Forward_FailureTest() {
		AcceptVector({1,2,3,4,5});

#if 0
		Forward_Vector({1,2,3,4,5}); // THIS FAILES AT COMPILE TIME
#endif
	}
}

namespace MoveSemantics::UniversalReferences {

	template<class T>
	void print(T&& param) {
		std::cout << param << std::endl;
	}

	void PrintTest() {
		print("sdsdsd");
	}

	////////////////////////////////////////////

	template<class T>
	void func(T&& vect) {
		vect.emplace_back(6);
		vect.emplace_back(7);
	}

	void UpdateVectorTest() {
		std::vector<Integer> vect;
		vect.reserve(10);
		for (int i : {1, 2, 3, 4, 5})
			vect.emplace_back(i);

		std::cout << std::endl;
		std::for_each(vect.begin(), vect.end(), [](const Integer& v) { std::cout << v << " "; });
		std::cout << std::endl;

		func(vect);

		std::cout << std::endl;
		std::for_each(vect.begin(), vect.end(), [](const Integer& v) { std::cout << v << " "; });
		std::cout << std::endl;
	}

	///////////////////////////////////////////////

	void Get_Int(Integer const&& integer)
	{
		std::cout << integer << std::endl;
	}

	void Pass_RValue_Test() {
		Get_Int(Integer(12));
	}
}

namespace MoveSemantics::RValue_LValue_Tests
{
	void dispatch(const int& var, std::string_view text) {
		std::cout << text << ": Lvalue" << std::endl;
	}

	void dispatch(int&& var, std::string_view text) {
		std::cout << text << ": Rvalue" << std::endl;
	}

	void RValue_LValue_Test() {
		int a = 10;
		dispatch(a, "dispatch(a)");
		dispatch(std::move(a), "dispatch(std::move(a))");
	}

	// -------------------------------------------------------------- //

	void Simple_Test()
	{
		int a = 10;
		int& lref = a; 		// Declaring lvalue reference (i.e variable a) 
		int&& rref = 20;    // Declaring rvalue reference 

		// Print the values 
		std::cout << "LValue ref: " << lref << std::endl;
		std::cout << "RValue ref: " << rref << std::endl;
	
		lref = 30;  // Value of both 'a' and 'lref' is changed 
		rref = 40;  // Value of rref is changed 

		std::cout << "LValue ref: " << lref << std::endl;
		std::cout << "RValue ref: " << rref << std::endl;

		// This line will generate an error as l-value cannot be assigned 
		// to the r-vaue referances  int &&ref = a; 
	}


	//--------------------------------------------------------------------------------------------------//

	void RValue_Range_Based_ForLoop() {
		std::vector<std::string> strings{ "one", "two", "three" };

		std::cout << "----------------------------------- First range based loop:---------------------------------- " << std::endl;
		for (const auto& str : strings)
			std::cout << "[" << str << "]  ";
		std::cout << std::endl;

		std::cout << "\n----------------------------------- Rvalue range based loop:---------------------------------- " << std::endl;
		for (auto&& str : strings)
			std::cout << "[" << str << "]  ";
		std::cout << std::endl;

		std::cout << "\n----------------------------------- Second range based loop:---------------------------------- " << std::endl;
		for (const auto& str : strings)
			std::cout << "[" << str << "]  ";
		std::cout << std::endl;
	}
}

namespace MoveSemantics::Move
{
	class StringObject {
	protected:
		std::string str;

	public:
		StringObject() : str("<empty>") {
			std::cout << "[Default constructor] (" << this->str << ")" << std::endl;
		}

		StringObject(std::string&& value) : str(std::move(value)) {
			std::cout << "[Base constructor 1] (" << this->str << ")" << std::endl;
		}

		StringObject(const std::string& value) : str(value) {
			std::cout << "[Base constructor 2] (" << this->str << ")" << std::endl;
		}

		StringObject(const StringObject &obj) {
			std::cout << "[Copy constructor] (" << this->str << ")" << std::endl;
			this->str = obj.str;
		}

		StringObject(StringObject&& obj) noexcept : str(std::move(obj.str)) {
			std::cout << "[Move constructor] (" << this->str << ")" << std::endl;
		}

		virtual ~StringObject() {
			std::cout << "[Destructor] (" << this->str << ")" << std::endl;
		}

		virtual std::string getValue() const {
			return this->str;
		}

		virtual void printInfo() const {
			std::cout << "Info : TestString value = " << this->str << std::endl;
		}

		virtual void setValue(const std::string& val) {
			this->str = val;
		}

		StringObject& operator=(const StringObject& right) {
			std::cout << "[Copy assignment operator] (" << this->str << " -> " << right.str << ")" << std::endl;
			if (this != &right)
				this->str = right.str;
			return *this;
		}

		StringObject& operator=(StringObject&& right) noexcept {
			std::cout << "[Move assignment operator] (" << this->str << " => " << right.str << ")" << std::endl;
			if (this != &right)
				this->str = std::move(right.str);
			return *this;
		}


	public: /** Operators reload. **/
		//friend std::ostream& operator<< (std::ostream& stream, const TestString& integer);
		//friend const TestString operator+(const TestString& left, const TestString& right);
		//friend TestString& operator+=(TestString& left, const TestString& right);
		//friend bool operator==(const TestString& left, const TestString& right);
	};

	//---------------------------------------------------------------------------------//

	std::vector<Integer> BuildVector_RVO() {
		std::vector<Integer> ints;
		ints.reserve(2);

		ints.emplace_back(1);
		ints.emplace_back(2);

		return ints;
	}

	std::vector<Integer> BuildVector() {
		std::vector<Integer> ints;
		ints.reserve(2);

		ints.emplace_back(1);
		ints.emplace_back(2);

		return std::move(ints); // SAME
	}

	void MoveVector() {
		std::cout << "--------------------- MOVE -----------------------:" << std::endl;
		{
			std::vector<Integer> vect = BuildVector();
			for (const auto& i : vect)
				std::cout << i.getValue() << std::endl;
		}
		std::cout << "--------------------- RVO -----------------------:" << std::endl;
		{
			std::vector<Integer> vect = BuildVector_RVO();
			for (const auto& i : vect)
				std::cout << i.getValue() << std::endl;
		}
	}

	//--------------------------------------------------------//

	class Annotation {
	private:
		std::string annotation_text;

	public:
		// Here std::move will FAIL
		explicit Annotation(const std::string& text): annotation_text(std::move(text)) {
		}
		void printInfo() {
			std::cout << annotation_text << std::endl;
		}
	};

	class AnnotationGood {
	private:
		std::string annotation_text;

	public:
		explicit AnnotationGood(std::string& text) : annotation_text(std::move(text)) {
		}
		void printInfo() {
			std::cout << annotation_text << std::endl;
		}
	};

	void Move_String_ToConstructor() {
		std::cout << "----------------------- Test1. Move fails: ------------------------" << std::endl;
		{
			std::string text = "Some_String";
			std::cout << "Text initial value: " << text << std::endl;

			Annotation a(text);
			a.printInfo();

			std::cout << "Text value after: " << text << std::endl;
		}
		std::cout << "----------------------- Test2. Move OK: ------------------------" << std::endl;
		{
			std::string text = "Some_String";
			std::cout << "Text initial value: " << text << std::endl;

			AnnotationGood a(text);
			a.printInfo();

			std::cout << "Text value after: " << text << std::endl;
		}
	}

	//--------------------------------------------------------//

	void Move_Value() {
		{
			Integer int1(11);
			Integer int2(22);

			int1 = int2; 
		}
		{
			Integer int1(11);
			Integer int2(22);

			int1 = std::move(int2);
		}
	}

}

namespace MoveSemantics::RVO {

	Integer getInteger() {
		Integer integer(1);
		return integer;
	}

	Integer getInteger(Integer integer) {
		return integer;
	}

	Integer&& getInteger_FromVector(size_t index)
    {
		std::vector<Integer> nums;
		nums.reserve(10);
		for (int i : {1, 2, 3, 4, 5})
			nums.emplace_back(i);

		//nums.er

		return std::move(nums[index]);
	}

	//-------------------------------------------------------------------//

	void GetObject_FromFunction() {
		Integer integer = getInteger();
		integer.printInfo();
	}

	void GetObject_FromFunction_Bad() {
		Integer integer = getInteger(Integer(123));
		integer.printInfo();
	}

	void GetObject_FromVector()
    {
		Integer&& integer = getInteger_FromVector(2);
		integer.printInfo();
	}
}

namespace MoveSemantics::Possible_Bugs {

	class Widget {
	private:
		int code { 0 };
		std::string str{};
		Integer* ptr { nullptr };

	public:
		Widget() {
			ptr = new Integer(1);
		}
		Widget(int i, const std::string& str, int v): code(i), str(str) {
			ptr = new Integer(v);
		}


		virtual ~Widget() {
			delete ptr;
		}

		Widget(Widget&& w): code(std::move(w.code)),
						    str(std::move(w.str)),
							ptr(std::move(w.ptr))  // <---- Check this // OR std::exchange(w.ptr, nullptr)!!!!!!!!!
		{
			std::cout << "Move contructor Winget" << std::endl;
			// Here after 'ptr(std::move(w.ptr))' we have TWO pointers to ONE memory
			// and therefore when tring to deleter BOTH of them we have CRUSH

			// w.ptr = nullptr;            // <------ THis can fix problem or use SMART_POINTERS intead Raw ptr
		}
	};

	void Move_RawPtr_In_Constructor() {
		Widget w1(1, "Text1", 111);
		Widget w2 = std::move(w1);
	}
}

namespace MoveSemantics::Exchange {

	template<class T>
	void print_vector(const T& vect, const std::string& text) {
		std::cout << text;
		for (const auto& v : vect)
			std::cout << v << " ";
		std::cout << std::endl;
	}


	void Exchange_Tests()
	{
		std::cout << "---------------------- Test1\n" << std::endl;
		{
            std::string  s1("String1"), s2("String2");
			std::cout << s1 << "   " << s2 << std::endl;
			std::cout << std::exchange(s1, s2) << std::endl;
			std::cout << s1 << "   " << s2 << std::endl;
		}
		std::cout << "\n\n---------------------- Test2\n" << std::endl;
		{
			Object obj1("Object1", 1), obj2("Object2", 2);

			obj1.PrintInfo();
			obj2.PrintInfo();

			Object obj3 = std::exchange(obj1, obj2);

			obj1.PrintInfo();
			obj2.PrintInfo();
			obj3.PrintInfo();
		}
	}

	void Exchange_Vector() {
		std::vector<int> v;

		// Since the second template parameter has a default value, it is possible
		// to use a braced-init-list as second argument. The expression below
		// is equivalent to std::exchange(v, std::vector<int>{1,2,3,4});

		auto old = std::exchange(v, { 1,2,3,4 });
		print_vector(v, "After exchange:");

		old = std::exchange(v, { 4,5,6,7});

		print_vector(old, "old:");
		print_vector(v, "After exchange:");
	}

	void Exchange_Vector2() {
		std::vector<Integer> v;

		// auto old = std::exchange(v, { std::move(Integer(1)), std::move(Integer(2)) });
		auto old = std::exchange(v, { Integer(1), Integer(2) });
	}
}

namespace MoveSemantics::MoveFailureCases {

	class S1 {
	public:
		S1() = default;

		S1(const S1& right) {
		}

		// S1(S1&& right) = delete;

		~S1() {
			std::cout << "S1::~S1()" << std::endl;
		}
	};

	void Move_Class_DTor_Only() {

		S1 obj;
		auto&& targer = std::move(obj);

		std::cout << std::boolalpha << "S1 is move-constructible? "
			<< std::is_move_constructible<S1>::value << '\n'
			<< "S1 is trivially move-constructible? "
			<< std::is_trivially_move_constructible<S1>::value << '\n'
			<< "S1 is nothrow move-constructible? "
			<< std::is_nothrow_move_constructible<S1>::value << '\n';

		std::cout << std::endl;
	}
}

namespace MoveSemantics::MoveCollections {

	class Vector : public std::vector<int> {
	public:
		Vector& operator=(const Vector& vect) = delete;
		Vector(const Vector& vect) = delete;
		Vector& operator=(Vector&& vect) noexcept = delete;
		Vector(Vector&& vect)noexcept = delete;

		Vector() {
		}
	};

	Vector getVector() {
		/*
		Vector vect;
		vect.reserve(5);
		for (int i : {1, 2, 3, 4, 5})
			vect.push_back(i);
		*/
		return Vector();
	}

	void MoveVectorTest() {
		Vector vect = getVector();
		for (int i : vect)
			std::cout << i << " ";
		std::cout << "\n";
	}
}

namespace MoveSemantics::Tests
{
	Integer getInt(int i) {
		return Integer(i);
	}

	void Lifitime_Extenstion_Test() {

		Integer&& intger = getInt(123);
		
		std::cout << "==== Test ====\n";

	}
}

namespace MoveSemantics::Move_Overload_vs_PerfectForwarding
{

    std::vector<std::string> logs {};
    std::vector<Integer> storage {};

    template<typename T> requires std::convertible_to<T, Integer>
    void store_new(T&& v)
    {
        storage.push_back(std::forward<T>(v));
    }

    void store(const Integer & str)
    {
        storage.push_back(str);
    }

    void store(Integer && str)
    {
        storage.push_back(std::move(str));
    }


    void test_overload()
    {
        {
            Integer l{1};
            store(l);
        }
        std::cout << std::endl;

        {
            store(Integer{1});
        }
        std::cout << std::endl;
    }

    void test_perfect_forwarding()
    {
        {
            Integer l{1};
            store_new(l);
        }
        std::cout << std::endl;

        {
            store_new(Integer{1});
        }
        std::cout << std::endl;
    }
}

namespace MoveSemantics::UseAfterMove_Pitfalls
{
    template <class T>
    struct MyAlloc
    {
        using value_type = T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using propagate_on_container_move_assignment = std::false_type;

        T* allocate(size_t n) {
            return static_cast<T*>(::malloc(n * sizeof(T)));
        }

        void deallocate(T* ptr, size_t n) {
            ::free(static_cast<void*>(ptr));
        }

        using is_always_equal = std::false_type;

        bool operator == (const MyAlloc&) const {
            return false;
        }
    };

    // https://github.com/Nekrolm/ubbook/blob/master/lifetime/use-after-move.md
    void MoveVector_WithAllocator()
    {
        using VectorString = std::vector<std::string, MyAlloc<std::string>>;

        {
            VectorString v{"hello", "world", "my"};
            VectorString vv = std::move(v);
            std::cout << v.size() << "\n"; // выведет 0. Это был move-конструктор
        }

        {
            VectorString v{"hello", "world", "my"};
            VectorString vv;
            vv = std::move(v);

            std::cout << v.size() << "\n";

            // выведет 3. Было move-присваивание
            for (auto &x: v) {
                // но каждый элемент был перемещен -- тут пусто
                std::cout << x;
            }
        }
    }


    void MoveString_Depending_SSO()
    {
        {
            std::string str { "123456789"};
            std::string dest { std::move(str)};

            std::cout << "capacity: " << str.capacity() << ", size: " << str.size() << " | " << str << std::endl;
            std::cout << "capacity: " << dest.capacity() << ", size: " << dest.size() << " | " << dest << "\n\n";
        }

        {
            std::string str { "123456789"};
            std::string dest = std::move(str);

            std::cout << "capacity: " << str.capacity() << ", size: " << str.size() << " | " << str << std::endl;
            std::cout << "capacity: " << dest.capacity() << ", size: " << dest.size() << " | " << dest << "\n\n";
        }

        {
            std::string str { "123456789123456789123456789123456789123456789123456789" };
            std::string dest = std::move(str);

            std::cout << "capacity: " << str.capacity() << ", size: " << str.size() << " | " << str << std::endl;
            std::cout << "capacity: " << dest.capacity() << ", size: " << dest.size() << " | " << dest << "\n\n";
        }
    }
}

namespace MoveSemantics::RValue_vs_DeclType_Auto_Return
{
    struct MyInteger
    {
        explicit MyInteger(int i) : value { i } {
            std::cout << "MyInteger(value: " << value << ")\n";
        }

        MyInteger(const MyInteger& myInteger): value { myInteger.value }
        {
            std::cout << "MyInteger(value: " << value << ") copy\n";
        }

        MyInteger(MyInteger&& myInteger) noexcept: value { std::exchange(myInteger.value, 0) }
        {
            std::cout << "MyInteger(value: " << value << ") move\n";
        }

        MyInteger& operator=(const MyInteger& myInteger) noexcept
        {
            value  = myInteger.value;
            std::cout << "MyInteger& operator=(value: " << value << ") copy\n";
            return *this;
        }

        MyInteger& operator=(MyInteger&& myInteger) noexcept
        {
            value = std::exchange(myInteger.value, 0);
            std::cout << "MyInteger& operator=(value: " << value << ") move\n";
            return *this;
        }

        ~MyInteger() {
            std::cout << "~MyInteger(value: " << value << ")\n";
        }

        int value { 0 };
    };

    decltype(auto) get_class_decltype() {
        return MyInteger(1);
    }

#if 0
    auto&& get_class_auto() {
        // INFO: shall not even compile 'error: returning reference to temporary [-Wreturn-local-addr]'
        return MyInteger(2);
    }
#endif

    void demo()
    {
        decltype(auto) a = get_class_decltype();
        std::cout << std::string(120, '=') << std::endl;


        //decltype(auto) b = get_class_auto();
        //std::cout << std::string(120, '=') << std::endl;
    }
}


void MoveSemantics::TestAll()
{
	// FillVector_NoCopy();
	// FillVector_Copy();
	// test_0();
	// test();
	// test1();
	Performance_Test_1();
	// RValue_Tests();

	// Move_BaseTests();
	// IsMovable();
	// Move_If_Noexcept();
	// Move_If_Const();

    // RValue_vs_DeclType_Auto_Return::demo();

	// MoveFailureCases::Move_Class_DTor_Only();

	// MoveCollections::MoveVectorTest();

	// Move_String();

	// PerfectForwarding::Forward_Tests();
	// PerfectForwarding::Forward_Tests2();
	// PerfectForwarding::Forward_Tests3();
	// PerfectForwarding::Forward_FailureTest();

	// Exchange::Exchange_Tests();
	// Exchange::Exchange_Vector();
	// Exchange::Exchange_Vector2();

	// Possible_Bugs::Move_RawPtr_In_Constructor();

	// CopyAssignmentOperator::Check_CopyAssignmentOperator_IfClassHasVirtFuncs();
	// MoveAssignmentOperator::Check_MoveAssignmentOperator_IfClassHasVirtFuncs();

	// UniversalReferences::Pass_RValue_Test();
	// UniversalReferences::UpdateVectorTest();

	// RValue_LValue_Tests::RValue_LValue_Test();
	// RValue_LValue_Tests::Simple_Test();
	// RValue_LValue_Tests::RValue_Range_Based_ForLoop();

	// RVO::GetObject_FromFunction();
	// RVO::GetObject_FromFunction_Bad();
	// RVO::GetObject_FromVector();

	/***************************************************************************/

	// Move::MoveVector();
	// Move::Move_Value();
	// Move::Move_String_ToConstructor();

	// Tests::Lifitime_Extenstion_Test();

    // Move_Overload_vs_PerfectForwarding::test_overload();
    // Move_Overload_vs_PerfectForwarding::test_perfect_forwarding();

    /***************************************************************************/

    // UseAfterMove_Pitfalls::MoveVector_WithAllocator();
    // UseAfterMove_Pitfalls::MoveString_Depending_SSO();
}