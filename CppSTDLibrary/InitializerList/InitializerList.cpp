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

#include "InitializerList.h"

namespace InitializerList
{
	template<class T> void print_list(std::initializer_list<T> il) {
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


namespace InitializerList::Experiments
{
    std::initializer_list<int> wrong() { // for illustration only!
        return { 1, 2, 3, 4};
    }


    void BAD() {
        std::initializer_list<int> x = wrong();
    }
}


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


void InitializerList::TestAll()
{
    // PrintTest();
    // Test1();

    /*
    std::initializer_list list{1,2,3,4,5};
    for (const auto v : list) {
        std::cout << v << std::endl;
    }*/

    InitializerList_FoldExpr::test();

    // Experiments::BAD();
};