//============================================================================
// Name        : SharedPtrTests.cpp
// Created on  : 21.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Shared PTR C++  src
//============================================================================

#define _SILENCE_CXX17_SHARED_PTR_UNIQUE_DEPRECATION_WARNING

#include <cassert>
#include <atomic>
#include <iostream>
#include <string>
#include <string_view>
#include <chrono>
#include <algorithm>
#include <thread>
#include <mutex>
#include <array>
#include <vector>
#include <memory>

#include "../Integer/Integer.h"
#include "SharedPtrTests.h"


namespace SharedPtr_Tests {

	class Base {
	public:
		Base() {
			std::cout << "  Base::Base()" << std::endl;
		}
		virtual ~Base() {
			std::cout << "  Base::~Base()" << std::endl;
		}
	};

	class Derived : public Base {
	public:
		Derived() {
			std::cout << "  Derived::Derived()" << std::endl;
		}
		virtual ~Derived() {
			std::cout << "  Derived::~Derived()" << std::endl;
		}
	};
}

namespace SharedPtr_Tests::ExceptionHandling {

	void Exeption_DuringCreate_SharedPtr() {
		auto bar = []()-> int {
			throw std::runtime_error("Exception from bar()");
			return 0;
		};
		auto foo = [&](std::shared_ptr<Integer> integer, int bar) {
			integer->printInfo();
		};

		try {
			foo(std::shared_ptr<Integer>(new Integer(123)), bar());
		} catch (const std::exception& exc) {
			std::cout << "Exception occured." << std::endl;
			std::cout << exc.what() << std::endl;
		}
	}

	void Exeption_DuringCreate_SharedPtr2() {
		auto bar = []()-> int {
			throw std::runtime_error("Exception from bar()");
			return 0;
		};
		auto foo = [&](std::shared_ptr<Integer> integer, int bar) {
			integer->printInfo();
		};

		try {
			std::shared_ptr<Integer> int_var(new Integer(123));
			foo(int_var, bar());
		}
		catch (const std::exception& exc) {
			std::cout << "Exception occured." << std::endl;
			std::cout << exc.what() << std::endl;
		}
	}
}

namespace SharedPtr_Tests::DeleterTest {

	class IntegerDeleter {
	public:
		void operator() (Integer* integer) {
			std::cout << "****** Calling delete for Integer object... " << std::endl;
			delete integer;
		}
		void info() {
			std::cout << "Hello from IntegerDeleter" << std::endl;
		}

		// IntegerDeleter() { std::cout << "IntegerDeleter()" << std::endl; }
		// IntegerDeleter(const IntegerDeleter& deleter) { std::cout << "IntegerDeleter(const IntegerDeleter& deleter)" << std::endl; }
		// IntegerDeleter(IntegerDeleter&& deleter) noexcept { std::cout << "IntegerDeleter(IntegerDeleter&& deleter) noexcept" << std::endl; }
		//  ~IntegerDeleter() { std::cout << "~IntegerDeleter()" << std::endl; }
	};


	void CustomDeleter_Test_1() {
		const auto deleter = [](Integer* x)-> void {
			std::cout << "DELETER FUNCTION CALLED" << std::endl;
			delete x;
		};

		{
			std::shared_ptr<Integer> integer(new Integer(111), deleter);
			integer->printInfo();
		}

		std::cout << "\nTest2:\n" << std::endl;

		{
			std::shared_ptr<Integer> integer = std::shared_ptr<Integer>(new Integer(222), deleter);
			integer->printInfo();
		}
	}

	void CustomDeleter_Test_2() {
		std::shared_ptr<Integer> integer(new Integer(555), IntegerDeleter());
		integer->printInfo();
	}

	void IntegerDeleterFunc(Integer* integer) {
		std::cout << "****** Calling delete for Integer object... " << std::endl;
		delete integer;
	}

	void Get_Deleter_Test()
	{
		std::shared_ptr<Integer> integer(new Integer(111), IntegerDeleterFunc);
		auto deleter = std::get_deleter<void(*)(Integer*)>(integer);
		if (nullptr == deleter) {
			std::cout << "Failed to get deleter" << std::endl;
			return;
		}
		if (*deleter == IntegerDeleterFunc)
			std::cout << "OKKK" << std::endl;
	}
}

namespace SharedPtr_Tests {

