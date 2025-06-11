/**============================================================================
Name        : Exceptions.cpp
Created on  : 07.06.2020
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Exceptions
============================================================================**/

#include "Exceptions.h"
#include "../Helpers/Wrapper.h"

#include <iostream>
#include <string>
#include <exception>


namespace Exceptions::Exceptions_In_Constructors {

	class Base_WithException {
	public :
		Base_WithException() {
			std::cout << "Base_WithException::Base_WithException()" << std::endl;
			throw std::runtime_error("Exception from Base_WithException::Base_WithException()");
		}
		virtual ~Base_WithException() /*noexcept(false)*/ {
			std::cout << "~Base_WithException::Base_WithException()" << std::endl;
		}
	};
	
	class Base {
	public:
		Base() {
			std::cout << "Base::Base()" << std::endl;
		}
		virtual ~Base() /*noexcept(false)*/ {
			std::cout << "Base::~Base()" << std::endl;
		}
	};

	class Base2 {
	public:
		Base2(): Base2("Test") {
			std::cout << "Base2::Base2()" << std::endl;
			throw std::runtime_error("Exception from Base_WithException::Base_WithException()");
		}
		Base2(const std::string& s) {
			std::cout << "Base2::Base2(const std::string& s)" << std::endl;
		}
		virtual ~Base2() noexcept(false) {
			std::cout << "Base2::~Base2()" << std::endl; 
		}
	};

	class Derived: public Base {
	public:
		Derived() {
			std::cout << "Derived::Derived()" << std::endl;
			throw std::runtime_error("Exception from Derived::Derived()");
		}
		virtual ~Derived() /*noexcept(false)*/ {
			std::cout << "Derived::~Derived()" << std::endl;
		}
	};


	class A {
	public:
		A() { std::cout << "A::A()" << std::endl; }
		virtual ~A() { std::cout << "A::~A()" << std::endl; }
	};

	class B {
	public:
		B() { 
			std::cout << "B::B()" << std::endl;
			throw std::runtime_error("Exception from B::B()");
		}
		virtual ~B() { std::cout << "B::~B()" << std::endl; }
	};

	class C{
	public:
		C() { std::cout << "C::C()" << std::endl; }
		virtual ~C() { std::cout << "C::~C()" << std::endl; }
	};


	class TestClass {
	private:
		A a;
		B b;
		C c;

	public:
		TestClass() {
			std::cout << "TestClass::TestClass()" << std::endl;
		}
		virtual ~TestClass() /*noexcept(false)*/ {
			std::cout << "TestClass::~TestClass()" << std::endl;
		}
	};

	class TestClass_Handle_Exceptions {
	private:
		A a;
		B b;
		C c;

	public:
		TestClass_Handle_Exceptions() try {
			std::cout << "TestClass_Handle_Exceptions::TestClass_Handle_Exceptions()" << std::endl;
		} catch (const std::exception& exc) {
			std::cout << "EXCEPTION BEFORE CTOR:  " << exc.what() << std::endl;
		}
	
		virtual ~TestClass_Handle_Exceptions() /*noexcept(false)*/ {
			std::cout << "TestClass_Handle_Exceptions::~TestClass_Handle_Exceptions()" << std::endl;
		}
	};

	///////////////////////////////////////////////////////////////////////////////

	class BadObject {
	public:
		BadObject(int v) {
			std::cout << "BadObject::BadObject()" << std::endl;
			if (v < 0)
				throw std::runtime_error("Exception from BadObject::BadObject()");
		}
		virtual ~BadObject() { 
			std::cout << "BadObject::~BadObject()" << std::endl; 
		}
	};

	class ExcHandlerClass : public BadObject {
	public:
		ExcHandlerClass(int v) try: BadObject(v) {
			std::cout << "ExcHandlerClass::ExcHandlerClass() constructor" << std::endl;
		}
		catch (const std::exception& exc) {
			std::cout << "EXCEPTION BEFORE CTOR:  " << exc.what() << std::endl;
		}


		virtual ~ExcHandlerClass() {
			std::cout << "ExcHandlerClass::~ExcHandlerClass()" << std::endl;
		}
	};


	///////////////////////////////////////////////////////////////////////////////

	void Test_Base() {
		try {
			Base_WithException b;
		}
		catch (const std::exception& exc) {
			std::cout << "EXCEPTION:  " << exc.what() << std::endl;
		}
	}

	void Test_Base_TwoConstructors() {
		try {
			Base2 b;
		}
		catch (const std::exception& exc) {
			std::cout << "EXCEPTION:  " << exc.what() << std::endl;
		}
	}
	

	void Test_DerivedClass() {
		try {
			Derived b;
		}
		catch (const std::exception& exc) {
			std::cout << "EXCEPTION:  " << exc.what() << std::endl;
		}
	}

	void Test_Exception_Composition() {
		try {
			TestClass obj;
		}
		catch (const std::exception& exc) {
			std::cout << "EXCEPTION:  " << exc.what() << std::endl;
		}
	}

