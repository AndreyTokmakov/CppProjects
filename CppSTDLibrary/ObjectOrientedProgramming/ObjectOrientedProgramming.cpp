//============================================================================
// Name        : ObjectOrientedProgramming.hcpp
// Created on  : 01.54.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : TupleTests testing 
//============================================================================

#include "ObjectOrientedProgramming.h"

#include <cassert>
#include <iostream>
#include <string>
#include <any>
#include <functional>
#include "../Integer/Integer.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                       Constructor Invoke Order                                                               //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::Constructors {

	class Base {
	protected:
		std::string name;

	public:
		Base(std::string_view name) : name(name) {
			std::cout << "Base::Base(). [Name: " << this->name << "]" << std::endl;
		}

		virtual ~Base() {
			std::cout << "Base::~Base(). [Name: " << this->name << "]" << std::endl;
		}

		virtual void printInfo() const noexcept {
			std::cout << "INFO from Base: Name = " << name << std::endl;
		}

		virtual void printInfo() noexcept {
			std::cout << "Base class info. [name: " << name << "]" << std::endl;
		}
	};


	class Derived : public Base {
	private:
		std::string desc;

	public:
		Derived() : Derived("EmptyDerived", "No description") {
			std::cout << "Derived::Derived() 1. [Name: " << this->name << ", Description: " << this->desc << "]" << std::endl;
		}

		Derived(std::string_view name) : Derived(name, "No description") {
			std::cout << "Derived::Derived() 2. [Name: " << this->name << ", Description: " << this->desc << "]" << std::endl;
		}

		Derived(std::string_view name, std::string_view description) : Base(name), desc(description) {
			std::cout << "Derived::Derived() 3. [Name: " << this->name << ", Description: " << this->desc << "]" << std::endl;
		}

		~Derived() override {
			std::cout << "Derived::~Derived(). [Name: " << this->name << ", Description: " << this->desc << "]" << std::endl;
		}

		void printInfo() noexcept override {
			Base::printInfo();
			std::cout << "Derived class info. [name: " << name << ", desc: " << desc << "]" << std::endl;
		}
	};

	class DerivedEx1 : public Base {
	private:
		std::string desc;

	public:
		// NO BASE CLASS CONSTRUCTORS
		virtual ~DerivedEx1() override {
			std::cout << "DerivedEx1::~DerivedEx1(). [Name: " << this->name << ", Description: " << this->desc << "]" << std::endl;
		}
	};

	class DerivedEx2 : public Base {
	private:
		std::string desc;

	public:
		using Base::Base;

		virtual ~DerivedEx2() override {
			std::cout << "DerivedEx2::~DerivedEx2(). [Name: " << this->name << ", Description: " << this->desc << "]" << std::endl;
		}
	};

	//-------------------------------------------------------------------------------------//

	void CallOneConstructorFromAnother() {
		Derived d;
	}

	void CallBaseClassConstructor() {
		std::cout << "Test1:" << std::endl;
		{
			Base* obj = new Derived();
			delete obj;
		}

		std::cout << "\nTest2:" << std::endl;
		{
			std::shared_ptr<Base> obj = std::make_shared<Derived>("TestNam1", "DESC2");
		}

		std::cout << "\nTest3:" << std::endl;
		{
			const auto func = [](const std::shared_ptr<Base> obj)-> void { obj->printInfo();  };
			std::shared_ptr<Base> obj = std::make_shared<Derived>("TestNam1", "DESC2");
			func(obj);
		}
	}

	void Call_Base_Class_Method() {
		std::make_unique<Derived>()->printInfo();
		// (new Derived())->printInfo();
	}


	void InheritedConstructors() {
		
		// If BaseClasscConstructor not implemented
#if 0
		Base* b = new DerivedEx1("Some_input");
#endif

		Base* b = new DerivedEx2("Some_input");


	}
}


