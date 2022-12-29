//============================================================================
// Name        : BitSet.h
// Created on  : 17.05.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : BitSet (custom implementation) src
//============================================================================

#include <iostream>
#include <cassert>

#include "BitSet.h"

namespace BitSet {

	template<typename T>
	void showbit_int(T x) {
		std::cout << x << "  ===>  ";
		for (int i = (sizeof(x) * 8) - 1; i >= 0; i--)
			std::cout << (x & (1u << i) ? '1' : '0');
		std::cout << std::endl;
	}
};

namespace BitSet::Tests {

	void MyAssert(const BitSet<>& set, int bit, bool value) {
		if (value == set.getBit(bit)) {
			std::cout << "OK\n";
		} else {
			std::cout << "False\n";
		}
	}

	void Complex_Test() {
		BitSet set;

		auto test = [&set](size_t bit, bool value) {
			set.setBit(bit, value);
			if (value != set.getBit(bit))
			{
				std::cout << "********* FAILED 1 ************\n";
			}
			if (false != set.getBit(bit + 1))
			{
				std::cout << "********* FAILED 2 ************\n";
			}
		};

		constexpr bool value = true;
		for (int i = 1; i <= 1024; ++i) {
			test(i, value);
		}
		std::cout << "OK\n";
	}

	void SimpleTest() 
	{
		{
			BitSet set;
			int bit = 3;
			MyAssert(set, bit, false);

			set.setBit(bit, true);
			MyAssert(set, bit, true);
		}


		{
			BitSet set;
			int bit = 13;
			MyAssert(set, bit, false);

			set.setBit(bit, true);
			MyAssert(set, bit, true);
		}
	}


	void Test1() {

		BitSet set;
		for (int i = 1; i < 64; ++i) {
			if (true == set.getBit(i))
			{
				std::cout << "Check 1 failed. i = " << i << std::endl;
				break;
			}

			set.setBit(i, true);

			if (false == set.getBit(i))
			{
				std::cout << "Check 2 failed. i = " << i << std::endl;
				break;
			}

			set.setBit(i, false);

			if (true == set.getBit(i))
			{
				std::cout << "Check 3 failed. i = " << i << std::endl;
				break;
			}

			
			if (true == set.getBit(i + 1))
			{
				std::cout << "Check 4 failed. i = " << i << std::endl;
				break;
			}
			if (i > 2 && true == set.getBit(i - 1))
			{
				std::cout << "Check 5 failed. i = " << i << std::endl;
				break;
			}
		}

		std::cout << "Done\n";
	}

}

void BitSet::TEST_ALL() 
{
	// Tests::Complex_Test();

	// Tests::SimpleTest();

	// Tests::Test1();

    auto bSet = BitSet();

    std::cout << sizeof(bSet) << std::endl;

}