	void worker(std::shared_ptr<Base> p)
	{
		std::this_thread::sleep_for(std::chrono::seconds(5));
		std::shared_ptr<Base> lp = p; // thread-safe, even though the shared use_count is incremented
		{
			static std::mutex io_mutex;
			std::lock_guard<std::mutex> lk(io_mutex);
			std::cout << "local pointer in a thread:\n"
					  << "  lp.get() = " << lp.get()
					  << ", lp.use_count() = " << lp.use_count() << std::endl;
		}
	}

	class Fooooooo {
	private:
		uint64_t value1 = 1;
		uint64_t value2 = 2;
		uint64_t value3 = 3;
		uint64_t value4 = 4;

	public:
		Fooooooo() {
			std::cout << "Fooooooo()" << std::endl;
		}

		virtual ~Fooooooo() {
			std::cout << "~Fooooooo()" << std::endl;
		}
	};
	
	struct C { int* data; };

	void Create() {
		std::shared_ptr<int> p1;
		std::shared_ptr<int> p2(nullptr);
		std::shared_ptr<int> p3(new int);
		std::shared_ptr<int> p4(new int, std::default_delete<int>());
		std::shared_ptr<int> p5(new int, [](int* p) {delete p; }, std::allocator<int>());
		std::shared_ptr<int> p6(p5);
		std::shared_ptr<int> p7(std::move(p6));
		std::shared_ptr<int> p8(std::unique_ptr<int>(new int));
		std::shared_ptr<C> obj(new C);
		std::shared_ptr<int> p9(obj, obj->data);

		std::cout << "use_count:\n";
		std::cout << "p1: " << p1.use_count() << '\n';
		std::cout << "p2: " << p2.use_count() << '\n';
		std::cout << "p3: " << p3.use_count() << '\n';
		std::cout << "p4: " << p4.use_count() << '\n';
		std::cout << "p5: " << p5.use_count() << '\n';
		std::cout << "p6: " << p6.use_count() << '\n';
		std::cout << "p7: " << p7.use_count() << '\n';
		std::cout << "p8: " << p8.use_count() << '\n';
		std::cout << "p9: " << p9.use_count() << '\n';
	}

	void Simple_Test_0() {
		Integer* item1 = new Integer(111);
		Integer* item2 = new Integer(222);
		std::shared_ptr<Integer> ptr1(item1);
		{
			std::shared_ptr<Integer> ptr2(item2);
			std::cout << "Killing item2 ptr" << std::endl;
		}
		std::cout << "Killing item1 ptr" << std::endl;
	}

	void Simple_Test_1() {
		Integer* item = new Integer(1);
		std::shared_ptr<Integer> ptr1(item);
		{
			std::shared_ptr<Integer> ptr2(ptr1);
			std::cout << "Killing one shared pointer" << std::endl;
		}
		std::cout << "Killing another shared pointer" << std::endl;
	}

	void Simple_Test_2() {
		auto increment = [](std::shared_ptr<Integer> integer_prt)-> void {
			(*integer_prt)++;
		};
		auto sp = std::make_shared<Integer>(10);
		increment(sp);
		std::cout << *sp << std::endl;
	}

	void Simple_Test_3() {
		auto increment = [](std::shared_ptr<Integer> integer_prt)-> void {
			integer_prt->increment();
		};
		auto sp = std::make_shared<Integer>(10);
		increment(sp);
		std::cout << *sp << std::endl;
	}

	void SimpleTest_Crush() {
		Integer* item = new Integer(1);
		std::shared_ptr<Integer> ptr1(item);
		{
			std::shared_ptr<Integer> ptr2(item);
			std::cout << "Killing one shared pointer" << std::endl;
		}
		std::cout << "Killing another shared pointer" << std::endl;
	}


	void Reset_Test() {
		{
			std::shared_ptr<Integer> integer = std::make_shared<Integer>(11);
			integer->printInfo();

			std::cout << "Resest std::shared_ptr<Integer>(11)" << std::endl;
			integer.reset(new Integer(22));
			std::cout << "Clean up" << std::endl;
		}
		std::cout << "\n\n--------------------------Test3: --------------------------\n" << std::endl;
		{
			std::shared_ptr<Integer> integer = std::make_shared<Integer>(11);
			integer->printInfo();

			integer.reset(new Integer(22), DeleterTest::IntegerDeleter());
			integer->printInfo();
		}
		std::cout << "\n\n--------------------------Test4: --------------------------\n" << std::endl;
		{
			std::shared_ptr<Integer> integer = std::make_shared<Integer>(11);
			integer->printInfo();

			integer.reset();
			std::cout << "bp2.use_count() = " << integer.use_count() << '\n';
		}
	}

