//============================================================================
// Name        : ConsoleInOut.cpp
// Created on  : 28.04.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ Stringstream src
//============================================================================

#define _CRT_SECURE_NO_WARNINGS

#include <cinttypes>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <iomanip>
#include <locale>
#include <limits>
#include <map>
#include <array>
#include <ranges>
// #include <format>

#include "ConsoleInOut.h"

namespace Color {
	enum Code {
		FG_RED = 31,
		FG_GREEN = 32,
		FG_BLUE = 34,
		FG_DEFAULT = 39,
		BG_RED = 41,
		BG_GREEN = 42,
		BG_BLUE = 44,
		BG_DEFAULT = 49
	};

	class Modifier {
		Code code;
	public:
		Modifier(Code pCode) : code(pCode) {}
		friend std::ostream&
			operator<<(std::ostream& os, const Modifier& mod) {
			return os << "\033[" << mod.code << "m";
		}
	};
}


namespace ConsoleInOut
{
	void Simple_Read()
	{
		std::string input;
		while (true) {
			std::cout << "Enter text: ";
			std::getline(std::cin, input);
			std::cout << "Text: " << input << std::endl;
		}
	}

	void Simple_Read_2()
	{
		std::string a, b;
		std::cin >> a >> b;
		std::cout << a << std::endl;
		std::cout << b << std::endl;
	}

	void ReadLines()
	{
		std::string line;
		std::getline(std::cin, line);
		//std::cout << "Line: " << line << std::endl;
		std::getline(std::cin, line);
		std::cout << "Line: " << line << std::endl;
	}

	void Read_4_Str() {
		std::string str;
		std::vector<std::string> vec;

		std::cout << "Enter 4 sentense:";
		while ((std::getline(std::cin, str, ';')) && (vec.size() < 4))
		{
			std::cout << "str   " << str << std::endl;
			vec.push_back(str);
		}
	}

	void Read_And_Convert_To_Ints()
	{
		std::string mystr;
		float price = 0;
		int quantity = 0;

		std::cout << "Enter price: ";
		std::getline(std::cin, mystr);
		std::stringstream(mystr) >> price;

		std::cout << "Enter quantity: ";
		std::getline(std::cin, mystr);
		std::stringstream(mystr) >> quantity;

		std::cout << "Total price: " << price * quantity << std::endl;
	}

	void Gcount_Test()
	{
		char str[20];
		std::cout << "Please, enter a word: ";
		std::cin.getline(str, 20);
		std::cout << std::cin.gcount() << " characters read: " << str << '\n';
	}

	void Scanf_Test() {

		[[maybe_unused]] int n;
		std::int64_t lld = 0;
		[[maybe_unused]] char c;
		[[maybe_unused]] float f;
		[[maybe_unused]] double l5;

		/*
		scanf("%d %lld %c %f %lf", &n, &lld, &c, &f, &lf);

		std::cout << n << std::endl;
		std::cout << lld << std::endl;
		std::cout << c << std::endl;
		printf("%3.3f\n", f);
		printf("%5.9f\n", lf);
		*/

		scanf("%lf", &l5);
		printf("%5.9lf\n", l5);
	}

	void OutputFormat() {
		std::cout << std::setw(5) << 0.2 << std::setw(10) << 123456 << std::endl;
		std::cout << std::setw(5) << 0.12 << std::setw(10) << 123456789 << std::endl;
	}

	void OutputFormat2()
	{
		//std::cout.imbue(std::locale("en_US.utf8"));

		std::cout << "Left fill:\n" << std::left << std::setfill('*')
			<< std::setw(12) << -1.23 << '\n'
			<< std::setw(12) << std::hex << std::showbase << 42 << std::endl;
			//<< std::setw(12) << std::put_money(123, true) << "\n\n";

		/*

			std::cout << "Internal fill:\n" << std::internal
			<< std::setw(12) << -1.23 << '\n'
			<< std::setw(12) << 42 <<  std::endl;
			//<< std::setw(12) << std::put_money(123, true) << "\n\n";

			std::cout << "Right fill:\n" << std::right
			<< std::setw(12) << -1.23 << '\n'
			<< std::setw(12) << 42 << std::endl;
			//<< std::setw(12) << std::put_money(123, true) << '\n';
			*/
	}