namespace ObjectOrientedProgramming {

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Object {
	private:
		std::string text;
		unsigned int code;

	public:
		Object(const std::string& t, unsigned int c) : text(t), code(c) {
		}
		/** Calling constructor Object(str, int)**/
		Object() : Object("Empty", 0) {
		}

	public:
		void print() {
			std::cout << "Text: " << this->text << ". Code = " << this->code << std::endl;
		}
	};


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	class NoHeapObject {
	private:
		int id;
		std::string name;

	public:
		NoHeapObject(int userId, const std::string& userName) : id(userId), name(userName) {
			std::cout << __FUNCTION__ << std::endl;
		}

		~NoHeapObject() {
			std::cout << __FUNCTION__ << std::endl;
		}

		// Delete the new function to prevent object creation on heap
		//void * operator new (size_t) = delete;
	};



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	namespace PureVirtualException {

		class Base {
		public:
			Base() {
				std::cout << "Base::Base()" << std::endl;
				CallVirtualFunc();
			}

		public:
			void CallVirtualFunc() {
				std::cout << "TestObject::CallVirtualFunc()" << std::endl;
				PureVirutalFunction();
			}

			virtual void PureVirutalFunction() noexcept = 0;
		};

		class Derived : public Base {
		public:
			virtual void PureVirutalFunction() noexcept override {
				std::cout << "TestObjectObject::PureVirutalFunction()" << std::endl;
			}
		};


		class Base1 {
		public:
			Base1() {
				std::cout << "Base::Base()" << std::endl;
			}
			~Base1() {
				std::cout << "Base::~Base()" << std::endl;
				//CallVirtualFunc();
			}

		public:
			void CallVirtualFunc() {
				std::cout << "TestObject::CallVirtualFunc()" << std::endl;
				virtual_function();
			}

			virtual void virtual_function() noexcept = 0;
		};

		class Derived1 : public Base1 {
		public:
			Derived1() {
				std::cout << "Derived1::Derived1()" << std::endl;
			}
			~Derived1() {
				std::cout << "Derived1::~Derived1()" << std::endl;
			}

		public:
			void virtual_function() noexcept override {
				std::cout << "TestObjectObject::PureVirutalFunction()" << std::endl;
			}
		};


		void Test() {
			std::shared_ptr<Base> obj = std::make_shared<Derived>();
			//std::cout << std::endl;
			//obj->CallVirtualFunc();

			/*
			try {
				Derived b;
			}
			catch (const std::exception& exc) {
				std::cout << exc.what() << std::endl;
			}
			*/
		}

		void Test1() {
			std::shared_ptr<Derived1> obj = std::make_shared<Derived1>();
		}

	}
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace TestUseVirtualFuncsDestructor {

		class Base {
		public:
			virtual void method() { 
				std::cout << "Base::method()" << std::endl; 
			}
			virtual ~Base() {
				method(); 
			}
		public:
			void baseMethod() { 
				method(); 
			}
		};

		class Derived: public Base {
		public:
			void method() {
				std::cout << "Derived::method()" << std::endl;
			}
			~Derived() {
				method(); 
			}
		};

		/////////// BAD implementation down - no virtual dtor

		class BaseBad {
		public:
			virtual void method() {
				std::cout << "BaseBad::method()" << std::endl;
			}
			~BaseBad() {
				method();
			}
		public:
			void baseMethod() {
				method();
			}
		};

		class DerivedBad : public BaseBad {
		public:
			void method() {
				std::cout << "DerivedBad::method()" << std::endl;
			}
			~DerivedBad() {
				method();
			}
		};

		void Test() {
			std::cout << "\nTest1" << std::endl; {
				std::shared_ptr<Base> obj = std::make_shared<Derived>();
			}

			std::cout << "\nTest2" << std::endl; {
				Base* ptr = new Derived();
				delete ptr;
			}

			std::cout << "\nTest3" << std::endl; {
				std::shared_ptr<BaseBad> obj = std::make_shared<DerivedBad>();
			}

			std::cout << "\nTest4" << std::endl; {
				BaseBad* ptr = new DerivedBad();
				delete ptr;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace TestUseVirtualFuncsConstructor {

		class Base {
		public:
			Base() { 
				std::cout << "Base::Base()" << std::endl;
				virtual_method();
			}
	
			virtual void virtual_method() {
				std::cout << "Base::virtual_method()"<< std::endl;
			}
		};

		class Derived : public Base {
		public:
			void virtual_method() override {
				std::cout << "Derived::virtual_method()" << std::endl;
			}
			Derived() {
				std::cout << "Derived::Derived()" << std::endl;
				virtual_method();
			}
		};

		void Test() {
			std::shared_ptr<Base> obj = std::make_shared<Derived>();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                       Constructor Invoke Order                                                               //
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace Constructor_Invoke_Order {

		class Base {
		public:
			Base() {
				std::cout << "Base::Base()" << std::endl;
				info();
			}

			virtual ~Base() {
				info();
				std::cout << "Base::~Base()" << std::endl;
			}

		public:
			virtual void info() const noexcept {
				std::cout << "INFO from <Base>" << std::endl;
			}
		};

		class Derived : public Base {
		public:
			Derived() {
				std::cout << "Derived::Derived()" << std::endl;
			}

			virtual ~Derived() {
				std::cout << "Derived::~Derived()" << std::endl;
			}

		public:
			virtual void info() const noexcept override {
				std::cout << "INFO from <Derived>" << std::endl;
			}
		};

		void FunctionsInvokeOrder() {
			Base* a = new Derived();
			a->info();
			delete a;
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		class BaseForOverloadWithoutVituals {
		public:
			BaseForOverloadWithoutVituals() {
				std::cout << "BaseForOverloadWithoutVituals::BaseForOverloadWithoutVituals()" << std::endl;
			}

		public:
			void info() const noexcept {
				std::cout << "BASE:   BaseForOverloadWithoutVituals::info()" << std::endl;
			}

			virtual void info_virtual() const noexcept {
				std::cout << "BASE:   BaseForOverloadWithoutVituals::info_virtual()" << std::endl;
			}
		};

		class DerivedForOverloadWithoutVituals : public BaseForOverloadWithoutVituals {
		public:
			DerivedForOverloadWithoutVituals() {
				std::cout << "DerivedForOverloadWithoutVituals::DerivedForOverloadWithoutVituals()" << std::endl;
			}

		public:
			virtual void info() const noexcept {
				std::cout << "DERIVED:   DerivedForOverloadWithoutVituals::info()" << std::endl;
			}

			virtual void info_virtual() const noexcept override {
				std::cout << "DERIVED:   DerivedForOverloadWithoutVituals::info_virtual()" << std::endl;
			}
		};

		void OverloadWithoutVituals_Test() {
			BaseForOverloadWithoutVituals* obj = new DerivedForOverloadWithoutVituals();
			obj->info();
			obj->info_virtual();
		}


		////////////////////////////////////////  VIRTUAL CLASSES TESTS     //////////////////////////////////////////////////////////////

		class VirtBase {
		public:
			VirtBase() { std::cout << "VirtBase::VirtBase()" << std::endl; }
			virtual ~VirtBase() { std::cout << "VirtBase::~VirtBase()" << std::endl; }
		};

		class VirtBase2 {
		public:
			VirtBase2() { std::cout << "VirtBase2::VirtBase2()" << std::endl; }
			virtual ~VirtBase2() { std::cout << "VirtBase2::~VirtBase2()" << std::endl; }
		};

		class VirtA : public virtual VirtBase {
		public:
			VirtA() { std::cout << "VirtA::VirtA()" << std::endl; }
			virtual ~VirtA() { std::cout << "VirtA::~VirtA()" << std::endl; }
		};

		class VirtB : public virtual VirtBase {
		public:
			VirtB() { std::cout << "VirtB::VirtB()" << std::endl; }
			virtual ~VirtB() { std::cout << "VirtB::~VirtB()" << std::endl; }
		};


		class VirtDerived1 : public VirtA, public VirtB {
		public:
			VirtDerived1() { std::cout << "VirtDerived1::VirtDerived1()" << std::endl; }
			virtual ~VirtDerived1() { std::cout << "VirtDerived1::~VirtDerived1()" << std::endl; }
		};

		class VirtDerived2 : public VirtBase, public VirtBase2 {
		public:
			VirtDerived2() { std::cout << "VirtDerived2::VirtDerived2()" << std::endl; }
			virtual ~VirtDerived2() { std::cout << "VirtDerived2::~VirtDerived2()" << std::endl; }
		};

		class VirtDerived3 : public VirtBase, virtual VirtBase2 {
		public:
			VirtDerived3() { std::cout << "VirtDerived3::VirtDerived3()" << std::endl; }
			virtual ~VirtDerived3() { std::cout << "VirtDerived3::~VirtDerived3()" << std::endl; }
		};

		void VirtClassTests() {
			{
				VirtDerived1 v;
			}

			std::cout << std::endl;
			{
				VirtDerived2 v;
			}

			std::cout << std::endl;
			{
				VirtDerived3 v;
			}
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void GenerateExcepiton() {
		throw std::exception();
	}

	class ConstructorWithExceptionBad {
	private:
		Integer* integer;

	public:
		ConstructorWithExceptionBad() {

			std::cout << __FUNCTION__ << std::endl;
			this->integer = new Integer(123);
			GenerateExcepiton();

		}
		~ConstructorWithExceptionBad() {
			std::cout << __FUNCTION__ << std::endl;
			delete this->integer;
		}
	};


	class ConstructorWithExceptionGood {
	private:
		std::unique_ptr<Integer> integer;

	public:
		ConstructorWithExceptionGood() {
			std::cout << __FUNCTION__ << std::endl;
			this->integer = std::make_unique<Integer>(123);
			GenerateExcepiton();
		}
		~ConstructorWithExceptionGood() {
			std::cout << __FUNCTION__ << std::endl;
		}
	};


	void ExceptionInConstructor() {
		/*
		try {
			ConstructorWithExceptionBad obj;
		} catch (const std::exception &exc) {
			std::cout << "Exception message: " << exc.what() << std::endl;
		}
		*/
		try {
			ConstructorWithExceptionGood obj;
		}
		catch (const std::exception &exc) {
			std::cout << "Exception message: " << exc.what() << std::endl;
		}
	}

	void No_Heap_Object() {
		NoHeapObject* obj = new NoHeapObject(1, "Test");
		delete obj;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace ConstructorInitializationLists {

		class SomeClassBad {
		private:
			Integer value;

		public:
			SomeClassBad(int v) {
				value = v;
			}
		};

		class SomeClassGood {
		private:
			Integer value;

		public:
			SomeClassGood(int v): value{v} {
			}
		};


		class SomeClassGood1 {
		private:
			Integer value;

		public:
			SomeClassGood1(int v) : value(v) {
			}
		};

		void InitList_Construction() {
			std::cout << "------------------------------------- BAD -------------------------------\n";
			{
				SomeClassBad i(22);
			}
			std::cout << "------------------------------------- GOOD1 -------------------------------\n";
			{
				SomeClassGood i(22);
			}
			std::cout << "------------------------------------- GOOD2 -------------------------------\n";
			{
				SomeClassGood1 i(22);
			}
		}

		//----------------------------------------------------------------------------------//

		class Base {
		private:
			int value;
			std::string name;

		public:
			Base(): Base("", -1) {
			}
			Base(const std::string& n, int v) : name(n), value(v) {
			}
			virtual ~Base() = default;
		};


		class Derived : public Base {
		private:
			const int const_value = 1;
			unsigned long& ref_value;

		public:
			Derived(const std::string& n, int v, unsigned long r) : Base(n, v),        /* We have to use 'initialization list' to init Base class values */
																	const_value(123),  /* We have to use 'initialization list' to update CONST value     */
																	ref_value(r)       /* We have to use 'initialization list' to init Refference value  */
			{
				std::cout << "const_value = " << this->const_value << std::endl;
			}
		};

		void Test() {
			Derived obj("Name", 333, 123456789);

		}

		//----------------------------------------------------------------------------------//

		class Vars {
		private:
			std::shared_ptr<Integer> a;
			std::shared_ptr<Integer> b;
			std::shared_ptr<Integer> c;

			inline static int i = 0;

		public:
			Vars() : c(std::make_shared<Integer>(++i)), b(std::make_shared<Integer>(++i)), a(std::make_shared<Integer>(++i))
			{
			}

			Vars(int x) : c(std::make_shared<Integer>(i++)), b(std::make_shared<Integer>(i++)), a(std::make_shared<Integer>(i++))
			{
			}

			void info() {
				std::cout << "a = " << a->getValue() << std::endl;
				std::cout << "b = " << b->getValue() << std::endl;
				std::cout << "c = " << c->getValue() << std::endl;
			}
		};

		void Call_Members_InitializationOrder() {
			Vars v;
			//Vars v(1);

			v.info();
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace Explicit {

		class Long {
		private:
			long value;

		public:
			Long(long l) : value(l) {	}
			void info() { std::cout << value << std::endl; }
		};

		class LongEx {
		private:
			long value;

		public:
			explicit LongEx(long l) : value(l) { }
			void info() { std::cout << value << std::endl; }
		};

		class Base {
		public:
			Base(int i) {  // converting constructor
				std::cout << "Base::Base(" << i << ")" << std::endl;
			}      
			Base(int a, int b) { // converting constructor (C++11)
				std::cout << "Base::Base(" << a << ", " << b  << ")" << std::endl;
			}
			operator bool() const { 
				std::cout << "Base::operator bool()" << std::endl;
				return true; 
			}
		};

		class ExplicitBase {
		public:
			explicit ExplicitBase(int i) {  // converting constructor
				std::cout << "ExplicitBase::ExplicitBase(" << i << ")" << std::endl;
			}
			explicit ExplicitBase(int a, int b) { // converting constructor (C++11)
				std::cout << "ExplicitBase::ExplicitBase(" << a << ", " << b << ")" << std::endl;
			}
			explicit operator bool() const {
				std::cout << "ExplicitBase::operator bool()" << std::endl;
				return true;
			}
		};

		void ConstructorTests() {
			long long_v = 123;
			Long l = long_v;
			l.info();

#if 0		// ERROR HERE
			int intv = 123;
			LongEx l2 = intv;
			l2.info();
#endif
		}

		void CTor_Operator_Bool_1() {
			Base b1(1);
			Base b2 = 2;
			Base b3 { 3, 4 };  
			Base b4 = { 5, 6 };
			Base b5 = (Base)7;

			if (b1) {  /* OK: A::operator bool(). */}

			bool na1 = b1; // OK: copy-initialization selects A::operator bool()
			bool na2 = static_cast<bool>(b1); // OK: static_cast performs direct-initialization
		}

		void CTor_Operator_Bool_2() {
			ExplicitBase b1(1);
			// ExplicitBase b2 = 2;             // ERROR
			ExplicitBase b3{ 3, 4 };
			// ExplicitBase b4 = { 5, 6 };      // ERROR
			ExplicitBase b5 = (ExplicitBase)7;

			if (b1) {  /* OK: A::operator bool(). */ }

			// bool na1 = b1;                   // ERROR   
			bool na2 = static_cast<bool>(b1); // OK: static_cast performs direct-initialization
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	namespace UserDefinedConversion {

		class X {
		public:
			// Implicit conversion to 'int'
			explicit operator int() const {
				std::cout << "Converting X --> int" << std::endl;
				return 7;
			}

			// Implicit conversion to 'int'
			explicit operator std::string() const {
				std::cout << "Converting X --> std::string" << std::endl;
				return "7_Str";
			}

			explicit operator double() const noexcept {
				std::cout << "Converting X --> double" << std::endl;
				return 7.7;
			}
		};

		class IntClass {
		private:
			int num;

		public:
			IntClass(int a) : num(a) { }

			// conversion from User-defined type to Basic type 
			operator int() const {
				return num;
			}
		};

		void show(int x) {
			std::cout << x << std::endl;
		}

		/////////////////////

		void Test1() {
			X x;

			int n = static_cast<int>(x);
			std::cout << n << std::endl;

			std::string str = static_cast<std::string>(x);
			std::cout << str << std::endl;

			double d = static_cast<double>(x);
			std::cout << d << std::endl;

		}

		void Test_AdHoc_Polymorthism()
		{
			IntClass i = 100;
			show(746); // outputs 746 
			show(i); // outputs 100 

			int v = i;
			std::cout << v << std::endl;
		}

	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace MultipleInheritance {

		class Base {
		protected:
			std::string name;

		public:
			Base(const std::string& n) : name(n) {
			}
			// virtual ~Base() = default;

			virtual ~Base() {
				std::cout << "Base::~Base()."<< std::endl;
			}

			virtual void Info() {
				std::cout << "Base::Info(). Name: " << this->name << std::endl;
			}
		};

		class DerivedOne : virtual public Base {
		public:
			DerivedOne() : Base("DerivedOneInfo") {
			}

			~DerivedOne() {
				std::cout << "DerivedOne::~DerivedOne()." << std::endl;
			}

			virtual void Info() override  {
				std::cout << "DerivedOne::Info(). Name: " << this->name << std::endl;
			}
		};

		class DerivedTwo: virtual public Base {
		public:
			DerivedTwo() : Base("DerivedTwoInfo") {
			}

			~DerivedTwo() {
				std::cout << "DerivedTwo::~DerivedTwo()." << std::endl;
			}

			virtual void Info() override  {
				std::cout << "DerivedTwo::Info(). Name: " << this->name << std::endl;
			}
		};


		
		class Worker:  public DerivedOne, public DerivedTwo {
		public:
			Worker(const std::string& n) : Base(n) {
			}

			~Worker() {
				std::cout << "Worker::~Worker()." << std::endl;
			}

			virtual void Info() override  {
				std::cout << "DerivedTwo::Info(DerivedOne). Name: " << DerivedOne::name << std::endl;
				std::cout << "DerivedTwo::Info(DerivedTwo). Name: " << DerivedTwo::name << std::endl;
				std::cout << "DerivedTwo::Info(). Name: " << this->name << std::endl;
			}
		};

		void TEST() {

			Base* b = new Worker("WorkerInfo");
			b->Info();

			delete b;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                    Operator Overload Tests                                                                   //
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace Operator_Overload_Tests {

		/** Base class definition: **/
		class Base {
		protected:
			std::string text;

		public:
			virtual ~Base() = default;

		public:
			Base(const std::string& str) : text(str) {
				std::cout << "Base: Constructor called. { " << text << " }" << std::endl;
			}

			Base& operator=(Base& obj) noexcept {
				std::cout << "Base: Assignment operator called. Value = " << text << std::endl;
				this->text = obj.text;
				return *this;
			}

			friend std::ostream& operator<<(std::ostream& stream, const Base& base) {
				stream << base.text;
				return stream;
			}
		};

		/** Base class definition: **/
		class Derived: public Base {
		public:
			std::string description;

		public:
			Derived(const std::string& str, const std::string& desc) : Base(str), description(desc) {
				std::cout << "Derived: Constructor called. { " << text  << ", " << description << " }" << std::endl;
			}

			/*
			Derived& operator=(Derived& obj) noexcept {
				std::cout << "Derived: Assignment operator called. Value = " << text << std::endl;
				this->description = obj.description;
				return *this;
			}*/

			friend std::ostream& operator<<(std::ostream& stream, const Derived& obj) {
				stream << "{ " << obj.text << ", " << obj .description << " }";
				return stream;
			}
		};

		////////////////////////////

		void Inherit_Copy_Assignment_Operator() {

			/*  // Constructor src:
			{
				Base obj1("Value1");
				std::cout << obj1 << std::endl;

				Base obj2 = std::string("Value2");
				std::cout << obj2 << std::endl;
			}
			*/

			{
				Base obj1("Value1"), obj2("Value2");
				std::cout << std::endl;

				std::cout << "obj1 = " << obj1 << ", obj2 = " << obj2 << std::endl;
				obj1 = obj2;
				std::cout << "obj1 = " << obj1 << ", obj2 = " << obj2 << std::endl;
			}

			std::cout << "\n ----------------- Test2 ----------------------" << std::endl << std::endl;
			{
				Derived obj1("Value111", "Desc111"), obj2("Value2222", "Desc2222");

				std::cout << std::endl;

				std::cout << "obj1 = " << obj1 << ", obj2 = " << obj2 << std::endl;
				obj1 = obj2;
				std::cout << "obj1 = " << obj1 << ", obj2 = " << obj2 << std::endl;
			}
		}
	}

	namespace OperatorOverloading {

		class Long {
			long value{ 0 };

		public:
			Long() = default;

			explicit Long(long v) : value{ v } {
				std::cout << "Long::Long(" << value << ")\n";
			}

			~Long() {
				std::cout << "Long::~Long(" << value << ")\n";
			}

			Long(const Long& l) = delete;
			Long& operator=(const Long& l) = delete;

			Long(Long&& l) noexcept = delete;
			Long& operator=(Long&& l) noexcept = delete;

			friend Long operator+(const Long& l1, const Long& l2);


			friend std::ostream& operator<<(std::ostream& stream, const Long& obj);
		};

		std::ostream& operator<<(std::ostream& stream, const Long& obj) {
			stream << obj.value;
			return stream;
		}

		Long operator+(const Long& l1, const Long& l2) {
			return Long(l1.value + l2.value);
		}

		//------------------------------------------------------------------

		void Tests() {
			Long v1{ 11 };
			std::cout << v1 << std::endl;

			Long v2{ 22 };
			auto x = (v1 + v2);
			std::cout << x << std::endl;

			// Long v3 = std::move(v2);
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                                                                                                              //
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
	namespace Final_Tests {

#define OVERRIDE_TEST_1

		class Base {
		public:
			virtual void Foo(short value) {
				std::cout << "Base:Foo()" << std::endl;
			}
		};

		class Derived : public Base {
		public:
#if OVERRIDE_TEST_1
			// We should have the ERROR here
			virtual void Foo(int value) override {
				std::cout << "Derived::Foo()" << std::endl;
			}
#endif

		};
	}
*/

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//                                    Static_Members_Inheritance                                                                //
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	namespace Static_Members_Inheritance {
		
		class Base {
		protected:
			static const std::string name;

			static inline std::string ClassName() noexcept   {
				return "BaseClass";
			}

		public:
			Base() {
			}
			virtual ~Base() = default;

			virtual void Info() {
				std::cout << "Base. " << name << std::endl;
				std::cout << "Base. " << ClassName() << std::endl;
			}
		};

		const std::string Base::name = "BaseName";
		
		/** Base class definition: **/
		class Derived : public Base {
		protected:
			static const std::string name;

			static inline std::string ClassName() noexcept {
				return "DerivedClass";
			}

		public:
			Derived() {
			}
			virtual ~Derived() = default;

			virtual void Info() {
				std::cout << "Derived. " << name << std::endl;
				std::cout << "Derived. " << ClassName() << std::endl;
				std::cout << "Derived. " <<Base::ClassName() << std::endl;
			}
		};

		const std::string Derived::name = "DerivedName";

		void Test() {
			Base objBase;
			objBase.Info();

			Derived objDerived;
			objDerived.Info();
		}
	}
}

namespace ObjectOrientedProgramming::AccessModifiersTests {

	class Base {
	public:
		int value;
	protected:
		std::string name;
	};


	class PrivateDerived: private Base {
	public:
		void ShowName() { 
			std::cout << this->name << std::endl;
			// std::cout << Base::value << std::endl;
		}
	};
	class ProtectedeDerived : protected Base { };
	class DefaultDerived : Base { };


	class DerivedEx : 
	//	public PrivateDerived {
		public ProtectedeDerived {
	//	public DefaultDerived {

	public:
		void Info() {
			std::cout << this->value << std::endl;
		}

	};



	///////////////////////////////////////////////////////////////////

	class PrivateBase {
	private:
		int value = 0;
		friend class FriendOfPrivateBase;

	public:
		PrivateBase(): value(0) {
			std::cout << "Base::Base()" << std::endl;
		}

		PrivateBase(int v) : value(v) {
			std::cout << "Base::Base()" << std::endl;
		}

		void Info() const {
			std::cout << "Base::Info(). Value: " << this->value << std::endl;
		}

		friend void PrintInfo(const PrivateBase& base);

		// T
		friend class Derived_Friend;
	};

	class Derived : public PrivateBase {
	public:
		void Info() {
			// ERROR: Can access PrivateBase::value
			// std::cout << "Base::Info(). Value: " << this->value << std::endl;
		}
	};

	class Derived_Friend : public PrivateBase {
	public:
		Derived_Friend(int v) : PrivateBase(v) {
			std::cout << "Derived_Friend::Derived_Friend()" << std::endl;
		}

		void Info() {
			// ERROR: Can access PrivateBase::value
			 std::cout << "Base::Info(). Value: " << this->value << std::endl;
		}
	};


	void PrintInfo(const PrivateBase& base) {
		base.Info();
	}

	class FriendOfPrivateBase {
	public:
		void Info(const PrivateBase& base)
		{
			base.Info();
		}
	};

	void Private_FriendClassTest() {
		PrivateBase base(123);
		FriendOfPrivateBase F;
		F.Info(base);
	}

	void Private_FriendFunction() {
		PrivateBase base(123);
		PrintInfo(base);
	}

	void Private_Derived_FriendClass() {
		Derived_Friend obj(123);
		obj.Info();
	}
};

////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::AccessMembers {

	template<typename Type>
	class Base {
	protected:
		std::vector<Type> vector;

	public:
		inline size_t size() const noexcept {
			return vector.size();
		}
	};

	template<typename Type>
	class Derived : public Base<Type> {
	private:

		/*
		using Base<Type>::vector;

		inline size_t size1() const noexcept {
			return this->vector.size();
		}
		*/
	};

	/////////////////////////////////////////////////

	void Test() {
		Derived<int> d;
		std::cout << d.size() << std::endl;
	}
}

////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::AggregateInitialization {

	class Base {
	public:
		int a;
		int b;

		//Base() {
		//}

	};


	class Derived: public Base {
	public:
		int c;
	};


	void InitTest() {
		Derived obj{ {1,2},3 }; // Failes if Base::a and Base::b a NOT public

		std::cout << obj.a << "  " << obj.b << "  " << obj.c << std::endl;
	}
}


////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::Private_Ctor_Dtor {

	class B {
	private:
		std::string name;
		B() {}

	public:
		B(B const&) = delete;
		B(B&&) noexcept = delete;
		B& operator=(B const&) = delete;
		B& operator=(B&&) noexcept = delete;

	public:
		static std::shared_ptr<B> create() {
			struct make_shared_enabler : public B {};
			return std::make_shared<make_shared_enabler>();
		}

		inline const std::string getName() const noexcept {
			return this->name;
		}

		inline void setName(const std::string& name) noexcept {
			this->name = name;
		}
	};

	void DirtyHackTest() {
		std::shared_ptr<B> obj1 = B::create();
		obj1->setName("Test1");

		std::cout << obj1->getName() << std::endl;
		std::cout << typeid(*(obj1)).name()<< std::endl;
	}

	//////////////////////////////

	class Base {
	protected:
		Base() // = delete;
		{}
	};

	class Derived : public Base {
	public:
		Derived() {
		}
	};

	void Test() {
		Derived d;
	}
}

////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::ParametricPolymorphism {
	template <class T>
	T max(T a, T b) {
		return a > b ? a : b;
	}

	void Test() {
		std::cout << max(9, 5) << std::endl;     // 9

		std::string foo("foo"), bar("bar");
		std::cout << std::max(foo, bar) << std::endl; // "foo"
	}
}


////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::Final_Tests {

	class BaseForDisableOverride {
	public:
		virtual void printInfo() const noexcept final {
			std::cout << "BaseForDisableOverride::printInfo()" << std::endl;
		}
	};

	class DerivedForDisableOverride : public BaseForDisableOverride {
	public:

#if 0   // We will get the ERROR here
		virtual void printInfo() const noexcept override {
			std::cout << "BaseForDisableOverride::printInfo()" << std::endl;
		}
#endif
	};





	class Base final {
	};


#if 0   
	// We will get the ERROR here
	class Derived: public Base {
	};
#endif
}

////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::Overloaded_Tests {

	class Base {
	public:
		virtual void info() {
			std::cout << "Base::info() called" << std::endl;
		}
		virtual void info(int i) {
			std::cout << "Base::info(int i) called" << std::endl;
		}
	};

	class Derived : public Base {
	public:
		virtual void info(const std::string& text) {
			std::cout << "Derived::info(const std::string& text) called" << std::endl;
		}
	};

	class Derived2 : public Base {
	public:
		using Base::info; // --> This allow as to to call Base::info() of the Derived2 class object

		virtual void info(const std::string& text) {
			std::cout << "Derived2::info(const std::string& text) called" << std::endl;
		}
	};

	void Test() {

		std::cout << "--------------------Test1---------------\n" << std::endl;

		Base b;
		b.info();
		b.info(1);

		std::cout << "\n--------------------Test2---------------\n" << std::endl;

		Derived d;
//		d.info();      // Error
//		d.info(1);     // Error
		d.info("TETET");

		std::cout << "\n--------------------Test3---------------\n" << std::endl;

		Base* b1 = new Derived();
		b1->info();
		b1->info(1);

		dynamic_cast<Derived*>(b1)->info(""); // Calls Derived::Info();

		dynamic_cast<Derived*>(b1)->Base::info(); // Still call Base:f()

		//assert(nullptr != d1);
	}

	void Test_AccessHidenMethods() {
		Derived2 d;
		d.info();      // Error
		d.info(1);     // Error
		d.info("TETET");
	}

	/////////////////////////////////////////


	class Base1 {
	public:
		void f(double x)  { // Doesn't matter whether or not this is virtual
			std::cout << x << std::endl;
		}
	};

	class Derived1 : public Base1 {
	public:
		void f(char d) {  // Doesn't matter whether or not this is virtual
			std::cout << d << std::endl;
		}
	};

	void HideBaseClassMethod_Test() {

		Derived1* d = new Derived1();
		Base1* b = d;
		b->f(65.3);  // Okay: passes 65.3 to f(double x)
		d->f(65.3);  // Bizarre: converts 65.3 to a char ('A' if ASCII) and passes it to f(char c); does NOT call f(double x)!!
		delete d;
	}

	///////////////////////////////

	class B {
	public:
		void f(int i) { 
			std::cout << "f(int): " << i + 1 << std::endl;
		}
	};

	class D_Bad : public B {
	public:
		void f(double d) {
			std::cout << "f(double): " << d + 1.3 << std::endl;
		}
	};

	class D_Good : public B {
	public:
		using B::f; //!!!!!!!!!!!

		void f(double d) {
			std::cout << "f(double): " << d + 1.3 << std::endl;
		}
	};

	void Overload_Test___BadUsage()
	{
		D_Bad* pd = new D_Bad;
		pd->f(2);
		pd->f(2.3);
	}

	void Overload_Test___GoodUsage()
	{
		D_Good* pd = new D_Good;
		pd->f(2);
		pd->f(2.3);
	}

	//---------------------------------------------------------------------

	class BaseStatic {
	public:
		static void staticInfo() {
			std::cout << "BaseStatic::staticInfo()" << std::endl;
		}
	};

	class DerivedStatic : public BaseStatic {
	public:
		static void staticInfo() {
			std::cout << "DerivedStatic::staticInfo()" << std::endl;
		}
	};



	void Overload_StaticMethod() {
		std::cout << "---------------------------- Test1 ---------------------------------\n";
		{
			BaseStatic::staticInfo();
			DerivedStatic::staticInfo();
		}

		std::cout << "---------------------------- Test2 ---------------------------------\n";
		{
			BaseStatic* b = new DerivedStatic();
			b->staticInfo();
		}
		
		std::cout << "---------------------------- Test3 ---------------------------------\n";
		{
			BaseStatic().staticInfo();
			DerivedStatic().staticInfo();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::Override_Test {

	class Base {
	public:
		virtual void info_bad() const noexcept {
			std::cout << "Base::info_bad()\n";
		}

		virtual void info_good() const noexcept {
			std::cout << "Base::info_good()\n";
		}
	};

	class Derived: public Base {
	public:
		virtual void info_bad() noexcept {
			std::cout << "Derived::info_bad()\n";
		}

		virtual void info_good() const noexcept override {
			std::cout << "Derived::info_good()\n";
		}
	};

	void Override_WithOut_Const() {
		{
			std::unique_ptr<Base> obj{ std::make_unique<Derived>() };

			obj->info_bad(); // WARNING: Base::info_bad() insted of Derived::info_bad()
			obj->info_good();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::Classes_Structs_Sizeof_Tests {

	class EmptyClass {
	};

	class Foo {
		int i;
		char a;
	};

	class ClassWithStaticVariable {
		int i;
		static int stat;
	};

	int ClassWithStaticVariable::stat = 0;

#pragma pack(push, 1)
	struct FooAligned
	{
		int i;
		char a;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	class Base_WithCharPrt {
	private:
		int value;
		char* prt;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	class Base_WithFunc {
	private:
		int value;
	public:
		void Func() { std::cout << __FUNCTION__ << std::endl; }
	};
#pragma pack(pop)

#pragma pack(push, 1)
	class Base_WithVirtFunc {
	private:
		int value;
	public:
		virtual void Func() { std::cout << __FUNCTION__ << std::endl; }
	};
#pragma pack(pop)

	class Base_WithVirtFuncOnly {
	public:
		virtual void Func() { std::cout << __FUNCTION__ << std::endl; }
	};

#pragma pack(push, 1)
	class Base_WithVirtFunc2 {
	private:
		int value;
	public:
		virtual void Func1() { std::cout << __FUNCTION__ << std::endl; }
		virtual void Func2() { std::cout << __FUNCTION__ << std::endl; }
		virtual void Func3() { std::cout << __FUNCTION__ << std::endl; }
		virtual void Func4() { std::cout << __FUNCTION__ << std::endl; }
	};
#pragma pack(pop)

#pragma pack(push, 1)
	class DerivedClass : public FooAligned {
	private:
		int value;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	class DerivedClass2 : public Base_WithCharPrt {
	private:
		int value;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	class DerivedClassVirt : virtual public FooAligned {
	private:
		int value;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	class DerivedClassWithVirtFuncs : public FooAligned {
	private:
		int value;
	public:
		virtual void Func1() { std::cout << __FUNCTION__ << std::endl; }
		virtual void Func2() { std::cout << __FUNCTION__ << std::endl; }
		virtual void Func3() { std::cout << __FUNCTION__ << std::endl; }
		virtual void Func4() { std::cout << __FUNCTION__ << std::endl; }
	};
#pragma pack(pop)


#pragma pack(push, 1)
	class ClassWithVirtDestructor {
	private:
		int value;

		virtual ~ClassWithVirtDestructor() {}
	};
#pragma pack(pop)

#pragma pack(push, 1)
	class Class1 {
	private:
		int value1;
		int value2;
		int value3;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	class DerivedClassWitVirtInhereance : virtual public Class1 {
	private:
		int value4;
	};
#pragma pack(pop)

#pragma pack(push, 1)
	class DerivedClassWitVirtInhereanceAndFunc : virtual public Class1 {
	private:
		int value;
		virtual ~DerivedClassWitVirtInhereanceAndFunc() {}
	};
#pragma pack(pop)

	class Object {
	public:
		std::string public_value;

	protected:
		std::string protected_value;

	private:
		std::string private_value;

	public:
		Object() {
			//public_value = "PublicVariable";
			//protected_value = "ProtectedVariable";
			//private_value = "PrivateVariable";
		}
	};

	class A {
	};

	class B {
	};

	class C : virtual public A, virtual public B {
	};

    namespace Virtual_FuncDiffClass
    {
        struct A {
            virtual void a() = 0;
        };

        struct B {
            virtual void b() = 0;
        };

        struct C {
            virtual void b() = 0;
        };

        struct D {
            virtual void d1() = 0;
            virtual void d2() = 0;
            virtual void d3() = 0;
        };

        struct Clazz1 : A { /** **/ };
        struct Clazz2 : A, B { /** **/ };
        struct Clazz3 : A, B, C { /** **/ };

        struct Clazz4 : D { /** **/ };
        struct Clazz5 : A, D { /** **/ };

        void VirtualMethodTests()
        {
            std::cout << "--------------------- Depending of number VTBL ---------------------------------\n";
            std::cout << sizeof(Clazz1) << std::endl;
            std::cout << sizeof(Clazz2) << std::endl;
            std::cout << sizeof(Clazz3) << std::endl << std::endl;
            std::cout << sizeof(Clazz4) << std::endl;
            std::cout << sizeof(Clazz5) << std::endl;
        }

    }


	void StructSizeTest() {
		std::cout << "Size string: " << sizeof(std::string) << std::endl;


		std::cout << "Size Object: " << sizeof(Object) << std::endl;
		std::cout << "Size EmptyClass: " << sizeof(EmptyClass) << std::endl;
		std::cout << "Size Foo: " << sizeof(Foo) << std::endl;
		std::cout << "Size ClassWithStaticVariable: " << sizeof(ClassWithStaticVariable) << std::endl;
		std::cout << "Size FooAligned: " << sizeof(FooAligned) << std::endl;
		std::cout << "Size Base_WithCharPrt: " << sizeof(Base_WithCharPrt) << std::endl;
		std::cout << "Size Base_WithFunc: " << sizeof(Base_WithFunc) << std::endl;
		std::cout << "Size Base_WithVirtFuncOnly: " << sizeof(Base_WithVirtFuncOnly) << std::endl;
		std::cout << "Size Base_WithVirtFunc: " << sizeof(Base_WithVirtFunc) << std::endl;
		std::cout << "Size Base_WithVirtFunc2: " << sizeof(Base_WithVirtFunc2) << std::endl;
		std::cout << "Size DerivedClass: " << sizeof(DerivedClass) << std::endl;
		std::cout << "Size DerivedClass2: " << sizeof(DerivedClass2) << std::endl;
		std::cout << "Size DerivedClassVirt: " << sizeof(DerivedClassVirt) << std::endl;
		std::cout << "Size DerivedClassWithVirtFuncs: " << sizeof(DerivedClassWithVirtFuncs) << std::endl;
		std::cout << "Size ClassWithVirtDestructor: " << sizeof(ClassWithVirtDestructor) << std::endl;
		std::cout << "Size DerivedClassWitVirtInhereance: " << sizeof(DerivedClassWitVirtInhereance) << std::endl;
		std::cout << "Size DerivedClassWitVirtInhereanceAndFunc: " << sizeof(DerivedClassWitVirtInhereanceAndFunc) << std::endl;
		std::cout << "Size C: " << sizeof(C) << std::endl;

        Virtual_FuncDiffClass::VirtualMethodTests();
	}

	//---------------------------------------------------------------------------------------------------//


	namespace VitrtInheritanceTests {

		class Base1 {
		public:
			int a1;
			int a2;
			int a3;
			int a4;
		};

		class Base2 {
		public:
			int b1;
			int b2;
			int b3;
			int b4;
		};


		class Derived : public Base1 {
		public:
			int x;
		};

		class VDerived : public virtual Base1 {
		public:
			int x;
		};


		void Test() {
			std::cout << "Base: " << sizeof(Base1) << std::endl;
			std::cout << "Not Virtual: " << sizeof(Derived) << std::endl;
			std::cout << "Virtual: " << sizeof(VDerived) << std::endl;
		}
	}
};


////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::LocalClasses {

	void fun()
	{
		// Local class  
		class Object {
		public:
			Object() {
				std::cout << "Local object created Object::Object()" << std::endl;
			}

		};

		Object obj;

		[[maybe_unused]]
		Object  *ptr;
		Object  *ptr1 = new Object();
	}

	void Test() {
		fun();
	}
}

////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::VirtualFunctions {

	class Base {
	public:
		virtual void show() { std::cout << "In Base" << std::endl; }
	};

	class Derived : public Base {
	public:
		virtual void show() { std::cout << "In Derived" << std::endl; }
	};

	void Call_BaseClass_Func_Hack()
	{
		Base *ptr = new Derived;
		ptr->Base::show();  // "In Base" will be printed
	}

	void Call_Method_FromObjectReference() {
		Derived d;
		Base &br = d;
		br.show();  // "In Derived" will be printed
	}
}

////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::ReturnValueOptimization {

	class Value {
	private:
		std::string text;

	public:
		Value() : Value("") {
			std::cout << "Value::Value()" << std::endl;
		}

		Value(const std::string& str) : text(str) {
			std::cout << "Value::Value(" << str << ")" << std::endl;
		}

		virtual ~Value() {
			std::cout << "Value::~Value(" << text << ")" << std::endl;
		}

		Value(const Value& bar) {
			this->text = text;
			std::cout << "Value::Value(" << bar.text << ") Copy constructor." << std::endl;
		}

		Value(Value&& bar) noexcept: text(std::move(bar.text))  {
			std::cout << "Value::Value(" << bar.text << ") Move constructor." << std::endl;
		}

		Value& operator=(const Value& bar) {
			std::cout << "Copy assignment operator" << std::endl;
			if (&bar != this) {
				this->text = bar.text;
			}
			return *this;
		}

		Value& operator=(Value&& bar) noexcept {
			std::cout << "Move assignment operator" << std::endl;
			if (&bar != this) {
				this->text = std::exchange(bar.text, "");
			}
			return *this;
		}

		void Info() {
			std::cout << this->text << std::endl;
		}
	};

	Value createValue(const std::string& str = "") {
		return Value(str);
	}

	Value createValue2(const std::string& str = "") {
		Value v(str);
		return v;
	}

	//-------------------------------------------------//

	void Create_NoCopyConstructor() {
		Value value = createValue("Test1");
		value.Info();
	}

	void Create_NoCopyConstructor2() {
		Value value = createValue2("Test2");
		value.Info();
	}
}

////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::CopyMove_Constructor_Operators {

	class Value {
	private:
		std::string text;

	public:
		Value() : Value("") {
			std::cout << "Value::Value()" << std::endl;
		}

		Value(const std::string& str) : text(str) {
			std::cout << "Value::Value(" << str << ")" << std::endl;
		}

		virtual ~Value() {
			std::cout << "Value::~Value(" << text << ")" << std::endl;
		}

		Value(const Value& bar) {
			this->text = text;
			std::cout << "Value::Value(" << bar.text << ") Copy constructor." << std::endl;
		}

		Value(Value&& bar) noexcept : text(std::move(bar.text)) {
			std::cout << "Value::Value(" << bar.text << ") Move constructor." << std::endl;
		}

		Value& operator=(const Value& bar) {
			std::cout << "Copy assignment operator" << std::endl;
			if (&bar != this) {
				this->text = bar.text;
			}
			return *this;
		}

		Value& operator=(Value&& bar) noexcept {
			std::cout << "Move assignment operator" << std::endl;
			if (&bar != this) {
				this->text = std::exchange(bar.text, "");
			}
			return *this;
		}

		void Info() {
			std::cout << this->text << std::endl;
		}
	};

	void CopyConstructorTests() {

		const auto func1 = [](const Value var) {
			// Do something
		};

		std::cout << "---------------Test1:" << std::endl;
		{
			Value var("Test1");
			func1(var);
		}
		std::cout << "\n---------------Test2:" << std::endl;
		{
			Value var("Test2");
			Value var2 = var;
		}
		std::cout << "\n---------------Test3:" << std::endl;
		{
			Value var("Test2");
			Value var2(var);
		}
	}

	void CopyElision() {
		std::cout << "----------------------------Test1----------------------------------:" << std::endl;
		{
			Value var = Value("Test2");
		}
	}

	void CopyAssignmentTests() {
		std::cout << "----------------------------Test1----------------------------------:" << std::endl;
		{
			Value var1("111"), var2("222");
			var1 = var2;
		}
	}

	void MoveConstructorTests() {
		std::cout << "\n----------------------------Test2----------------------------------:" << std::endl;
		{
			Value var = std::move(Value("Test2"));
		}
	}

	void MoveAssignmentTests() {
		std::cout << "----------------------------Test1----------------------------------:" << std::endl;
		{
			Value var1("111"), var2("222");
			var2 = std::move(var1);
		}
	}
}

namespace ObjectOrientedProgramming::CallBaseClassMembers {

	class A {
	public:
		A() { std::cout << "A::A()" << std::endl; }
		virtual ~A() { std::cout << "A::~A()" << std::endl; }

		virtual void Info() {
			std::cout << "A::Info()" << std::endl;
		}
	};

	class B : public A {
	public:
		B() { std::cout << "B::B()" << std::endl; }
		B(int i) { std::cout << "B::B(int)" << std::endl; }

		virtual ~B() { std::cout << "B::~B()" << std::endl; }

		virtual void Info() override {
			std::cout << "B::Info()" << std::endl;
		}
	};

	class C : public B {
	public:
		C() { std::cout << "C::C()" << std::endl; }
		C(int i) { std::cout << "C::C(int)" << std::endl; }

		virtual ~C() { std::cout << "C::~C()" << std::endl; }

		virtual void Info() override {
			//A::Info();
			std::cout << "C::Info()" << std::endl;
		}
	};

	class D : public C {
	public:
		D() { std::cout << "D::D()" << std::endl; }
		virtual ~D() { std::cout << "D::~D()" << std::endl; }

		virtual void Info() override {
			//A::Info();  // -- OK if A public-ly inherited to B
			//B::Info();  // -- OK if B public-ly inherited to C
			//C::Info();
			std::cout << "D::Info()" << std::endl;
		}
	};


	void Test() {
		/*
		A* a = new B;
		a->Info();
		delete a;
		*/
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::Virtual_Constructor_DestructorTests {

	class Base {
	public:
		Base() { std::cout << "Base::Base()" << std::endl; }
		virtual ~Base() {
			Hack(); // Crash here!!! Pure virtual exception Hack() is not implemented in Base
			std::cout << "Base::Base()" << std::endl;
		}
		void Hack() {
			Func();

		}
		virtual void Func() const noexcept = 0;
	};

	class Derived : public Base {
	public:
		Derived() { std::cout << __FUNCTION__ << std::endl; }
		~Derived() { std::cout << __FUNCTION__ << std::endl; }

		virtual void Func() const noexcept override {
			std::cout << "Derived::Func()" << std::endl;
		}
	};



	void PureVirtualException_InDestructor() {
		std::cout << "----------Test1:" << std::endl;
		{
			std::shared_ptr<Base> obj = std::make_shared<Derived>();
		}
		std::cout << "\n----------Test2:" << std::endl;
		{
			Base* obj = new Derived();
			delete obj;
		}
	}

	//---------------------------------------------------

	class A {
	public:
		A() { std::cout << "A::A()" << std::endl; }
		virtual ~A() { std::cout << "A::~A()" << std::endl; }
	};

	class B : public A {
	public:
		B() { std::cout << "B::B()" << std::endl; }
		~B() { std::cout << "B::~B()" << std::endl; }
	};

	class C : public B {
	public:
		C() { std::cout << "C::C()" << std::endl; }
		~C() { std::cout << "C::~C()" << std::endl; }
	};

	class D : public C {
	public:
		D() { std::cout << "D::D()" << std::endl; }
		~D() { std::cout << "D::~D()" << std::endl; }
	};


	void Virutual_Destructors_Invoke() {
		 A* obj = new D();
		// B* obj = new C();
		// C* obj = new D();

		// std::unique_ptr<A> ptr = std::make_unique<D>();  // This failes with 
		// std::shared_ptr<A> ptr = std::make_shared<D>();

		std::cout << std::endl;
		delete obj;
	}


	//---------------------------------------------------

	class VirtBase {
	public:
		VirtBase() { std::cout << "VirtBase::VirtBase()" << std::endl; }
		virtual ~VirtBase() = default;
	};

	class VirtDerived : public VirtBase {
	public:
		VirtDerived() { std::cout << "VirtDerived::VirtDerived()" << std::endl; }
		~VirtDerived() { std::cout << "VirtDerived::~VirtDerived()" << std::endl; }
	};

	void Default_Virtual_DTor() {
		VirtBase* obj = new VirtDerived();
		std::cout << std::endl;
		delete obj;
	}


	//---------------------------------------------------

	class Base1 {
	public:
		Base1() { 
			std::cout << "Base1::Base1()" << std::endl; 
		}
		virtual ~Base1() { 
			std::cout << "Base1::~Base1()" << std::endl;
		}
	};

	class Derived1: public Base1 {
	public:
		Derived1() {
			std::cout << "Derived1::Derived1()" << std::endl;
		}

		~Derived1() override { 
			std::cout << "Derived1::~Derived1()" << std::endl;
		}
	};


	class Derived2 : public Derived1 {
	public:
		Derived2() {
			std::cout << "Derived2::Derived2()" << std::endl;
		}
		~Derived2() override {
			std::cout << "Derived2::~Derived2()" << std::endl;
		}
	};

	void Override_Destructor_Test() {
		Base1* b = new Derived2();
		delete b;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::Destructors {

	class Object {
	private:
		std::string text;

	public:
		Object(const std::string& t) : text(t) {
			std::cout << "Object::Object(" << this->text << ")" << std::endl;
		}

		virtual ~Object() {
			std::cout << "Object::~Object(" << this->text << ")" << std::endl;
		}

	public:
		void print() {
			std::cout << "Text: " << this->text << std::endl;
		}
	};


	void Call_Destructor_Manually() {
		Object* ptr = new Object("Test1");

		ptr->~Object();

		// delete ptr;
	}

	void Call_Destructor_Manually_NewPlacement() {
		// Object *objects = new Object[10]; // ERROR: No default CTor

		int size = 10;
		void *memBlock = operator new[](size * sizeof(Object));
		Object *objects = static_cast<Object*>(memBlock);
		for (size_t offset = 0; offset < size; ++offset)
			new (objects + offset) Object("Object_" + std::to_string(offset));

		for (int i = size - 1; i >= 0; --i)
			objects[i].~Object();
		operator delete[](memBlock);
	}

    namespace MultipleDestructors::before_cpp_17
    {
        class Wrapper_Trivial {
        public:
            // ~Wrapper() = default;
            ~Wrapper_Trivial() {
                std::cout << "Trivial\n";
            }
        };

        class Wrapper_NonTrivial {
        public:
            ~Wrapper_NonTrivial() {
                std::cout << "Not trivial\n";
            }
        };

        template<typename T>
        class Wrapper : public std::conditional_t<std::is_trivially_destructible_v<T>, Wrapper_Trivial, Wrapper_NonTrivial> {
            T t;
        };

        void test() {
            Wrapper<int> wrappedInt;
            Wrapper<std::string> wrappedString;
        }
    }

    namespace MultipleDestructors::after_cpp_17
    {
        template<typename T>
        class Wrapper {
            T t;
        public:
            ~Wrapper() requires (!std::is_trivially_destructible_v<T>) {
                std::cout << "Not trivial\n";
            }

            // ~Wrapper() = default;
            ~Wrapper() {
                std::cout << "Trivial\n";
            }
        };

        void test() {
            Wrapper<int> wrappedInt;
            Wrapper<std::string> wrappedString;
        }
    }

    void MultipleDestructorsTests()
    {
        MultipleDestructors::before_cpp_17::test();
        MultipleDestructors::after_cpp_17::test();
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::ObjectsLifeTime {

	class Long {
	protected:
		int value;

	public:
		Long() : value(0) {
			std::cout << "Long::Long(" << value << ")" << std::endl;
		}

		explicit Long(int val) : value(val) {
			std::cout << "Long::Long(" << value << ")" << std::endl;
		}

		Long(const Long &obj) {
			this->value = obj.value;
			std::cout << "Copy constructor: Long::Long(" << value << ")" << std::endl;
		}

		Long& operator=(const Long& right) {
			if (&right != this) {
				value = right.value;
			}
			std::cout << "Copy assignment: Long::Long(" << value << ")" << std::endl;
			return *this;
		}

		Long(Long && obj) noexcept : value(std::exchange(obj.value, 0)) {
			std::cout << "Move constructor: Long::Long(" << value << ")" << std::endl;
		}

		Long& operator=(Long&& right) noexcept {
			if (this != &right) {
				this->value = std::exchange(right.value, 0);
			}
			std::cout << "Move assignment: Long::Long(" << value << ")" << std::endl;
			return *this;
		}

		virtual ~Long() {
			std::cout << "Long::~Long(" << value << ")" << std::endl;
		}
	};

	Long get_Long() {
		return {};
	}

	void Extend_LocalObject_Lifetime() {
		const auto& longRef = get_Long();
#if 0
		auto& sObjRef = get_S(); // THIS IS WILL NOT EVEN COMPILE
#endif
		std::cout << "OK!!" << std::endl;
	}

	void Extend_LocalObject_Lifetime_RVO() {
		auto longObj = get_Long();
		std::cout << "OK!!" << std::endl;
	}

}

namespace ObjectOrientedProgramming::PolymorthismWithoutPointers {

	struct ICalculator {
		virtual int compute(int input) const = 0;
		virtual void log(int input, int output) const = 0;
		virtual ~ICalculator() {};
	};

	struct BigCalculator : public ICalculator {
		int compute(int input) const override {
			return input * 5;
		}

		void log(int input, int output) const override {
			std::cout << "BigCalculator took an input of " << input << " and produced an output of " << output << '\n';
		}
	};

	struct SmallCalculator : public ICalculator {
		int compute(int input) const override {
			return input + 2;
		}

		void log(int input, int output) const override {
			std::cout << "SmallCalculator took an input of " << input << " and produced an output of " << output << '\n';
		}
	};

	struct Calculator
	{
	private:
		using AnyCalcPtr = std::function<ICalculator& (std::any&)>;

	public:
		template<typename ConcreteCalculator>
		Calculator(ConcreteCalculator&& calculator) :
			storage{ std::forward<ConcreteCalculator>(calculator) },
			getter{ [](std::any& storage) -> ICalculator& {
				return std::any_cast<ConcreteCalculator&>(storage);
			} } {
			// Do nothing here.
		}

		ICalculator* operator->() { return &getter(storage); }

	private:
		std::any storage;
		AnyCalcPtr getter;
	};


	void Test() {
		std::vector<Calculator> anyVector;

		anyVector.emplace_back(BigCalculator());
		anyVector.emplace_back(SmallCalculator());
		anyVector.emplace_back(BigCalculator());
		anyVector.emplace_back(SmallCalculator());


		for (Calculator& T : anyVector)
			T->log(1, 2);
	}
}

namespace ObjectOrientedProgramming::Nested_Classes {

	class TestClass {
	private:
		std::string text;

	protected:

		class SmallInnerClass {
		public:
			SmallInnerClass() {
				std::cout << "   " << __FUNCTION__ << std::endl;
				// std::cout << "   " << text << std::endl;
			}
		};

	public:
		TestClass(std::string&& txt) : text{std::move(txt)} {
			std::cout << __FUNCTION__ << ": " << text << std::endl;
		}

		void create() {
			SmallInnerClass o;
		}

	};


	void Test() {

		TestClass obj("Some_Text");
		obj.create();
	}

	//-----------------------------------------------------------------------//

	class Product {
	private:
		std::string first_name{ "Jonh" };
		std::string seconds_name{ "Dow" };

	private:

		class Builder {
		public:
			Builder* setFirstName(std::string&& name) {
				product->first_name.assign(std::move(name));
				return this;
			}

			Builder* setSecondName(std::string&& name) {
				product->seconds_name.assign(std::move(name));
				return this;
			}

			std::unique_ptr<Product> build() {
				return std::move(product);
			}

			Builder() { std::cout << "Builder() created" << std::endl; }
			~Builder() { std::cout << "Builder() destroyed" << std::endl; }

		private:
			std::unique_ptr<Product> product = std::make_unique<Product>();
		};

	public:
		static Builder* getBuilder() {
			return new Builder();
		}

		void info() {
			std::cout << "Product(first_name: " << first_name << ", second_name: " << seconds_name << ")\n";
		}

	};

	class ProductNew {
	private:
		std::string first_name{ "Jonh" };
		std::string seconds_name{ "Dow" };

	private:

		class Builder {
		public:
			const Builder& setFirstName(std::string&& name) const {
				product->first_name.assign(std::move(name));
				return *this;
			}

			const Builder& setSecondName(std::string&& name) const {
				product->seconds_name.assign(std::move(name));
				return *this;
			}

			std::unique_ptr<ProductNew> build() const {
				return std::move(product);
			}

			Builder() { std::cout << "Builder() created" << std::endl; }
			~Builder() { std::cout << "Builder() destroyed" << std::endl; }

		private:
			mutable std::unique_ptr<ProductNew> product = std::make_unique<ProductNew>();
		};

	public:
		const static Builder& getBuilder() {
			return Builder();
		}

		void info() {
			std::cout << "Product(first_name: " << first_name << ", second_name: " << seconds_name << ")\n";
		}

	};

	void BuilderTest() {

		std::unique_ptr<Product> prod = Product::getBuilder()->
			setFirstName("FIRST")->
			setSecondName("SECOND")->build();

		prod->info();

		std::unique_ptr<ProductNew> prod1 = ProductNew::getBuilder().
			setFirstName("FIRST").
			setSecondName("SECOND").build();

		prod1->info();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::ClassVariables {

	class Type {
		static inline std::vector ints{ 1, 2, 3, 4, 5, 6, 7 }; // deduced vector<int>

		inline static unsigned int default_width = 1028;
		inline static unsigned int default_height = 768;
	};


	class Type2 {
		inline static int default_height {[] {
			// std::cout << "Calling lambda to init class static variable!\n";
			return 10;
		}()} ;
	};

	void ClassInlineVariables() {

	}

	void InitStaticVariable_WithLambda() {
		Type2 T{};
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ObjectOrientedProgramming::InheritanceAndTemplates {

	struct Handler {
		virtual void process() noexcept = 0;
	};


	struct HandlerOne : public Handler {
		virtual void process() noexcept override {
			std::cout << "HandlerOne::process() called\n";
		}
	};

	struct HandlerTwo : public Handler {
		virtual void process() noexcept override {
			std::cout << "HandlerTwo::process() called\n";
		}
	};


	class Command {
	public:
		virtual void execute() noexcept = 0;
	};

	template<typename _Handler>
	class ConcreteCommand : public Command {
		_Handler handler{};

	public:
		virtual void execute() noexcept override {
			handler.process();
		}
	};


	void Test()
	{
		std::unique_ptr<Command> cmd1 = std::make_unique<ConcreteCommand<HandlerOne>>();
		cmd1->execute();

		std::unique_ptr<Command> cmd2 = std::make_unique<ConcreteCommand<HandlerTwo>>();
		cmd2->execute();
	}
}


namespace ObjectOrientedProgramming::Ref_Qualifiers_Class_Members
{
    struct MyClass
    {
        void operation() & {
            std::cout << "mutable l-value\n";
        }
        void operation() && {
            std::cout << "pr_value or xvalue\n";
        }
        void operation() const & {
            std::cout << "const l-value\n";
        }
    };

    void test()
    {
        const MyClass objConst;
        MyClass obj;

        objConst.operation();       // const l-value

        obj.operation();            // mutable l-value

        std::move(obj).operation(); // pr_value or xvalue

        MyClass{}.operation();      // pr_value or xvalue
    }
}


void ObjectOrientedProgramming::TestAll()
{
	// UserDefindedConversation();
	// ExceptionInConstructor();
	

	// ******************************** CONSTRUCTORS: *********************************//

	// Constructors::CallOneConstructorFromAnother();
	// Constructors::CallBaseClassConstructor();
	// Constructors::Call_Base_Class_Method();
	// Constructors::InheritedConstructors();

	// Constructor_Invoke_Order::FunctionsInvokeOrder();
	// Constructor_Invoke_Order::OverloadWithoutVituals_Test();
	// Constructor_Invoke_Order::VirtClassTests();

	// Private_Ctor_Dtor::Test();
	// Private_Ctor_Dtor::DirtyHackTest();

	// CopyMove_Constructor_Operators::CopyConstructorTests();
	// CopyMove_Constructor_Operators::CopyElision();
	// CopyMove_Constructor_Operators::CopyAssignmentTests();
	// CopyMove_Constructor_Operators::MoveConstructorTests();
	// CopyMove_Constructor_Operators::MoveAssignmentTests();

	// ConstructorInitializationLists::Test();
	// ConstructorInitializationLists::Call_Members_InitializationOrder();
	// ConstructorInitializationLists::InitList_Construction();

	// Virtual_Constructor_DestructorTests::PureVirtualException_InDestructor();
	// Virtual_Constructor_DestructorTests::Virutual_Destructors_Invoke();
	// Virtual_Constructor_DestructorTests::Default_Virtual_DTor();
	// Virtual_Constructor_DestructorTests::Override_Destructor_Test();

	// TestUseVirtualFuncsConstructor::Test();
	// TestUseVirtualFuncsDestructor::Test();


    // ******************************** DESTRUCTORS ********************************* //

    // Destructors::Call_Destructor_Manually();
    Destructors::Call_Destructor_Manually_NewPlacement();
    // Destructors::MultipleDestructorsTests();


    // ******************************** OVERLOAD & OVERRIDE: *********************************//

	// Overloaded_Methods_InDerivedClass::Test();
	// Overloaded_Methods_InDerivedClass::Test_AccessHidenMethods();
	// Overloaded_Methods_InDerivedClass::HideBaseClassMethod_Test();
	// Overloaded_Methods_InDerivedClass::Overload_Test___BadUsage();
	// Overloaded_Methods_InDerivedClass::Overload_Test___GoodUsage();
	// Overloaded_Tests::Overload_StaticMethod();

	// Override_Test::Override_WithOut_Const();
	

	// ******************************** OVERLOAD & OVERRIDE: *********************************//

	// ClassVariables::ClassInlineVariables();


	// PureVirtualException::Test();
	// PureVirtualException::Test1();

	// UserDefinedConversion::Test1();
	// UserDefinedConversion::Test_AdHoc_Polymorthism();
	// ParametricPolymorphism::Test();

	// Explicit::CTor_Operator_Bool_1();
	// Explicit::CTor_Operator_Bool_2();

	// AccessModifiersTests::Private_FriendClassTest();
	// AccessModifiersTests::Private_FriendFunction();
	// AccessModifiersTests::Private_Derived_FriendClass();

	// VirtualFunctions::Call_BaseClass_Func_Hack();
	// VirtualFunctions::Call_Method_FromObjectReference();

	// ReturnValueOptimization::Create_NoCopyConstructor();
	// ReturnValueOptimization::Create_NoCopyConstructor2();

	// Operator_Overload_Tests::Inherit_Copy_Assignment_Operator();
	// OperatorOverloading::Tests();
	
	// Static_Members_Inheritance::Test();
	// AggregateInitialization::InitTest();

	// Classes_Structs_Sizeof_Tests::StructSizeTest();
	// Classes_Structs_Sizeof_Tests::VitrtInheritanceTests::Test();

	// LocalClasses::Test();

	// Nested_Classes::Test();
	// Nested_Classes::BuilderTest();
	

	// ObjectsLifeTime::Extend_LocalObject_Lifetime();
	// ObjectsLifeTime::Extend_LocalObject_Lifetime_RVO();

	// AccessMembers::Test();
	// MultipleInheritance::TEST();
	// CallBaseClassMembers::Test();


	// PolymorthismWithoutPointers::Test();

	// InheritanceAndTemplates::Test();

    // Ref_Qualifiers_Class_Members::test();
}
