//============================================================================
// Name        : UniquePtrTests.cpp
// Created on  : 23.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Unique PTR C++  src
//============================================================================

#include <iostream>
#include <string>
#include <fstream>
#include <string_view>

#include "../Integer/Integer.h"
#include "UniquePtr.h"

using String = std::string;
using CString = const String&;

namespace UniquePtr_Tests {

	class IntegerDeleter {
	public:
		void operator() (Integer* integer) {
			std::cout << "****** Calling delete for Integer(" << integer->getValue() << ")" << std::endl;
			delete integer;
		}
		void info() {
			std::cout << "Hello from IntegerDeleter" << std::endl;
		}
	};

	void func(const Integer& integer) {
		std::cout << "func() called" << std::endl;
		integer.printInfo();
	}

	class Object {
	private:
		String name;

	public:
		Object() : name("Empty") {
			std::cout << __FUNCTION__ << std::endl;
		}
		Object(CString n) : name(n) {

			std::cout << __FUNCTION__ << std::endl;
		}
		virtual ~Object() {}


	public:
		void setName(CString name) noexcept {
			this->name = name;
		}

		const String getName() const noexcept {
			return this->name;
		}

	};


	/////////////////////////////////////////////////////////////////////

	void MakeUnique_ObjectRequirements() {

		std::unique_ptr<Object> obj = std::make_unique<Object>("TTTT");
		std::cout << obj->getName() << std::endl;
	}

	void UniquePtr_Test1()
	{
		std::unique_ptr<Integer> p1 = std::make_unique<Integer>();
		if (p1) {
			p1->printInfo();
		}

		{
			std::unique_ptr<Integer> p2(std::move(p1));
			func(*p2);

			if (!p1) {
				std::cout << "p1 is NULL" << std::endl;
			}

			p1 = std::move(p2);
			std::cout << "destroying p2.. " << std::endl;
		}

		if (p1) {
			p1->printInfo();
		}
	}

	void Swap_Test_1() {
		std::unique_ptr<Integer> foo = std::make_unique<Integer>(10);
		std::unique_ptr<Integer> bar = std::make_unique<Integer>(20);

		std::cout << "foo = " << foo->getValue() << ". bar =  " << bar->getValue() << std::endl;

		foo.swap(bar);

		std::cout << "foo = " << foo->getValue() << ". bar =  " << bar->getValue() << std::endl;
	}


	void Swap_Test_2() {
		std::cout << "********************************* Test1 **************************************\n";

		{
			std::unique_ptr<Integer> foo = std::make_unique<Integer>(111);


			std::cout << "--------------------------------------------------------------\n";
			{
				std::unique_ptr<Integer> bar = std::make_unique<Integer>(222);
				std::swap(foo, bar);
			}
			std::cout << "--------------------------------------------------------------\n";
		}

		std::cout << "\n\n********************************* Test2 **************************************\n\n";

		{
			std::unique_ptr<Integer> foo = std::make_unique<Integer>(111);


			std::cout << "--------------------------------------------------------------\n";
			{
				std::unique_ptr<Integer> bar = std::make_unique<Integer>(222);
				foo = std::move(bar);
			}
			std::cout << "--------------------------------------------------------------\n";
		}
	}

	void Release_Test() {
		std::unique_ptr<Integer> auto_pointer = std::make_unique<Integer>();
		Integer* manual_pointer;

		std::cout << " ----->  Set to 10 " << std::endl;

		*auto_pointer = (int)10;

		manual_pointer = auto_pointer.release();

		std::cout << "manual_pointer points to " << *manual_pointer << std::endl;
		delete manual_pointer;
	}