	void Test_Exception_Composition_Handling() {
		try {
			TestClass_Handle_Exceptions obj;
		}
		catch (const std::exception& exc) {
			std::cout << "EXCEPTION:  " << exc.what() << std::endl;
		}
	}

	void Test_Exception_InitList() {
		try {
			ExcHandlerClass obj(1);
		}
		catch (const std::exception& exc) {
			std::cout << "EXCEPTION:  " << exc.what() << std::endl;
		}

		try {
			ExcHandlerClass obj(-1);
		}
		catch (const std::exception& exc) {
			std::cout << "EXCEPTION:  " << exc.what() << std::endl;
		}
	}
};


namespace Exceptions::Exceptions_In_Destructor
{
	struct Base_WithDtorException
    {
		Base_WithDtorException() {
			std::cout << "Base_WithException::Base_WithException()" << std::endl;
			
		}

        ~Base_WithDtorException() noexcept(false) {
			std::cout << "~Base_WithException::Base_WithException()" << std::endl;
			throw std::runtime_error("Exception from Base_WithException::Base_WithException()");
		}
	};

    struct BadClass
    {
        bool throwException {false};

        explicit BadClass(bool bad): throwException {bad} {
            std::cout << "BadClass created\n";
        }

        ~BadClass() {
            throw std::runtime_error("it's actually impossible to catch .. without noexcept(false)");
            std::cout << "BadClass created\n";
        }
    };

	void Test1()
    {
		try {
			Base_WithDtorException b;
		}
		catch (const std::exception& exc) {
			std::cout << "EXCEPTION:  " << exc.what() << std::endl;
		}
	}


    void Test_MemLeak()
    {
        Helpers::Integer integer {12345};

        try {
            BadClass obj {true};
            /** **/
        } catch (const std::exception& exc) {
            std::cout << "Error: " << exc.what() << std::endl;
        } catch (...) {
            std::cout << "Unknown error" << std::endl;
        }

        // якобы теперь ОК
        // Helpers::~Integer() will never be called
    }
}

namespace Exceptions::ExceptionsTypes {

	void throw_timed_out_exception() {
		throw std::system_error(std::make_error_code(std::errc::timed_out));
	}

	void TimeoutException() {
		try {
			throw_timed_out_exception();
		} catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}
}

namespace Exceptions::Crush {

	class Bad {
	public:
		Bad() {}
		virtual ~Bad() noexcept(false) {
			std::cout << __FUNCTION__ << std::endl;
			throw_exception();
		}

		void throw_exception() {
			throw 42;
		}
	};

	void Text_Exceptio_With_Crush() {
		try {
			Bad b;
			throw 32;
		}
		catch (int err) {
			std::cout << __FUNCTION__ << ", Err = " << err << std::endl;
			std::cout << err;
		}
	}
}


namespace Exceptions::CustomExceptions {

	class MyExceptionBase : public std::exception {
	public:
		MyExceptionBase(char const* const _Message) noexcept : std::exception() {
			std::cout << "MyExceptionBase(" << _Message << ") created.\n";
		}

		~MyExceptionBase() override {
			std::cout << "~MyExceptionBase(" << this->what() << ") destroyed.\n";
		}
	};

	class MyExceptionDerived : public MyExceptionBase {
	public:
		MyExceptionDerived(char const* const _Message) noexcept : MyExceptionBase(_Message) {
			std::cout << "MyExceptionDerived(" << _Message << ") created.\n";
		}

		~MyExceptionDerived() override {
			std::cout << "~MyExceptionDerived(" << this->what() << ") destroyed.\n";
		}
	};

	//====================================================================================

	class MyExceptionBaseEx : public std::exception {
	public:
        // FIXME: std::exception() do not have constructor(char* )
		MyExceptionBaseEx(char const* const _Message) noexcept : std::exception() {
			std::cout << "MyExceptionBaseEx(" << _Message << ") created.\n";
		}

		~MyExceptionBaseEx() override {
			std::cout << "~MyExceptionBaseEx(" << this->what() << ") destroyed.\n";
		}

		virtual void raise() {
			throw* this;
		}
	};

	class MyExceptionDerivedEx : public MyExceptionBaseEx {
	public:
		MyExceptionDerivedEx(char const* const _Message) noexcept : MyExceptionBaseEx(_Message) {
			std::cout << "MyExceptionDerivedEx(" << _Message << ") created.\n";
		}

		~MyExceptionDerivedEx() override {
			std::cout << "~MyExceptionDerivedEx(" << this->what() << ") destroyed.\n";
		}

		virtual void raise() override {
			throw* this;
		}
	};

	//====================================================================================

	void foo(MyExceptionBase& e) {
		throw e;
	}

	void foo_fix(MyExceptionBaseEx& e) {
		e.raise();
	}

	//====================================================================================