	void Count_Test() {
		auto func2 = [](std::shared_ptr<Integer> sp) {
			std::cout << "func2 : sp.use_count() == " << sp.use_count() << std::endl;
		};
		auto func1 = [&](std::shared_ptr<Integer> sp) {
			std::cout << "func1 : sp.use_count() == " << sp.use_count() << std::endl;
			func2(sp);
		};
		std::shared_ptr<Integer> sp1 = std::make_shared<Integer>(10);

		std::cout << "sp1.use_count() == " << sp1.use_count() << std::endl;
		func1(sp1);
	}

// #define USING_CPP17

	void Unique_Test()
	{

		std::cout << "\nTest1:" << std::endl;
		{
			std::shared_ptr<Integer> sp1 = std::make_shared<Integer>(5);
#ifdef USING_CPP17
			std::cout << "sp1.unique() == " << std::boolalpha << sp1.unique() << std::endl;
#endif

			std::shared_ptr<Integer> sp2{ sp1 };
#ifdef USING_CPP17
			std::cout << "sp1.unique() == " << std::boolalpha << sp1.unique() << std::endl;
#endif
		}

		std::cout << "\nTest2:" << std::endl;
		{
			std::shared_ptr<Integer> sp1 = std::make_shared<Integer>(11);
			std::shared_ptr<Integer> sp2 = std::make_shared<Integer>(22);

#ifdef USING_CPP17
			std::cout << "sp1.unique() == " << std::boolalpha << sp1.unique() << std::endl;
			std::cout << "sp2.unique() == " << std::boolalpha << sp2.unique() << std::endl;
#endif
			sp2 = sp1;

#ifdef USING_CPP17
			std::cout << "sp1.unique() == " << std::boolalpha << sp1.unique() << std::endl;
			std::cout << "sp2.unique() == " << std::boolalpha << sp2.unique() << std::endl;
#endif
		}
	}

	void GOOD_Usage_Test1()
	{
		auto foo = [&](std::shared_ptr<Integer> integer) {
			integer->printInfo();
		};
		std::shared_ptr<Integer> integer = std::make_shared<Integer>(5);
		foo(integer);
	}

	int bar() {
		throw std::runtime_error("Exception from bar()");
	}

	void BAD_Usage_Test() {
		const auto foo = [&](const auto bar, std::shared_ptr<Integer> integer) {
			integer->printInfo();
		};
		try {
			// Not the fact that Integer() will be destroyes
			foo(bar(), std::shared_ptr<Integer>(new Integer(3)));
		}
		catch (const std::exception& exc) {
			std::cout << exc.what() << std::endl;
		}
	}

	void BAD_Usage_Test1() {
		const auto bar1 = []()-> void {
			throw std::runtime_error("Exception from bar()");
		};
		const auto foo = [&](std::shared_ptr<Integer> integer, const auto bar) {
			integer->printInfo();
		};
		
		try {
			std::shared_ptr<Integer> integer = std::make_shared<Integer>(5);
			foo(integer, bar());
		} catch (const std::exception& exc) {
			std::cout << "Exception raied" << std::endl;
			std::cout << exc.what() << std::endl;
		}
	}

	std::shared_ptr<Integer> return_shared_ptr_OK() {
		std::shared_ptr<Integer> integer = std::make_shared<Integer>(123);
		return integer;
	}

	std::shared_ptr<Integer> return_shared_ptr_BAD() {
		std::shared_ptr<Integer> integer = std::make_shared<Integer>(123);
		return integer;
	}

	void BAD_Usage_Test2() {
		{
			std::cout << "---- This is OK ----- " << std::endl;
			auto integer = return_shared_ptr_OK();
			integer->printInfo();
		}

		{
			std::cout << "\n---- This is BADDDD ----- \n" << std::endl;
			auto integer = return_shared_ptr_BAD();
			integer->printInfo();
		}

	}

