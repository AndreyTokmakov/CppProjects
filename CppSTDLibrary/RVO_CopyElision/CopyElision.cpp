//============================================================================
// Name        : CopyElision.cpp
// Created on  : 20.09.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : CopyElision src
//============================================================================

#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <array>
#include <format>
#include <memory>

#include "CopyElision.h"
#include "../Helpers/Helpers.h"

using Helpers::Integer;
using Helpers::Long;

namespace CopyElision
{
	struct Long_NonMove_NonCopy
    {
		long value { 0 };

		Long_NonMove_NonCopy() : Long_NonMove_NonCopy(0) {
			std::cout << "Long_NonMove_NonCopy (" << this->value << ")" << std::endl;
		}

		explicit Long_NonMove_NonCopy(int val) : value(val) {
			std::cout << "explicit Long_NonMove_NonCopy (" << this->value << ")" << std::endl;
		}

		Long_NonMove_NonCopy(const Long_NonMove_NonCopy &obj) = delete;
		Long_NonMove_NonCopy& operator=(const Long_NonMove_NonCopy& right) = delete;

		Long_NonMove_NonCopy(Long_NonMove_NonCopy &&obj) = delete;
		Long_NonMove_NonCopy& operator=(Long_NonMove_NonCopy&& integer) = delete;

		virtual ~Long_NonMove_NonCopy() {
			std::cout << "~Long_NonMove_NonCopy(" << this->value << ")" << std::endl;
		}

		void printInfo() const noexcept {
			std::cout << "Value =  " << this->value << std::endl;
		}
	};
}

namespace CopyElision::Good
{

	Integer getInteger()
    {
		Integer integer(1);
		return integer;
	}

	void GetObject_FromFunction()
    {
		Integer integer = getInteger();
		std::cout << integer.getValue() << std::endl;
	}

	//-------------------------------------------------------------------------------------//

	Long_NonMove_NonCopy _get_ConCopy_Object() {
		return Long_NonMove_NonCopy(123); // returns temporary
	}

	void GetNonCopiableObject() {
		Long_NonMove_NonCopy obj = _get_ConCopy_Object();
		obj.printInfo();
	}


	//-------------------------------------------------------------------------------------//

	Long_NonMove_NonCopy _get_object_init() {
		return Long_NonMove_NonCopy{};
	}

	void GetNonCopiableObject_AggragateInit() {
		Long_NonMove_NonCopy obj = _get_object_init();
		obj.printInfo();
	}

	//-------------------------------------------------------------------------------------//

	Long _get_ConCopy_Object_RNVO()
    {
		Long obj(3);
		return obj;
	}

	void RNVO_Test() {
		Long obj = _get_ConCopy_Object_RNVO();
        std::cout << obj.getValue() << std::endl;
	}

	//--------------------------------------------------------------------------------------//

	template<class T>
	T _get_passed_object(T obj) {
		return obj;
	}

	void Get_Passed_Object()
    {
		Long obj = _get_passed_object(Long(123));
        std::cout << obj.getValue() << std::endl;
	}


	//--------------------------------------------------------------------------------------//

	template <typename T, typename... Args>
	T create(Args&&... args) {
		return T{ std::forward<Args>(args)... };
	}

	void GetObject_CreateForward()
    {
		auto obj = create<Long>(123);
        std::cout << obj.getValue() << std::endl;
	}

	//--------------------------------------------------------------------------------------//

	Integer&& _getInteger_FromVector(size_t index) {
		std::vector<Integer> nums;
		nums.reserve(10);
		for (int i : {1, 2, 3, 4, 5})
			nums.emplace_back(i);

		return std::move(nums[index]);
	}

	void GetObject_FromVector_MOVE()
    {
		Integer&& integer = _getInteger_FromVector(2);
        std::cout << integer.getValue() << std::endl;
	}

	//----------------------------------------------------------------------------------------//

	std::unique_ptr< Long_NonMove_NonCopy> get_unique_ptr(int i ) {
		std::unique_ptr< Long_NonMove_NonCopy> ptr = std::make_unique< Long_NonMove_NonCopy>(i);
		return ptr;
	}

	void Test_UniquePtr() {
		std::unique_ptr< Long_NonMove_NonCopy> ptr = get_unique_ptr(2);
		std::cout << "Ptr created" << std::endl;
	}
};

namespace CopyElision::RNVO_Failure_Cases {

	class Long {
	protected:
		long value;

	public:
		Long() : value(0) {
			std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
		}

		explicit Long(int val) : value(val) {
			std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
		}

		/*
		Long(const Long& obj) = delete;
		Long(Long&& obj) = delete;
		Long& operator=(const Long& right) noexcept = delete;
		Long& operator=(Long&& integer) noexcept = delete;
		*/

