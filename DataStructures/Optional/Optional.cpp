//============================================================================
// Name        : Optional_Custom.cpp
// Created on  : 13.08.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Optinal custom src
//============================================================================

#include <optional>

#include "../Utilities/Integer.h"
#include "Optional.h"

namespace Optional {

	void Create() {
		std::cout << "-------------------------------- Test (STD)-----------------------------\n\n";
		{
			std::optional<Integer> integer(Integer(11111));
		}
		std::cout << "\n-------------------------------- Test1-----------------------------\n\n";
		{
			Optional<Integer> integer(Integer(11111));
		}
		/*
		std::cout << "\n\n\n-------------------------------- Test2-----------------------------\n";
		{
			Optional<Utilities> integer(22222);
		}
		*/
	}

	void Create_Null() {

		std::cout << "-------------------------------- std::optional -----------------------------\n";
		{
			std::optional<Integer> integer;
		}

		std::cout << "\n\n\n-------------------------------- Optional [Custom] -----------------------------\n";
		{
			Optional<Integer> integer;
		}
	}

	void Bool_Operator() {

		std::cout << "-------------------------------- std::optional -----------------------------\n";
		{
			std::optional<Integer> integer;
			if (integer) {
				std::cout << "Has some value " << std::endl;
			}
			else {
				std::cout << "Empty" << std::endl;
			}

			std::optional<Integer> integer1{ 12345 };
			if (integer1) {
				std::cout << "Has some value " << std::endl;
			}
			else {
				std::cout << "Empty" << std::endl;
			}
		}

		std::cout << "\n\n\n-------------------------------- Optional [Custom] -----------------------------\n";
		{
			Optional<Integer> integer;
			if (integer) {
				std::cout << "Has some value " << std::endl;
			}
			else {
				std::cout << "Empty" << std::endl;
			}

			Optional<Integer> integer1{ 12345 };
			if (integer1) {
				std::cout << "Has some value " << std::endl;
			}
			else {
				std::cout << "Empty" << std::endl;
			}
		}
	}

	void Assignment() {
		std::cout << "-------------------------------- std::optional -----------------------------\n";
		{
			std::optional<Integer> integer = Integer(12345);
		}

		std::cout << "\n\n\n-------------------------------- Optional [Custom] -----------------------------\n";
		{
			Optional<Integer> integer = Integer(12345);
		}
	}
};


void Optional::TEST_ALL() {

	// Create();
	// Create_Null();

	Bool_Operator();

	// Assignment();
};