	void VariousTests() {
		const auto bar1 = []() {
			throw std::runtime_error("Exception from bar()");
		};
		const auto foo = [&](std::shared_ptr<Integer> integer, const auto bar) {
			integer->printInfo();
		};
		try {
			//std::shared_ptr<Integer> integer =  std::make_shared<Integer>(5);
			//foo(integer, bar());
			foo(std::shared_ptr<Integer>(new Integer(3)), bar());
		}
		catch (const std::exception& exc) {
			std::cout << exc.what() << std::endl;
		}
	}

	void Swap_Test()
	{
		{
			std::shared_ptr<Integer> int1 = std::make_shared<Integer>(11);
			std::shared_ptr<Integer> int2 = std::make_shared<Integer>(22);

			int1->printInfo();
			int2->printInfo();

			int1.swap(int2);

			int1->printInfo();
			int2->printInfo();
		}
	}

	void Get_Test()
	{
		std::shared_ptr<Integer> int1 = std::make_shared<Integer>(11);
		std::cout << "TYPE: " <<  typeid(int1).name() << std::endl;

		std::cout << "Count: " << int1.use_count() << std::endl;

		std::shared_ptr<Integer> int2(int1);

		std::shared_ptr<Integer> int3(int2);

		std::cout << "Count: " << int1.use_count() << std::endl;
	}

	void Operator_Index() {
		/*
		std::shared_ptr<std::vector<int>> sharedVectoPtr = std::make_shared <std::vector<int>>();
		for (auto i : { 0,1,2,3,4,5 })
			sharedVectoPtr->push_back(i);

		std::cout << sharedVectoPtr->size() << std::endl;
		for (auto i : { 0,1,2,3,4,5 })
			std::cout << "[" << i << "] = " << (*sharedVectoPtr.get())[i] << std::endl;
		*/

		std::shared_ptr<std::array<size_t, 10>> ptr = std::make_shared<std::array<size_t, 10>>();
		for (auto i = 0; i < ptr->size(); i++)
			std::cout << ptr->operator[](i) << " ";
		std::cout << std::endl;

		for (auto i = 0; i < ptr->size(); i++)
			std::cout << (*ptr)[i] << " ";
		std::cout << std::endl;
	
	}

	struct A {
		static inline char static_type[] = "class A";
		const char* dynamic_type;

		A() { dynamic_type = static_type; }
	};

	struct B : A {
		static inline char static_type[] = "class B";

		B() { dynamic_type = static_type; }
	};


	void Dynamic_Ppointer_Cast() {
		std::shared_ptr<B> bar{ std::make_shared<B>() };
		std::shared_ptr<A> foo{ std::dynamic_pointer_cast<A>(bar) };

		std::cout << "foo's static  type: " << foo->static_type << '\n';
		std::cout << "foo's dynamic type: " << foo->dynamic_type << '\n';
		std::cout << "bar's static  type: " << bar->static_type << '\n';
		std::cout << "bar's dynamic type: " << bar->dynamic_type << '\n';
	}
}

namespace SharedPtr_Tests::Enable_Shared_From_This {

	template <class _Ty>
	class enable_shared_from_this_ex { // provide member functions that create shared_ptr to this
	public:
		using _Esft_type = enable_shared_from_this_ex;

        [[nodiscard]]
        std::shared_ptr<_Ty> shared_from_this() {
			return std::shared_ptr<_Ty>(_Wptr);
		}

        [[nodiscard]]
        std::shared_ptr<const _Ty> shared_from_this() const {
			return std::shared_ptr<const _Ty>(_Wptr);
		}

        [[nodiscard]]
        std::weak_ptr<_Ty> weak_from_this() noexcept {
			return _Wptr;
		}

        [[nodiscard]]
        std::weak_ptr<const _Ty> weak_from_this() const noexcept {
			return _Wptr;
		}

	protected:
		constexpr enable_shared_from_this_ex() noexcept : _Wptr() {}

		enable_shared_from_this_ex(const enable_shared_from_this_ex&) noexcept : _Wptr() {
			// construct (must value-initialize _Wptr)
		}

		// assign (must not change _Wptr)
		enable_shared_from_this_ex& operator=(const enable_shared_from_this_ex&) noexcept {
			return *this;
		}

		~enable_shared_from_this_ex() = default;

	private:
		template <class _Yty>
		friend class std::shared_ptr;

		mutable std::weak_ptr<_Ty> _Wptr;
	};

	//--------------------------------------------------------------------------------//

	struct Good : std::enable_shared_from_this<Good> {
		std::shared_ptr<Good> getptr() {
			return shared_from_this();
		}
		~Good() { std::cout << "Good::~Good() called\n"; }
	};