	void Get_Test() {
		std::unique_ptr<Integer> foo;                // null
		std::unique_ptr<Integer> bar;                // null  null
		Integer* p = nullptr;                        // null  null  null

		foo = std::make_unique<Integer>(10); // (10)  null  null
		bar = std::move(foo);                    // null  (10)  null
		p = bar.get();                           // null  (10)  (10)
		*p = 20;                                 // null  (20)  (20)
		p = nullptr;                             // null  (20)  null

		foo = std::make_unique<Integer>(30); // (30)  (20)  null
		p = foo.release();                       // null  (20)  (30)
		*p = 40;                                 // null  (20)  (40)

		std::cout << "foo: ";
		if (foo) std::cout << *foo << '\n'; else std::cout << "(null)" << std::endl;

		std::cout << "bar: ";
		if (bar) std::cout << *bar << '\n'; else std::cout << "(null)" << std::endl;

		std::cout << "p: ";
		if (p) std::cout << *p << '\n'; else std::cout << "(null)" << std::endl;
		std::cout << '\n';

		delete p;   // the program is now responsible of deleting the object pointed to by p
					// bar deletes its managed object automatically
	}


	void Reset() {
		{
			std::unique_ptr<Integer> integer1 = std::make_unique<Integer>(111);

			std::cout << "Reseting std::unique_ptr<Integer>(111) to null" << std::endl;
			integer1.reset(nullptr);
			std::cout << "Done" << std::endl;
		}
		std::cout << "----------------- TEST2 --------------------" << std::endl;
		{
			std::unique_ptr<Integer> integer1 = std::make_unique<Integer>(111);
			std::unique_ptr<Integer> integer2 = std::make_unique<Integer>(222);

			std::cout << "Reseting std::unique_ptr<Integer>(111)" << std::endl;
			integer1.reset(integer2.release());

			std::cout << "Done" << std::endl;
		}
	}

	void Reset_vs_Move() {
		std::unique_ptr<Integer> integer1 = std::make_unique<Integer>(111);
		std::unique_ptr<Integer> integer2 = std::make_unique<Integer>(222);

		std::cout << "integer1 = std::move(integer2)" << std::endl;
		integer1 = std::move(integer2);
		std::cout << "Done" << std::endl;

	}
}

namespace UniquePtr_Tests::VariousTest {
	struct deleter
	{
		bool use_free;
		template<typename T>
		void operator()(T* p) const
		{
			if (use_free)
			{
				p->~T();
				std::free(p);
			}
			else {
				std::cout << "Deleter.." << std::endl;
				delete p;
			}
		}
	};

	void Test() {
		std::unique_ptr<int, deleter> p1((int*)std::malloc(sizeof(int)), deleter{ true });
		std::unique_ptr<int, deleter> p2;
		std::unique_ptr<int, deleter> p3;

		p2 = std::move(p1);  // OK
		std::cout << "1" << std::endl;

		p3.reset(p2.release());  // UNDEFINED BEHAVIOUR!
	}

	void Test2() {
		{
			std::cout << "Creating new Integer object with Deleter" << std::endl;
			std::unique_ptr<Integer, IntegerDeleter> up(new Integer(111), IntegerDeleter());

			std::cout << "--------------" << std::endl;
			up.reset(new Integer(222));  // calls deleter for the old one

			std::cout << "--------------" << std::endl;
			up.reset(new Integer(333));  // calls deleter for the old one
		}
		std::cout << "\n--------------------TEST 2  ---------------\n" << std::endl;
		{
			std::cout << "Creating new Integer object with NOO Deleter." << std::endl;
			std::unique_ptr<Integer> up(new Integer(333));

			std::cout << "--------------" << std::endl;
			up.reset(new Integer(444));  // calls deleter for the old one

			std::cout << "-------------." << std::endl;
			up.reset(nullptr);
		}
		std::cout << "\n--------------------TEST 3  ---------------\n" << std::endl;
		{
			std::unique_ptr<Integer, IntegerDeleter> int1(new Integer(111), IntegerDeleter());
			std::unique_ptr<Integer> int2 = std::make_unique<Integer>(222);
			int2.reset(int1.release());
		}

		std::cout << "\n--------------------TEST 3  ---------------\n" << std::endl;
		{
			std::unique_ptr<Integer, IntegerDeleter> int1(new Integer(111), IntegerDeleter());
			std::unique_ptr<Integer> int2 = std::make_unique<Integer>(222);
			int1.reset(int2.release());
		}
	}

	void handleInteger(std::unique_ptr<Integer> intVar) {
		intVar->printInfo();
	}