	void Fill() {

		std::cout << std::left << std::setfill('*') << std::setw(20) << "TEXT" << std::endl;
		std::cout << std::right << std::setfill('*') << std::setw(20) << "TEXT" << std::endl;
		std::cout << std::internal << std::setfill('*') << std::setw(20) << "TEXT" << std::endl;
	}

	void PrintHex() {
		std::cout << std::setw(12) << std::hex << std::showbase << 42 << std::endl;
		std::cout << std::setw(12) << std::hex << std::showbase << 100 << std::endl;
	}

    void PrintInt_asHEX_asOCT() {
        int myInt = 123;

        std::cout << "Decimal: " << myInt << std::endl;

        std::cout.setf(std::ios::hex, std::ios::basefield);
        std::cout << "Hexadecimal: " << myInt << std::endl;

        std::cout << "Octal: " << resetiosflags(std::ios::basefield)
                  <<  setiosflags(std::ios::oct) << myInt << std::endl;
    }

	void SetPrecision() {
		double f = 3.14159;
		std::cout << std::setprecision(2) << f << std::endl;
		std::cout << std::setprecision(3) << f << std::endl;
		std::cout << std::setprecision(5) << f << std::endl;
		std::cout << std::setprecision(9) << f << std::endl;
		std::cout << std::fixed;
		std::cout << std::setprecision(5) << f << std::endl;
		std::cout << std::setprecision(9) << f << std::endl;
	}

    void PrintFloats() {
        double myFloat = 1234.123456789012345;
        int defaultPrecision = std::cout.precision(); // == 2

        std::cout << "Default precision: " << myFloat << std::endl;
        std::cout.precision(4);
        std::cout << "Modified precision: " << myFloat << std::endl;
        std::cout.setf(std::ios::scientific, std::ios::floatfield);
        std::cout << "Modified precision & scientific format: " << myFloat << std::endl;
/* back to default */
        std::cout.precision(defaultPrecision);
        std::cout.setf(std::ios::fixed, std::ios::floatfield);
        std::cout << "Default precision & fixed format:  " << myFloat << std::endl;
    }

	void Showpos() {
		int a = 10;
		int b = -120;

		std::cout << std::showpos << a << std::endl;
		std::cout << std::showpos << b << std::endl;
	}

	void TEST() {
		double A = 100.345;
		double B = 2006.008;
		double C = 2331.41592653498;

		std::cout << std::hex << std::showbase << (int)A << std::endl;
		std::cout << std::fixed << std::setw(15) << std::setprecision(2) << std::setfill('_') << std::right << std::showpos << B << std::endl;

		// LINE 3
		std::cout << std::scientific << std::uppercase << std::noshowpos << std::setprecision(9); // formatting
		std::cout << C << std::endl; // actual printed part
	}


	void Precision() {
		std::cout.precision(std::numeric_limits<double>::max_digits10);
		std::cout << (0.1 + 0.2) + 0.3 << " != " << 0.1 + (0.2 + 0.3) << '\n';
	}

    void FillLeftSideWithDots() {
        std::cout << std::right << std::setfill('.') << std::setw(30) << 500 << " pcs" << std::endl;
        std::cout << std::right << std::setfill('.') << std::setw(30) << 3000 << " pcs" << std::endl;
        std::cout << std::right << std::setfill('.') << std::setw(30) << 24500 << " pcs" << std::endl;
    }

    void FillWithSpaces_RightAdjusted() {
        std::cout << std::left << std::setfill('.') << std::setw(20) << "Flour"
             << std::right << std::setfill('.') << std::setw(20) << 0.7 << " kg" << std::endl;
        std::cout << std::left << std::setfill('.') << std::setw(20) << "Honey"
             << std::right << std::setfill('.') << std::setw(20) << 2 << " Glasses" << std::endl;
        std::cout << std::left << std::setfill('.') << std::setw(20) << "Noodles"
             << std::right << std::setfill('.') << std::setw(20) << 800 << " g" << std::endl;
        std::cout << std::left << std::setfill('.') << std::setw(20) << "Beer"
             << std::right << std::setfill('.') << std::setw(20) << 20 << " Bottles" << std::endl;
    }


