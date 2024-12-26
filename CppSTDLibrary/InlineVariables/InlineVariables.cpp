//============================================================================
// Name        : InlineVariables.cpp
// Created on  : 11.09.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : BitSet src
//============================================================================

#include "InlineVariables.h"

#include <iostream>
#include <string>

namespace InlineVariables {

	class MyClass {
	public:
		static inline std::string name = "SomeTestName"; 
	};
	
	// NOTE: constexpr now implies inline

	class MyClass2 {
	public:
		inline static constexpr int value = 10;
		// same as
		static constexpr int value2 = 11;
	};
};

// -----------------------------------------------------------------------------------------// 

// #define LIB1_VERSION_2
#define LIB2_VERSION_2

namespace Lib1
{
#ifndef LIB1_VERSION_2
	inline namespace version_1
	{
		template<typename T>
		int test(T value) { return 1; }
	}
#endif
#ifdef LIB1_VERSION_2
	inline namespace version_2
	{
		template<typename T>
		int test(T value) { return 2; }
	}
#endif
}

namespace Lib2
{
	namespace version_1
	{
		template<typename T>
		int test(T value) { return 1; }
	}
#ifndef LIB2_VERSION_2
	using namespace version_1;
#endif
	namespace version_2
	{
		template<typename T>
		int test(T value) { return 2; }
	}
#ifdef LIB2_VERSION_2
	using namespace version_2;
#endif
}

namespace InlineNamespaces
{
	void Test()
    {
		// Uncomment #define LIB1_VERSION_2 to user another version
		std::cout << Lib1::test(1) << std::endl;
	}

	void Test2()
    {
		// Uncomment #define LIB1_VERSION_2 to user another version
		std::cout << Lib2::test(1) << std::endl;
	}
}



namespace InlineVariables::Inlining {

    struct MyClass {
        inline static std::string name { "qwerty" }; // OK since C++17
    };

    struct ThreadData {
        // unique in program
        inline static std::string gName { "global" };

        // unique name per thread
        inline static thread_local std::string name{"qwerty"};
    };


    void Test() {

    }
};


void InlineVariables::TestAll() {
	
	// std::cout << MyClass::name << std::endl;

	InlineNamespaces::Test();
	// InlineNamespaces::Test2();


    // Inlining::Test();
};