	struct Bad {
		std::shared_ptr<Bad> getptr() {
			return std::shared_ptr<Bad>(this);
		}
		~Bad() { std::cout << "Bad::~Bad() called\n"; }
	};

	//--------------------------------------------------------------------------------//

	class SharedString : public std::enable_shared_from_this<SharedString> {
	private:
		std::string value;

	public:
		SharedString() : value("") {
			std::cout << "SharedString::SharedString(" << this->value << ")" << std::endl;
		}
		SharedString(const std::string& val) : value(val) {
			std::cout << "SharedString::SharedString(" << this->value << ")" << std::endl;
		}
		SharedString(const SharedString& obj) {
			std::cout << "SharedString::SharedString(" << this->value << "). Copy constructor" << std::endl;
			this->value = obj.value;
		}
		virtual ~SharedString() {
			std::cout << "SharedString::~SharedString(" << this->value << ")" << std::endl;
		}
		std::shared_ptr<SharedString> getSharedPrt() {
			std::cout << "getSharedPrt()" << std::endl;
			return this->shared_from_this();
		}
		std::weak_ptr<SharedString> getWeakPrt() {
			return this->weak_from_this();
		}
	};
	

	class SharedStringEx : public enable_shared_from_this_ex<SharedStringEx> {
	private:
        std::string value;

	public:
		template<typename... Args>
		SharedStringEx(Args&& ... params) : value{ std::forward<Args>(params)... } {
			std::cout << "SharedStringEx::SharedStringEx(" << this->value << ")" << std::endl;
		}

		SharedStringEx(const SharedStringEx& obj) {
			std::cout << "SharedStringEx::SharedStringEx(" << this->value << "). Copy constructor" << std::endl;
			this->value = obj.value;
		}


		std::shared_ptr<SharedStringEx> getSharedPrt() {
			return this->shared_from_this();
		}

		std::weak_ptr<SharedStringEx> getWeakPrt() {
			return this->weak_from_this();
		}


		virtual ~SharedStringEx() {
			std::cout << "SharedStringEx::~SharedStringEx(" << this->value << ")" << std::endl;
		}
	};

	//--------------------------------------------------------------------------------//

	void EnableSharedFomThis_Good() {
		std::shared_ptr<Good> gp1 = std::make_shared<Good>();
		std::shared_ptr<Good> gp2 = gp1->getptr();
		std::cout << "gp2.use_count() = " << gp2.use_count() << '\n';
	}

	void EnableSharedFomThis_Bad() {
		std::shared_ptr<Bad> bp1 = std::make_shared<Bad>();
		std::shared_ptr<Bad> bp2 = bp1->getptr();
		std::cout << "bp2.use_count() = " << bp2.use_count() << '\n';
	}


	void EnableSharedFromThis_Test_1()
	{
		std::shared_ptr<SharedString> bp1 = std::make_shared<SharedString>("Some_Test_String");
		std::shared_ptr<SharedString> bp2 = bp1->getSharedPrt();
		std::cout << "bp2.use_count() = " << bp2.use_count() << std::endl;
	}

	void EnableSharedFromThis_Test_2()
	{
		/*
		{
			SharedString str("Some_Test_String");

			std::shared_ptr<SharedString> strPtr1 = str.getSharedPrt();
			std::cout << "=======>" << std::endl;
		}
		*/

		std::cout << "\n--------------------------------------- Test2 -----------------------------------\n";

		{
			SharedString str("Some_Test_String");

			std::shared_ptr<SharedString> strPtr1 = std::shared_ptr<SharedString>(&str);
			std::cout << "=======>" << std::endl;

			std::shared_ptr<SharedString> strPtr2 = strPtr1->getSharedPrt();
			std::cout << "use_count() = " << strPtr2.use_count() << std::endl;
		}
	}

	void EnableSharedFromThis_Custom()
	{
		std::shared_ptr<SharedStringEx> bp1 = std::make_shared<SharedStringEx>("TEsT");
		std::shared_ptr<SharedStringEx> bp2 = bp1->getSharedPrt();
		std::cout << "bp2.use_count() = " << bp2.use_count() << std::endl;
	}
}

namespace SharedPtr_Tests::Allocation {

	template <class T>
	class CustomAllocator {
		typedef T value_type;
		CustomAllocator() noexcept {
			std::cout << "CustomAllocator()" << std::endl;
		}