	void BadInheritance_Test1() {
		MyExceptionDerived e("MyExceptionDerived");
		auto task = []() {
			throw MyExceptionDerived("MyExceptionDerived");
		};

		try {
			foo(e);
		}
		catch (const MyExceptionDerived& exc) {
			std::cout << "1    " <<  exc.what() << std::endl;
		}
		catch (const std::exception& exc) {
			std::cout << "2    " << exc.what() << std::endl;
		}
	}


	void Good_Workaround_Test() {
		MyExceptionDerivedEx e("MyExceptionDerived");

		try {
			foo_fix(e);
		}
		catch (const MyExceptionDerivedEx& exc) {
			std::cout << "1    " << exc.what() << std::endl;
		}
		catch (const std::exception& exc) {
			std::cout << "2    " << exc.what() << std::endl;
		}
	}


	void BadInheritance_Leak() {
		auto task = []() {
			throw new MyExceptionDerived("MyExceptionDerived");
		};

		try {
			task();
		}
		catch (const MyExceptionDerived* exc) {
			std::cout << "1.   " << exc->what() << std::endl;
			// HANDLE 
			delete exc; // DELETE OR LEAK
		}
		catch (const std::exception* exc) {
			std::cout << "2.   " << exc->what() << std::endl;
			// HANDLE 
			delete exc; // DELETE OR LEAK
		}
	}
}


namespace Exceptions::Tests {

	class MyExceptionBase: public std::exception {
	public:
		explicit MyExceptionBase(const char* const msg) noexcept : std::exception() {
		}
	};

	class MyExceptionDerived : public MyExceptionBase {
	public:
		explicit MyExceptionDerived(const char* const msg) noexcept : MyExceptionBase(msg) {
		}
	};

	void f(MyExceptionBase& e) {
		throw e;
	}

	void _TESTS_() {
		MyExceptionDerived e("MyExceptionDerived");

		try {
			f(e);
		} catch (MyExceptionDerived& e) {
			std::cout << "1" << std::endl;
			std::cout << e.what() << std::endl;
		} catch (...) {
			std::cout << "2" << std::endl;
			std::cout << e.what() << std::endl;
		}
	}
}

namespace Exceptions::CurrentException {

	void handle_eptr(std::exception_ptr eptr) {
		try {
			if (eptr) {
				std::rethrow_exception(eptr);
			}
		}
		catch (const std::exception& e) {
			std::cout << "Caught exception \"" << e.what() << "\"\n";
		}
	}


	// As the result the next line will be printed:
	// Caught exception "invalid string position"
	void GetCurrentException()
	{
		std::exception_ptr eptr;
		try {
			auto result = std::string().at(1); // this generates a std::out_of_range

		}
		catch (...) {
			eptr = std::current_exception(); // capture
		}
		handle_eptr(eptr);
	}
}

namespace Exceptions::Exceptions_In_Destructor
{
    struct Boom: public std::runtime_error
    {
        explicit Boom(const std::string& text): std::runtime_error (text ) {
        }
    };

    struct Bang: public std::runtime_error
    {
        explicit Bang(const std::string& text): std::runtime_error (text ) {
        }
    };

    struct Evil final
    {
        Evil() {
            std::cout << "Evil::Evil()\n";
        }

        ~Evil() noexcept(false) {
            std::cout << "Evil::~Evil()\n";
            throw Bang { "Oks"};
        }
    };

    void bar()
    {
        throw Boom("Boom from bar()");
    }

    void foo()
    {
        std::cout << "A\n";
        Evil e;
        std::cout << "B\n";
        bar();
        std::cout << "C\n";
    }

    void bad_usage()
    {
        try {
            foo();
        } catch (const Boom&) {
            std::cerr << "Handling Boom...\n";
        } catch (const Bang&) {
            std::cerr << "Bang...\n";
        }

        // A
        // Evil::Evil()
        // B
        // Evil::~Evil()
        // terminate called after throwing an instance of 'Throwing_From_Destructor::Bang'
        //    what():  Oks
    }
}


void Exceptions::TestAll()
{
	// Exceptions_In_Constructors::Test_Base();
	// Exceptions_In_Constructors::Test_Base_TwoConstructors();

	// Exceptions_In_Constructors::Test_DerivedClass();
	// Exceptions_In_Constructors::Test_Exception_Composition();
	// Exceptions_In_Constructors::Test_Exception_Composition_Handling();
	// Exceptions_In_Constructors::Test_Exception_InitList();


	Exceptions_In_Destructor::Test1();
	Exceptions_In_Destructor::Test_MemLeak();
	Exceptions_In_Destructor::bad_usage();


	// ExceptionsTypes::TimeoutException();

	// Crush::Text_Exceptio_With_Crush();

	// CustomExceptions::BadInheritance_Test1();
	// CustomExceptions::BadInheritance_Leak();
	// CustomExceptions::Good_Workaround_Test();

	// Tests::_TESTS_();

	// CurrentException::GetCurrentException();
};