		Long(const Long& obj) : value(obj.value) {
			std::cout << __FUNCTION__ << " [Copy contructor]. (" << obj.value << ")" << std::endl;
		}

		Long(Long&& obj) : value(std::exchange(obj.value, 0)) {
			std::cout << __FUNCTION__ << " [Move contructor]. (" << obj.value << ")" << std::endl;
		}

		Long& operator=(const Long& right) {
			std::cout << "Long [Copy assignment operator]" << std::endl;
			if (this == &right) {
				return *this;
			}
			value = right.value;
			return *this;
		}

		Long& operator=(Long&& integer) noexcept {
			std::cout << "Long [Move assignment operator]" << std::endl;
			this->value = std::exchange(integer.value, 0);
			return *this;
		}


		virtual ~Long() {
			std::cout << __FUNCTION__ << "(" << this->value << ")" << std::endl;
		}

		void printInfo() const noexcept {
			std::cout << "Value =  " << this->value << std::endl;
		}
	};


	class LongEx : public Long {
	public:
		LongEx() : Long(0) {
			std::cout << "LongEx (" << this->value << ")" << std::endl;
		}

		explicit LongEx(int val) : Long(val) {
			std::cout << "explicit LongEx (" << this->value << ")" << std::endl;
		}

		virtual ~LongEx() {
			std::cout << "~Long_NonMove_NonCopy(" << this->value << ")" << std::endl;
		}

		/*
		LongEx(const LongEx& obj) = delete;
		LongEx(LongEx&& obj) = delete;
		LongEx& operator=(const LongEx& right) noexcept = delete;
		LongEx& operator=(LongEx&& integer) noexcept = delete;
		*/
		
	};

	LongEx func_1() {
		LongEx obj(54321);
		return obj;
	}

	LongEx func_2() {
		return LongEx{ 123 };
	}

	Long func_3() {
		Long obj(54321);
		return obj;
	}

	Long func_4() {
		LongEx obj(54321);
		return obj;
	}

	void Failure_Inheritancee() {
		std::cout <<   "----------------- Test1---------------\n";
		{
			LongEx obj = func_1();
		}

		std::cout << "\n----------------- Test2---------------\n";
		{
			LongEx obj = func_2();
		}

		std::cout << "\n----------------- Test3---------------\n";
		{
			Long obj = func_3();
		}

		std::cout << "\n----------------- Test4---------------\n";
		{
			Long obj = func_4();
		}
	}
}

namespace CopyElision::BAD {

	Integer _getInteger(Integer integer) {
		return integer;
	}

	void GetObject_FromFunction() {
		Integer integer = _getInteger(Integer(123));
        std::cout << integer.getValue() << std::endl;
	}

	//-------------------------------------------------------//

	Long _get_ConCopy_Object_RNVO()
    {
		Long obj;
		// Discard RNVO by using std::move()
		return std::move(obj);
	}

	void RNVO_Test()
    {
		Long obj = _get_ConCopy_Object_RNVO();
        std::cout << obj.getValue() << std::endl;
	}

	//-------------------------------------------------------//

	template<class T>
	T _get_passed_object(T obj) {
		return obj;
	}

	void Get_Passed_Object() {
		Long obj = _get_passed_object(Long(123));
        std::cout << obj.getValue() << std::endl;
	}

	void Get_Passed_Object_1() {
		Long l(123);
		Long obj = _get_passed_object(l);
        std::cout << obj.getValue() << std::endl;
	}

	//-----------------------------------------------------//

	Integer _getInteger_FromVector(size_t index) {
		std::vector<Integer> nums;
		nums.reserve(10);
		for (int i : {1, 2, 3, 4, 5})
			nums.emplace_back(i);

		return std::move(nums[index]);
	}

	void GetObject_FromVector() {
		Integer&& integer = _getInteger_FromVector(2);
        std::cout << integer.getValue() << std::endl;
	}
}

namespace CopyElision::CustomTests {

	class StringObject {
	protected:
		std::string str;

	public:
		StringObject() : str("<empty>") {
			std::cout << "[Default constructor] (" << this->str << ")" << std::endl;
		}

