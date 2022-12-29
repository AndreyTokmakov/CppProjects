//============================================================================
// Name        : NumericLimits.cpp
// Created on  : 02.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Numeric limits src
//============================================================================

#include "NumericLimits.h"

#include <iostream>
#include <type_traits>
#include <limits>
#include <numeric>

namespace NumericLimits {

	/*
	template<class T>
	typename std::enable_if<std::numeric_limits<T>::is_modulo>::type check_overflow()
	{
		std::cout << "\nmax value is " << std::numeric_limits<T>::max() << '\n'
			<< "min value is " << std::numeric_limits<T>::min() << '\n'
			<< "max value + 1 is " << std::numeric_limits<T>::max() + 1 << '\n';
	}*/



	void Check_Overflow()
	{
		// check_overflow<int>();
		// check_overflow<unsigned long>();
		// check_overflow<float>(); // compile-time error, not a modulo type

		std::cout << std::boolalpha << std::numeric_limits<int>::is_modulo << std::endl;
	}

	
	void Print_Limits() {
		unsigned short u_short_max = std::numeric_limits<unsigned short>::max();
		unsigned short u_short_min = std::numeric_limits<unsigned short>::min();

		std::cout << "Minimum value for Unsingned SHORT: " << u_short_min << std::endl;
		std::cout << "Maximum value for Unsingned SHORT: " << u_short_max << std::endl << std::endl;

		std::cout << "Minimum value for INT: " << std::numeric_limits<int>::min() << std::endl;
		std::cout << "Maximum value for INT: " << std::numeric_limits<int>::max() << std::endl << std::endl;

		std::cout << "Minimum value for LONG: " << std::numeric_limits<long>::min() << std::endl;
		std::cout << "Maximum value for LONG: " << std::numeric_limits<long>::max() << std::endl << std::endl;

		std::cout << "Minimum value for Unsingned LONG: " << std::numeric_limits<unsigned long>::min() << std::endl;
		std::cout << "Maximum value for Unsingned LONG: " << std::numeric_limits<unsigned long>::max() << std::endl << std::endl;

		std::cout << "Minimum value for LONG LONG: " << std::numeric_limits<long long>::min() << std::endl;
		std::cout << "Maximum value for LONG LONG: " << std::numeric_limits<long long>::max() << std::endl << std::endl;

		std::cout << "Minimum value for Unsingned LONG LONG: " << std::numeric_limits<unsigned long long>::min() << std::endl;
		std::cout << "Maximum value for Unsingned LONG LONG: " << std::numeric_limits<unsigned long long>::max() << std::endl << std::endl;
	}

	void TEST() {
		std::cout << std::boolalpha;
		std::cout << "int is signed: "        << std::numeric_limits<int>::is_signed << std::endl;
		std::cout << "Non-sign bits in int: " << std::numeric_limits<int>::digits << std::endl;
		std::cout << "int has infinity: "     << std::numeric_limits<int>::has_infinity << std::endl;
	}
};


namespace NumericLimits {
	void TestAll() {
		// Check_Overflow();

		// Print_Limits();

		// TEST();

		std::cout << std::gcd(12, 8) << std::endl;
	}
};

