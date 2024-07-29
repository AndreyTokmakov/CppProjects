//============================================================================
// Name        : Array.cpp
// Created on  : 29.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Array container testing
//============================================================================

#include <iostream>
#include <string>
#include <string_view>

#include <algorithm>
#include <array>

#include "../Helpers/Integer.h"
#include "Array.h"

namespace Array {

	auto printer_coma = [](const auto& val) {
		std::cout << val << ", ";
	};

	auto print_array = [](const auto& array) {
		for (const auto& enrty : array)
			printer_coma(enrty);
	};

	auto print_array_ex = [](const auto& array,
		std::string_view text,
		std::string_view postfix = "\n") {
		std::cout << text;
		print_array(array);
		std::cout << postfix << std::endl;
	};

	void Tests() {
        [[maybe_unused]]
		std::array<int, 4> myarray = { 8, 6, 4, 1 };

        [[maybe_unused]]
		std::array<int, 4> myarray2{ 4, 3, 2, 1 };

		print_array_ex(myarray, "Array1: ");
		print_array_ex(myarray, "Array2: ");
	}

	void AT_Test() {
		std::array<int, 5> ints{ 5, 4, 3, 2, 1 };

		print_array_ex(ints, "ints: ");

		std::cout << "ints.at(1) = " << ints.at(1) << std::endl;

		std::cout << "Set ints[1] to 7" << std::endl;
		ints.at(1) = 7;

		std::cout << "ints.at(1) = " << ints.at(1) << std::endl;

		print_array_ex(ints, "\nints: ");

	}

	void AT_Test_Error() {
		std::array<int, 5> ints{ 5, 4, 3, 2, 1 };

		print_array_ex(ints, "ints: ");
		try {
			ints.at(8) = 15;
		}
		catch (const std::exception& exc) {
			std::cout << exc.what() << std::endl;
		}
	}

	void Operator_OutOfBounds() {
		std::array<int, 5> ints{ 5, 4, 3, 2, 1 };

		ints[8] = 15; 
	
	}

	void Sort() {
		std::array<int, 5> ints{ 5, 4, 3, 2, 1 };

		print_array_ex(ints, "ints (Before sort): ");
		std::sort(ints.begin(), ints.end());
		print_array_ex(ints, "ints (After  sort): ");
	}

	void Compare() {
		std::array<int, 5> a { 1,2,3 };
		std::array<int, 5> b { 1,2,3 };
		std::array<int, 5> c { 3,2,3 };

		std::cout << std::boolalpha << (a == b) << std::endl;
		std::cout << std::boolalpha << (a > c) << std::endl;
		std::cout << std::boolalpha << (c > b) << std::endl;
	}

	void MultidimentionalArrays() {
		unsigned int x = 10;
		unsigned int y = 5;
		int **data = new int*[x];

		for (unsigned int i = 0; i < x; ++i) {
			data[i] = new int[y];
		}

		for (unsigned int i = 0; i < x; i++) {
			for (unsigned int n = 0; n < y; n++) {
				data[i][n] = i * n;
			}
		}

		//std::cout << data[1][1] << std::endl;
		//std::cout << data[1][1] << std::endl;

		for (unsigned int i = 0; i < x; i++) {
			for (unsigned int n = 0; n < y; n++) {
				std::cout << data[i][n] << "  ";
			}
			std::cout << std::endl;
		}

		for (unsigned int i = 0; i < x; ++i) {
			delete[] data[i];
		}
		delete[] data;
	}

    void ToArray()
    {
        {
            // returns `std::array<char, 4>`
            const std::array data = std::to_array("foo");
            std::cout << "std::array<" << typeid(data.front()).name() << ", " << data.size() << ">\n";
        }

        {
            // returns `std::array<int, 3>`
            const std::array data = std::to_array<int>({1, 2, 3});
            std::cout << "std::array<" << typeid(data.front()).name() << ", " << data.size() << ">\n";
        }

        {
            // returns `std::array<int, 3>`
            int a[] = {1, 2, 3};
            const std::array data = std::to_array(a);
            std::cout << "std::array<" << typeid(data.front()).name() << ", " << data.size() << ">\n";
        }
    }

	void TEST_ALL()
	{
		// MultidimentionalArrays();

		// Array::Tests();

		// Array::AT_Test();
		// Array::AT_Test_Error();

		// Array::Operator_OutOfBounds();

		// Array::Sort();

		// Array::Compare();

        ToArray();
	}
}
