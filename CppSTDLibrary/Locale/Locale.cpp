//============================================================================
// Name        : Locale.h
// Created on  : 15.05.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Locale  src
//============================================================================

#include <iostream>
#include <locale>
#include "Locale.h"

namespace Locale
{
	void IsSpace()
	{
		int i = 0;
		char str[] = "Example sentence to test isspace\n", c;
		while (str[i]) {
			c = str[i++];
			if (isspace(c)) 
				c = '\n';
			std::cout << c;
		}
	}

	void IsCntrl()
	{
		int i = 0;
		char str[] = "first line \n second line \n";
		while (!iscntrl(str[i]))
		{
			std::cout << str[i];
			i++;
		}
	}

	void IsUpper_ToLower()
	{
		int i = 0;
		std::string str = "TeSt-TeSt";
		std::cout << str << "  --->  ";
		for (auto& c : str) {
			if (isupper(c))
				c = tolower(c);
		}
		std::cout << str << std::endl;
	}

	void IsLower_ToUpper()
	{
		int i = 0;
		std::string str = "test-test";
		std::cout << str << "  --->  ";
		for (auto& c: str) {
			if (islower(c))
				c = toupper(c);
		}
		std::cout << str << std::endl;
	}

	void Get_Locale()
	{
		std::cout << "User-preferred locale setting is " << std::locale("").name().c_str() << '\n';
		// on startup, the global locale is the "C" locale

		std::cout << 1000.01 << '\n';
		// replace the C++ global locale as well as the C locale with the user-preferred locale

		std::locale::global(std::locale(""));
		// use the new global locale for future wide character output

		std::cout.imbue(std::locale());

		// output the same number again
		std::cout << 1000.01 << '\n';
	}

	void Get_Currency_Facet()
	{
		std::locale loc = std::locale(""); // user's preferred locale
		std::cout << "Your currency string is "
			<< std::use_facet<std::moneypunct<char, true>>(loc).curr_symbol() << '\n';
	}
};

void Locale::TestAll()
{
	// IsSpace();
	// IsCntrl();
	// IsUpper_ToLower();
	// IsLower_ToUpper();

	// Get_Locale();
	Get_Currency_Facet();
};