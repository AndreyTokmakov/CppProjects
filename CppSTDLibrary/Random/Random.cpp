//============================================================================
// Name        : Random.h
// Created on  : 21.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Random src
//============================================================================

#define _CRT_SECURE_NO_WARNINGS

#include "Random.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <random>
#include <cassert>

namespace Random {

	void Random_INT_UniformDistribution()
    {
		std::random_device rd{};
		auto mtgen = std::mt19937 { rd() };
		auto ud = std::uniform_int_distribution<>{ 1, 6 };

		for (auto i = 0; i < 20; ++i) {
			int number = ud(mtgen);
			std::cout << number << "  ";
		}
	}


	void SimpleRandomNumbers() {
		auto mtgen = std::mt19937{};
		for (auto i = 0; i < 10; ++i)
			std::cout << mtgen() << std::endl;
	}
}


namespace Random::UniformRealDistribution {

	void test()
	{
		std::random_device rd;  //Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_real_distribution<> distribution(1.0, 2.0);
		for (int n = 0; n < 10; ++n) {
			std::cout << distribution(gen) << ' ';
		}
		std::cout << '\n';
	}
}


namespace Random::DiscardBlockEngine {

	void test()
	{
		std::vector<uint32_t> raw(10), filtered(10);
		std::discard_block_engine<std::mt19937, 3, 2> g2;
		std::mt19937 g1 = g2.base();
		std::generate(raw.begin(), raw.end(), g1);
		std::generate(filtered.begin(), filtered.end(), g2);

		assert(raw[0] == filtered[0]);
		assert(raw[1] == filtered[1]);
		assert(raw[3] == filtered[2]);
		assert(raw[4] == filtered[3]);
	}
}



namespace Random::Strings {

	std::string randomString(int size = 16)
    {
		std::random_device rd{};
		auto mtgen = std::mt19937 {rd()};
		auto ud = std::uniform_int_distribution<> {(int)'A', (int)'Z'};

		std::string str;
		str.reserve(size);
		while (size-- > 0)
			str.push_back(static_cast<char>(ud(mtgen)));
		return str;
	}


	void Test() {
		auto str = randomString();
		std::cout << str << std::endl;

		str = randomString(32);
		std::cout << str << std::endl;

		str = randomString(64);
		std::cout << str << std::endl;
	}
}

void GenerateNumbersInRange() {
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> distribution(-0.1, 0.1);
    for (int n = 0; n < 10; ++n) {
        std::cout << distribution(gen) << ' ';
    }
}

void Random::TestAll()
{
	// Random::SimpleRandomNumbers();

	Random::Random_INT_UniformDistribution();

	// UniformRealDistribution::test();

	// Strings::Test();

    // GenerateNumbersInRange();

}