		template <class U> CustomAllocator(const CustomAllocator<U>&) noexcept {}
		T* allocate(std::size_t n) {
			return reinterpret_cast<T*>(::operator new(n * sizeof(T)));
		}

		void deallocate(T* p, std::size_t n) {
			::operator delete(p);
		}
	};

	void Allocate_Shared() {
		/** the default allocator for int. **/
		std::allocator<Integer> allocator;    

		/** the default allocator for int. **/
		// std::default_delete<int> deleter;

		std::shared_ptr<int> foo = std::allocate_shared<int>(allocator, 10);

		auto bar = std::allocate_shared<int>(allocator, 20);
		auto baz = std::allocate_shared<std::pair<int, int>>(allocator, 30, 40);

		std::cout << "*foo: " << *foo << std::endl;
		std::cout << "*bar: " << *bar << std::endl;
		std::cout << "*baz: " << baz->first << ' ' << baz->second << std::endl;
	}

	void Allocate_Shared_Integer() {
		/** the default allocator for int. **/
		std::allocator<Integer> allocator;

		std::shared_ptr<Integer> integer = std::allocate_shared<Integer>(allocator, 10);
		std::cout << *integer << std::endl;
	}


	void Allocate_Shared_CustomAllocator() {
		//auto ptr = std::allocate_shared<Fooooooo>(CustomAllocator);
	}

	/*
	void Allocator_Test_DEPRECATED() {
		// allocator for string values
		std::allocator<std::string> myAllocator;

		// allocate space for three strings
		std::string* str = myAllocator.allocate(3);

		// construct these 3 strings
		myAllocator.construct(str, "Geeks");
		myAllocator.construct(str + 1, "for");
		myAllocator.construct(str + 2, "Geeks");

		std::cout << str[0] << str[1] << str[2];

		// destroy these 3 strings
		myAllocator.destroy(str);
		myAllocator.destroy(str + 1);
		myAllocator.destroy(str + 2);

		// deallocate space for 3 strings
		myAllocator.deallocate(str, 3);
	}
	*/

}

namespace SharedPtr_Tests::Multithreading {
	
	void Bad() {
		long counter = 0;
		unsigned int tCount = 5, iterCount = 1000000;

		auto incremetor = [&]()-> void {
			for (unsigned int i = 0; i < iterCount; ++i) {
				counter = counter + 1;
			}
		};

		std::vector<std::thread> jobs;
		for (unsigned int i = 0; i++ < tCount;)
			jobs.emplace_back(incremetor);
		std::for_each(jobs.begin(), jobs.end(), [](auto& T) {T.join(); });

		std::cout << "Actual: " << counter << ". Expected: " << iterCount * tCount << std::endl;
		assert(counter == (iterCount * tCount)), "Test failed";
	}

	void Bad_Integer() {
		Integer shared_resource(0);
		unsigned int tCount = 5, iterCount = 1000000;

		auto incremetor = [&]()-> void {
			for (unsigned int i = 0; i < iterCount; ++i) {
				shared_resource.increment();
			}
		};

		std::vector<std::thread> jobs;
		for (unsigned int i = 0; i++ < tCount;)
			jobs.emplace_back(incremetor);
		std::for_each(jobs.begin(), jobs.end(), [](auto& T) {T.join(); });

		std::cout << "Actual: " << shared_resource << ". Expected: " << iterCount * tCount << std::endl;
		assert(shared_resource.getValue() == (iterCount * tCount)), "Test failed";
	}

	void Bad_Integer_SharedPrt() {
		std::shared_ptr<Integer> shared_resource = std::make_shared<Integer>(0);
		unsigned int tCount = 5, iterCount = 1000000;

		auto incremetor = [&]()-> void {
			for (unsigned int i = 0; i < iterCount; ++i) {
				shared_resource->increment();
			}
		};

		std::vector<std::thread> jobs;
		for (unsigned int i = 0; i++ < tCount;)
			jobs.emplace_back(incremetor);
		std::for_each(jobs.begin(), jobs.end(), [](auto& T) {T.join(); });

		std::cout << "Actual: " << shared_resource->getValue() << ". Expected: " << iterCount * tCount << std::endl;
		// assert(shared_resource->getValue() == (iterCount * tCount)), "Test failed";
	}