    void Padding_With_Zeros() {

        for (int i: {1, 12, 123, 1234, 12345, 123456, 1234567})
            std::cout << std::setfill('0') << std::setw(10) << i << std::endl;
    }

    void Time_And_Money() {
        long double specialOffering = 9995;

        std::cout.imbue(std::locale("en_US.UTF-8"));
        std::cout << std::showbase << std::put_money(specialOffering) << std::endl;
        std::cout.imbue(std::locale("de_DE.UTF-8"));
        std::cout << std::showbase << std::put_money(specialOffering) << std::endl;
        std::cout.imbue(std::locale("ru_RU.UTF-8"));
        std::cout  << std::showbase << std::put_money(specialOffering) << std::endl;
    }
}

namespace ConsoleInOut
{
    constexpr size_t Rows = 5;
    const std::map<std::string, std::array<double, Rows>> productOrders
    {
        { "apples", {100, 200, 50.5, 30, 10}},
        { "bananas", {80, 10, 100, 120, 70}},
        { "carrots", {130, 75, 25, 64.5, 128}},
        { "tomatoes", {70, 100, 170, 80, 90}}
    };

    template <typename T>
    constexpr size_t MaxKeyLength(const std::map<std::string, T>& tbl) {
        size_t maxLen = 0;
        for (const auto& [key, val] : tbl)
            if (key.length() > maxLen)
                maxLen = key.length();
        return maxLen;
    }

    const int32_t colLength = static_cast<int32_t>(MaxKeyLength(productOrders) + 2);

    void PrintTable()
    {
        // headers:
        for (const auto& [key, val] : productOrders)
            std::cout << std::setw(colLength) << key;
        std::cout << '\n';

        // values:
        for (size_t i = 0; i < Rows; ++i) {
            for (const auto& [key, val] : productOrders)
                std::cout << std::setw(colLength) << std::fixed << std::setprecision(2) << val[i];
            std::cout << '\n';
        }
    }

    void PrintTable2() // FORMAT
    {
        /*
        for (const auto& name : std::views::keys(productOrders))
            std::cout << std::format("{:*>{}}", name, colLength);
        std::cout << '\n';

        for (size_t i = 0; i < NumRows; ++i) {
            for (const auto& values : std::views::values(productsToOrders)) {
                std::cout << std::format("{:>{}.2f}", values[i], ColLength);
            }
            std::cout << '\n';
        }
        */
    }
}

namespace ConsoleInOut::Experiments
{
    void ReadInputTestData()
    {
        std::string line;
        std::getline(std::cin, line);
        int counter = atoi(line.data());

        std::string_view stringView;
        while (counter--) {
            std::getline(std::cin, line);
            stringView = line;

            auto pos = stringView.find(' ');
            if (std::string::npos == pos)
                continue;

            int a = atoi(stringView.substr(0, pos).data());
            int b = atoi(stringView.substr(pos + 1, stringView.size() - pos - 1).data());

            // std::cout << a << " " << b << std::endl;
            std::cout << a + b << std::endl;
        }

    }
}


/** TEST **/
void ConsoleInOut::TestAll()
{
	// Simple_Read();
	// Simple_Read_2();
	// ReadLines();
	// Read_4_Str();
	// Read_And_Convert_To_Ints();
	// Gcount_Test();
	// Scanf_Test();

    // Padding_With_Zeros();

	// PrintHex();
    // PrintInt_asHEX_asOCT();

	// SetPrecision();
    // PrintFloats();

	// OutputFormat();
	// OutputFormat2();

	// Showpos();

	// Precision();

    // Fill();
    // FillLeftSideWithDots();
    // FillWithSpaces_RightAdjusted();

    // Time_And_Money();

    // ConsoleInOut::PrintTable();
    // ConsoleInOut::PrintTable2();

    // TEST();

    Experiments::ReadInputTestData();
};

