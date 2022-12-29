//============================================================================
// Name        : Initialization.cpp
// Created on  : 09.11.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Initialization src
//============================================================================

#include <iostream>          // std::cout
#include <string>            // std::string
#include <string_view>           // std::stringstream
#include <initializer_list>  // std::initializer_list

#include "Initialization.h"

namespace Initialization {

	struct Object {
		int a{ 0 };
		int b{ 0 };
		int c{ 0 };
	};

	std::ostream& operator<<(std::ostream& stream, const Object& obj) {
		stream << "{" << obj.a << ", " << obj.b << ", " << obj.c << "}";
		return stream;
	}
	
};

namespace Initialization::AggregateInitialization {
	void Test() {
		{
			Object obj;
			std::cout << obj << std::endl;
		}
		{
			Object obj{ 1,2,3 };
			std::cout << obj << std::endl;
		}
		{
			Object obj{ .a = 3, .c = 1 };
			std::cout << obj << std::endl;
		}
	}
}


void Initialization::TestAll() {

	AggregateInitialization::Test();
};