	void Use_After_Move() {
		std::unique_ptr<Integer> intVar = std::make_unique<Integer>(1);
		intVar->printInfo();
		handleInteger(std::move(intVar));
		intVar->printInfo(); // CRash!!!!!!!!!!!
	}

	void IF_Initialyzer() {
		if (auto value = std::make_unique<Integer>(5); value->getValue() > 10) {
			std::cout << value->getValue() << " is greater than 10" << std::endl;
		} else {
			std::cout << value->getValue() << " is smaller than 10" << std::endl;
		}
		std::cout << "Test done!" << std::endl;
	}

	void Make_Default() {
		// std::unique_ptr<Integer> intVar = std::make_unique_default_init();
	}
}


namespace UniquePtr_Tests::Applications {

	struct FileCloser {
		void operator()(std::fstream* file) {
			if (nullptr != file)
				file->close();
		}
	};

	void readFile_STD(const std::string& filePath) {
		std::fstream file(filePath, std::ios::in);
		if (false == file.is_open() || true == file.bad()) {
			std::cout << "Unable to open file";
			return;
		}
		
		std::string line;
		while (std::getline(file, line)) 
			std::cout << line << '\n';
		file.close();
	}

	void readFile(const std::string& filePath) {
		std::unique_ptr<std::fstream, FileCloser> file (new std::fstream(filePath, std::ios::in), FileCloser());
		if (false == file->is_open() || true == file->bad()) {
			std::cout << "Unable to open file";
			return;
		}

		std::string line;
		while (std::getline(*file, line))
			std::cout << line << '\n';
	}

	void FileReader_Deleter() {

		// readFile_STD(R"(D:\Temp\Folder_For_Testing\File_1.txt)");
		readFile(R"(D:\Temp\Folder_For_Testing\File_1.txt)");
	}
}

namespace UniquePtr_Tests::Deleters {


	void Deleter_Test1()
	{
		static constexpr auto my_custom_deleter = [](Integer* integer) {
			std::cout << "----> Calling delete for Integer object... " << std::endl;
			delete integer;
		};

		std::unique_ptr<Integer, decltype(my_custom_deleter)> integer(new Integer(123), my_custom_deleter);
		integer->printInfo();
	}

	void Deleter_Test2()
	{
		std::cout << "Creating new Integer object..." << std::endl;
		std::unique_ptr<Integer, IntegerDeleter> integer(new Integer(555), IntegerDeleter());
		integer->printInfo();
	}

	namespace Helper {
		template<typename Type, typename Arg, typename Deleter = std::default_delete<Type>>
        [[nodiscard]]
        std::unique_ptr<Type, Deleter> make_unique(Arg&& args, Deleter&& deleter = std::default_delete<Type>()) {
			return std::unique_ptr<Type, Deleter>(new Type(std::forward<Arg>(args)), std::forward<Deleter>(deleter));
		}
	}

	void Deleter_Test3()
	{

		std::cout << "Creating new Integer object..." << std::endl;
		std::unique_ptr<Integer, IntegerDeleter> integer = Helper::make_unique<Integer>(444, IntegerDeleter());
		integer->printInfo();
	}

	void Deleter_Release_vs_Reset() {
		std::cout << "----------------- Release test --------------------\n" << std::endl;
		{
			std::unique_ptr<Integer, IntegerDeleter> integer = Helper::make_unique<Integer>(444, IntegerDeleter());
			std::cout << "--> integer.release()" << std::endl;
			integer.release();
			std::cout << "Done" << std::endl;
		}
		std::cout << "----------------- Reset test --------------------\n" << std::endl;
		{
			std::unique_ptr<Integer, IntegerDeleter> integer = Helper::make_unique<Integer>(444, IntegerDeleter());
			std::cout << "--> integer.reset()" << std::endl;
			integer.reset();
			std::cout << "Done" << std::endl;
		}
	}

	void Reset_Deleter()
	{
		std::cout << "Creating new Integer object..." << std::endl;
		std::unique_ptr<Integer, IntegerDeleter> up(new Integer(123), IntegerDeleter());

		std::cout << "Replace owned Foo with a new Integer object..." << std::endl;
		up.reset(new Integer());  // calls deleter for the old one

		std::cout << "Release and delete the owned Integer object..." << std::endl;
		up.reset(nullptr);
	}