	void MultithreadingTests()
	{
		std::shared_ptr<Base> ptr = std::make_shared<Derived>();

		std::cout << "Created a shared Derived (as a pointer to Base)\n"
			<< "  p.get() = " << ptr.get()
			<< ", p.use_count() = " << ptr.use_count() << std::endl;

		std::thread t1(worker, ptr);
		std::thread t2(worker, ptr);
		std::thread t3(worker, ptr);

		ptr.reset(); // release ownership from main
		std::cout << "Shared ownership between 3 threads and released\n"
			<< "ownership from main:\n"
			<< "  p.get() = " << ptr.get()
			<< ", p.use_count() = " << ptr.use_count() << std::endl;

		t1.join();
		t2.join();
		t3.join();

		std::cout << "All threads completed, the last one deleted Derived" << std::endl;
	}
}

namespace SharedPtr_Tests::PrivateConstructor {

	class Base {
	private:
		Base() {
			std::cout << "Base::Base()" << std::endl;
		}

	public:
		static Base* get() {
			return new Base();
		}
	};

	class Bar {
	public:
		static std::shared_ptr<Bar> create()
		{
			// return std::make_shared<Bar>(); // ERROR !!!!!!!!!
			return std::shared_ptr<Bar>(new Bar);
		}

	private:
		Bar() = default;
	};

	//-----------------------------------------------------------------------//

	void CreatePtrWithPrivateCtor() {
		Base* obj = Base::get();
		// std::shared_ptr<Base> obj = std::make_shared<Base>();

		// std::allocator<Base> allocator;
		// std::shared_ptr<Base> obj = std::allocate_shared<Base>(allocator);
	}

	void TryCallPrivateConstructor() {
		auto bar = Bar::create();
	}
}

namespace SharedPtr_Tests::CreateClassesToUseWithSharedPtr {

	class Value {
	private:
		inline static size_t counter{ 0 };
		std::string text;

	public:
		Value(const std::string& value = ""): text(value) {
			std::cout << "Value::Value(" << text << ")" << std::endl;
			Value::counter++;
		}

		Value(const Value& obj) {
			Value::counter++;
			this->text = obj.text;
			std::cout << "Copy constructor: Value::Value(" << text << ")" << std::endl;
		}

		virtual ~Value() {
			Value::counter--;
			std::cout << "~Value::Value(" << text << "). Objects remaining: " << Value::counter << std::endl;
		}

		void Info() {
			std::cout << Value::counter << " object created" << std::endl;
		}
	};

	//===========================================//

	void Test1() {
		std::shared_ptr<Value> value = std::make_shared<Value>("Test1");
		value->Info();

		std::shared_ptr<Value> value1 = std::make_shared<Value>("Test1");
		value1->Info();
	}
}

namespace SharedPtr_Tests::TESTS {

	void Size_Test() {
		std::shared_ptr<Integer> ptr = std::make_shared<Integer>(10);
		std::cout << sizeof(ptr) << std::endl;

		std::cout << "---------------------------\n";

		std::shared_ptr<Integer>&& ptr1 = std::move(ptr);

		std::cout << "---------------------------\n"; 
	}


    void InitVariableFromRef(std::shared_ptr<Integer>& ptr) {
        std::cout << "Uses: " << ptr.use_count() << std::endl;

        std::shared_ptr<Integer> var {ptr};
        std::cout << "Uses: " << ptr.use_count() << std::endl;
    }

	void Test() {
		auto ptr1 = std::make_shared<Integer>(111);

        std::cout << "Uses: " << ptr1.use_count() << std::endl;

        InitVariableFromRef(ptr1);

        std::cout << "Uses: " << ptr1.use_count() << std::endl;
	}

	void EmplaceToVector() {
		{
			std::vector<std::shared_ptr<Integer>> sharedIntegers;
			sharedIntegers.emplace_back(std::make_shared<Integer>(123));
		}
		{
			std::vector<std::shared_ptr<Integer>> sharedIntegers;
			sharedIntegers.emplace_back(new Integer(123));
		}
	}

	void Assign_Test() {
		auto ptr1 = std::make_shared<Integer>(10);
		auto ptr2 = std::make_shared<Integer>(22);
		ptr2 = ptr1;
		std::cout << "OK" << std::endl;
	}
}

namespace SharedPtr_Tests::Atomic {