		StringObject(std::string&& value) noexcept : str(std::move(value)) {
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

		virtual void Append(std::string&& val) {
			this->str.append(std::move(val));
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

	//--------------------------------------------------------------//

	StringObject create() {
		return StringObject("123456789");
	}

	StringObject create1() {
		StringObject a("123456789");
		return a;
	}

	StringObject create2() {
		StringObject a("123456789");

		a.setValue("New_val");
		a.Append("__TTTTTTTTTTTTT");

		return a;
	}

	
	
	StringObject create3() {
		/*static constexpr std::array< StringObject, 3> data {
			StringObject("One"),
			StringObject("Two"),
			StringObject("Three") 
		};*/	

		using namespace std::literals::string_view_literals;
		static constexpr std::array< std::string_view, 3> data{
			"One"sv,
			"Two"sv,
			"Three"sv
		};

		//return data[1];
	
		return StringObject("123456789");
	}

	/**************************************************************/

	void RVO_Simple() {
		StringObject str = create();
		str.printInfo();
	}

	void NRVO_Simple() {
		StringObject str = create1();
		str.printInfo();
	}

	void NRVO_Simple_Modif() {
		StringObject str = create2();
		str.printInfo();
	}

	void NRVO_Simple_GetFromVector() {
		StringObject str = create3();
		str.printInfo();
	}
}

namespace CopyElision::NotDeclaredMoveConstructor
{
    class MyClass
    {
    public:
        std::string name {};

        explicit MyClass(std::string n): name { std::move(n) }  {
            std::cout << std::format("Destroying MyClass::MyClass({})", name) << std::endl;
        }

        ~MyClass()  {
            std::cout << std::format("Destroying MyClass::~MyClass({})", name) << std::endl;
        }

        // Not copyable and ot assignable.
        MyClass(const MyClass&) = delete;
        MyClass& operator=(const MyClass&) = delete;

        // Movable only for NRVO purposes (and RVO in C++11).
        // Never implemented.

        MyClass(MyClass&& rhs);

        /*
        MyClass(MyClass&& rhs): name { std::move(rhs.name) } {
            std::cout << std::format("Move constructor: MyClass::~MyClass(MyClass&&) ({})", name) << std::endl;
        }

        MyClass& operator=(MyClass&& rhs) noexcept {
            std::cout << std::format("Move assignment: MyClass::~MyClass(MyClass&&)") << std::endl;
            name = std::move(rhs.name);
            return *this;
        }
        */
    };

    MyClass test1()
    {
        return MyClass("RNV Test");
    }

    // NOTE: This will COMPILE and LINK since we have 'MyClass(MyClass&& rhs);'
    //       even without IMPLEMENTATION
    MyClass test2()
    {
        MyClass c {"NRVO Test"};
        return c;
    }

    constexpr bool someSondition() {
        return true;
    }

    // NOTE: This will not LINK since 'MyClass(MyClass&& rhs);' NOT IMPLEMENTED
    /*
    MyClass test3()
    {
        MyClass c("C"), d("D");
        if (someSondition()) {
            return c;
        } else {
            return d;
        }
    }
    */

    void testAll()
    {
        {
            MyClass obj = test1();
        }

        {
            MyClass obj = test2();
        }

        /*
        {
            MyClass obj = test3();
        }
        */
    }
}

namespace CopyElision::BasicExamples
{
    Integer create_RVO(int v)
    {
        return {v};
    }

    Integer create_RVO_2(int v)
    {
        return create_RVO(v);
    }

    Integer create_NRVO(int v)
    {
        Integer i {v};
        return i;
    }

    Integer create_NRVO_2(int v)
    {
        Integer i = Integer{v};
        return i;
    }

    Integer create_NRVO_3(int v)
    {
        Integer i = create_RVO_2(v);
        return i;
    }

    void BasicTest()
    {
        std::cout << "-------------- RVO ---------------\n";
        {
            auto v = create_RVO(1);
        }
        std::cout << "-------------- RVO 2 ---------------\n";
        {
            auto v = create_RVO_2(2);
        }
        std::cout << "-------------- RNVO ---------------\n";
        {
            auto v = create_NRVO(3);
        }
        std::cout << "-------------- RNVO 2 ---------------\n";
        {
            auto v = create_NRVO_2(4);
        }
        std::cout << "-------------- RNVO 3 ---------------\n";
        {
            auto v = create_NRVO_3(5);
        }
    }
}

void CopyElision::TestAll()
{

    BasicExamples::BasicTest();

	// GetObject_FromFunction();
	// Good::GetObject_FromVector_MOVE();
	// Good::GetNonCopiableObject();
	// Good::GetNonCopiableObject_AggragateInit();
	// Good::RNVO_Test();
	// Good::Get_Passed_Object();
	// Good::GetObject_CreateForward();
	// Good::Test_UniquePtr();

	// RNVO_Failure_Cases::Failure_Inheritancee();

	// BAD::GetObject_FromFunction();
	// BAD::GetObject_FromVector();
	// BAD::RNVO_Test();                   // Discard RNVO by using std::move()
	// BAD::Get_Passed_Object();           // Only Def CTOR and MOVE CTOR called 
	// BAD::Get_Passed_Object_1();         // Default, Copy and Move constructors are called.

	// CustomTests::RVO_Simple();
	// CustomTests::NRVO_Simple();
	// CustomTests::NRVO_Simple_Modif();
	// CustomTests::NRVO_Simple_GetFromVector();

    // NotDeclaredMoveConstructor::testAll();
};

