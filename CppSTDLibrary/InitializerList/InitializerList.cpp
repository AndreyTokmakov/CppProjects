//============================================================================
// Name        : InitializerList.cpp
// Created on  : 18.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Initializer list semantics src
//============================================================================

#include <iostream>          // std::cout
#include <numeric>
#include <string>            // std::string
#include <sstream>           // std::stringstream
#include <initializer_list>  // std::initializer_list
#include <utility>
#include <vector>

#include "InitializerList.h"

namespace
{
    struct Int
    {
        int value { 0 };

        Int(int v): value{v} {
           std::cout <<  "Int(" << value << ")\n";
        }

        Int(const Int& anInt): value { anInt.value }  {
            std::cout <<  "Int(const Int& anInt)(" << value << ")\n";
        }

        Int(Int&& anInt) noexcept: value { std::exchange(anInt.value, 0)}  {
            std::cout << "Int(Int&& anInt)(" << value << ")\n";
        }

        Int& operator=(const Int& anInt) {
            value = anInt.value;
            std::cout <<  "Int(const Int& anInt)(" << value << ")\n";
            return *this;
        }

        Int& operator=(Int&& anInt) noexcept {
            value = std::exchange(anInt.value, 0);
            std::cout <<  "Int(Int&& anInt)(" << value << ")\n";
            return *this;
        }

        ~Int() {
            std::cout <<  "~Int(" << value << ")\n";
        }
    };
}


namespace InitializerList
{
	template<class T> void print_list(std::initializer_list<T> il)
	{
		for (const T* it = begin(il); it != end(il); ++it)
			std::cout << ' ' << *it;
		std::cout << std::endl;
	}

	class MyClass {
	private:
		std::string str;

	public:
		MyClass(std::initializer_list<int> args) {
			std::stringstream ss;
			std::cout << "Size = " << args.size() << std::endl;
			for (auto it = args.begin(); it != args.end(); ++it) 
				ss << ' ' << *it;
			str = ss.str();
		}

		std::string GetStr() {
			return this->str;
		}
	};
 

	void PrintTest() {
		print_list({ 10,20,30 });
	}

	void Test1() {
		MyClass myobject{ 10, 20, 30 };
		std::cout << "myobject contains:" << myobject.GetStr() << std::endl;
	}
};

namespace InitializerList::InitializerList_FoldExpr
{
    template <typename First, typename... Args>
    auto sum(const First first, const Args... args) -> decltype(first) {
        const auto values = { first, args... };
        std::cout << typeid(values).name() << std::endl;
        return std::accumulate(values.begin(), values.end(), First{0});
    }

    void test()
    {
        int result = sum(1, 2, 3, 4, 5);
        std::cout << result << std::endl;
    }

}

namespace InitializerList::Caveats_and_Bugs:: Referencing_Local_Array
{
    /** Same as:
     std::initializer_list<int> wrong() {
        const int arr[] { 1, 2, 3, 4}
        return std::initializer_list<int>{arr, arr+4};
     }
     **/

    std::initializer_list<int> wrong() { // for illustration only!
        return { 1, 2, 3, 4};
    }

    void test()
    {
        // Warning: warning: returning temporary ‘initializer_list’
        //                   does not extend the lifetime of the underlying array [-Winit-list-lifetime]

        std::initializer_list<int> x = wrong();
    }
}


namespace InitializerList::Caveats_and_Bugs::Copying_Of_Elements
{

    void test()
    {
        {
            std::vector<Int> ints { Int{1}, Int {2}, Int {3}};
        }

        std::cout << std::string (180, '-') << std::endl;

        {
            std::vector<Int> ints;
            ints.reserve(3);
            ints.emplace_back(1);
            ints.emplace_back(2);
            ints.emplace_back(3);
        }
    }

}




// https://www.cppstories.com/2023/initializer_list_improvements/
void InitializerList::TestAll()
{
    // PrintTest();
    // Test1();
    // InitializerList_FoldExpr::test();

    // Caveats_and_Bugs::Referencing_Local_Array::test();
    //Caveats_and_Bugs::Copying_Of_Elements::test();
};