	template<typename T>
	class ConcurrentStack {
	private:
		struct Node {
			T t;
			std::shared_ptr<Node> next;
		};

		// std::atomic_s

		//atomic_shared_ptr<Node> head;
	};

	void Test() {
	}
}

namespace SharedPtr_Tests::Aliasing {

	struct Object {
		int value{0};

		Object(int v) : value{ v } {
			std::cout << "Object(" << value << ")\n";
		}

		~Object() {
			std::cout << "~Object(" << value << ")\n";
		}
	};



	struct Base {
		Object m_data;

	public:
		template<typename ... Params>
		Base(Params&&... args) : m_data{ std::forward<Params>(args)... } {
		}
		
	};


	void Test() {
		std::shared_ptr<Base> obj1 { std::make_shared<Base>(42) };
		std::shared_ptr<Object> aliasing {std::shared_ptr<Object> { obj1, &obj1->m_data }};

		std::cout << obj1.get()->m_data.value << std::endl;
		std::cout << aliasing->value << std::endl;

		obj1.get()->m_data.value = 123;

		std::cout << obj1.get()->m_data.value << std::endl;
		std::cout << aliasing->value << std::endl;
	}
}

namespace SharedPtr_Tests::Arrays {

#if 0
	void CreateSharedPtrArray() {
		std::shared_ptr<Integer[]> numbers = std::make_shared<Integer[]>(3);

		for (int i = 0; i < 3; ++i) 
			numbers.get()[i] = i;
		for (int i = 0; i < 3; ++i) 
			std::cout << numbers.get()[i] << std::endl;
	}
#endif
}

namespace SharedPtr_Tests::CreateSharedPtr_PrivateConstructor {

    class Foo {
        std::string name{};

        explicit Foo(std::string n) : name{std::move(n)} {
            std::cout << "CTOR " << name << '\n';
        }

        ~Foo() {
            std::cout << "DTOR " << name << '\n';
        }

    public:
        static Foo *create_foo(std::string &&s) {
            return new Foo{move(s)};
        }

        static void destroy_foo(Foo *ptr) {
            delete ptr;
        }
    };

    std::shared_ptr<Foo> make_shared_foo(std::string &&s) {
        return {Foo::create_foo(move(s)), Foo::destroy_foo};
    }

    void Test() {
        auto foo = make_shared_foo("SomeData");

    }
}


void SharedPtr_Tests::TestAll() {
	// Simple_Test_0();
	// Simple_Test_1();
	// Simple_Test_2();
	// Simple_Test_3();
	// SimpleTest_Crush();

	// Count_Test();
	// Unique_Test();
	// Reset_Test();


	// Enable_Shared_From_This::EnableSharedFomThis_Good();
	// Enable_Shared_From_This::EnableSharedFomThis_Bad();
	
	// Enable_Shared_From_This::EnableSharedFromThis_Test_1();
	// Enable_Shared_From_This::EnableSharedFromThis_Test_2();
	
	// Enable_Shared_From_This::EnableSharedFromThis_Custom();

	// BAD_Usage_Test();
	// BAD_Usage_Test1();
	// BAD_Usage_Test2();
	// VariousTests();

	// DeleterTest::CustomDeleter_Test_1();
	// DeleterTest::CustomDeleter_Test_2();
	// DeleterTest::Get_Deleter_Test();

	// Swap_Test();
	// Get_Test();
	// Operator_Index();
	// Dynamic_Ppointer_Cast();

	// Multithreading::Bad();
	// Multithreading::Bad_Integer();
	// Multithreading::Bad_Integer_SharedPrt();
	// Multithreading::MultithreadingTests();

	// Allocator_Test_DEPRECATED();
	// Allocation::Allocate_Shared();
	// Allocation::Allocate_Shared_Integer();

	// PrivateConstructor::CreatePtrWithPrivateCtor();
	// PrivateConstructor::TryCallPrivateConstructor();

	// CreateClassesToUseWithSharedPtr::Test1();

	// ExceptionHandling::Exeption_DuringCreate_SharedPtr();
	// ExceptionHandling::Exeption_DuringCreate_SharedPtr2();

	// Arrays::CreateSharedPtrArray();

	// Atomic::Test();

	// Aliasing::Test();

	// TESTS::Test();
	// TESTS::EmplaceToVector();
	// TESTS::Assign_Test();
	// TESTS::Size_Test();

    CreateSharedPtr_PrivateConstructor::Test();
}