	void Get_Deleter_Test()
	{
		std::unique_ptr<Integer, IntegerDeleter> integer(new Integer(111), IntegerDeleter());
		IntegerDeleter& del = integer.get_deleter();
		del.info();
	}

	//----------------------------------------------------------------------------------------------//

	template <typename Type, typename... Args>
	std::shared_ptr<Type> getObject_SharedPtr(Args&& ... params) {
		return std::shared_ptr<Type>(new Type(std::forward<Args>(params)...), [](Type* obj) {
			std::cout << "Calling deleter\n";
			delete obj;
		});
	}

	void FabricMethod_Shared_WithDeleter() {
		auto integer = getObject_SharedPtr<Integer>(1);
	}

	//----------------------------------------------------------------------------------

	template <typename Type>
	struct Deleter {
		int id;

		void operator()(Type* ptr) const {
			std::cout << "Deleting Object of id = " << id << "\n";
			delete ptr;
		}
	};

	template <typename Type, typename... Args>
	auto getObject_UniquePtr(Args&& ... params) {
		int some_id{ 222 };
		return std::unique_ptr<Type, Deleter<Type>>(
			new Type(std::forward<Args>(params)...), Deleter<Type>{some_id});
	}

	void FabricMethod_WithDeleter() {
		auto integer = getObject_UniquePtr<Integer>(1);
	}
}


namespace UniquePtr_Tests::Arrays {

	void UniquePtr_Array() {
		std::unique_ptr<Integer[]> numbers = std::make_unique<Integer[]>(10);

		for (int i = 0; i < 10; ++i) {
			numbers.get()[i] = i;
		}

		for (int i = 0; i < 10; ++i) {
			std::cout << numbers.get()[i] << std::endl;
		}
	}
}


namespace UniquePtr_Tests::TESTS
{
	void UniquePtr_Array()
    {
		std::unique_ptr<int[]> numbers = std::make_unique<int[]>(10);

		for (int i = 0; i < 10; ++i) {
			numbers.get()[i] = i;
		}

		for (int i = 0; i < 10; ++i) {
			std::cout << numbers.get()[i] << std::endl;
		}
	}
}


namespace UniquePtr_Tests::Size_Depending_of_Params {

    struct Deleter {
        void operator()(std::FILE* f) {
            std::fclose(f);
        }
    };

    void deleter_func(std::FILE* f) {
        std::fclose(f);
    }

    void SizeTest()
    {
        auto deleter = [](auto x) { delete x; };

        std::unique_ptr<std::FILE, decltype(deleter)> ptr{std::fopen("foo.txt", "b"), deleter};
        static_assert(sizeof(ptr) == 8);

        std::unique_ptr<std::FILE, Deleter> ptr2{std::fopen("foo.txt", "b")};
        static_assert(sizeof(ptr2) == 8);

        std::unique_ptr<std::FILE, decltype(&deleter_func)> ptr3{std::fopen("foo.txt", "b"), deleter_func};
        static_assert(sizeof(ptr3) == 16);
    }
}



void UniquePtr_Tests::TestAll(){
	// UniquePtr_Test1();

	// Swap_Test_1();
	// Swap_Test_2();

	// Release_Test();
	// Reset();
	// Reset_Deleter();
	// Reset_vs_Move();

	// Deleter_Release_vs_Reset();

	// Get_Test();

	// VariousTest::Test();
	// VariousTest::Test2();
	// VariousTest::Use_After_Move();

	// VariousTest::IF_Initialyzer();

	// Applications::FileReader_Deleter();


	// Deleters::Deleter_Test1();
	// Deleters::Deleter_Test2();
	// Deleters::Deleter_Test3();
	// Deleters::Get_Deleter_Test();
	// Deleters::MakeUnique_ObjectRequirements();


	// Deleters::FabricMethod_Shared_WithDeleter();
	// Deleters::FabricMethod_WithDeleter();

	// Arrays::UniquePtr_Array();

	// TESTS::UniquePtr_Array();

    Size_Depending_of_Params::SizeTest();
};