//============================================================================
// Name        : Literals.cpp
// Created on  : 24.09.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Literals src
//============================================================================

#include <iostream>
#include <locale>
#include <chrono>
#include <cassert>
#include <complex>
#include <bitset>
#include <cstring>
#include "Literals.h"

namespace Literals::Custom_Literals_Tests
{
	namespace units
	{
		enum class Unit {
			kilogram,
			liter,
			meter,
			piece
		};


		namespace unit_traits
		{
			using namespace units;

			template <typename T>
			class Traits {
			public:
				static inline const std::string name(units::Unit unit) {
					return "";
				}
			};

			template <>
			class Traits<units::Unit> {
			public:
				static inline const std::string name(units::Unit unit) {
					switch (unit) {
					case units::Unit::kilogram:
						return "kilogram";
					case units::Unit::liter:
						return "liter";
					case units::Unit::meter:
						return "meter";
					case units::Unit::piece:
						return "piece";
					default:
						return "unknown";
					}
				}
			};
		}

		template <Unit U>
		class quantity {
		private:
			const double amount;

		public:
			constexpr explicit quantity(double const a) : amount(a) {
			}
			explicit operator double() const {
				return amount;
			}

			// template <Unit U>
			friend constexpr quantity<U> operator+(const quantity<U> &q1, const quantity<U> &q2);

			// template <Unit U>
			friend constexpr quantity<U> operator-(const quantity<U> &q1, const quantity<U> &q2);

			// template <Unit U>
			friend std::ostream& operator<<(std::ostream& os, const quantity<U>& q);
		};

		template <Unit U>
		constexpr quantity<U> operator+(const quantity<U> &q1, const quantity<U> &q2) {
			return quantity<U>(static_cast<double>(q1) + static_cast<double>(q2));
		}

		template <Unit U>
		constexpr quantity<U> operator-(const quantity<U> &q1, const quantity<U> &q2) {
			return quantity<U>(static_cast<double>(q1) - static_cast<double>(q2));
		}

		template <Unit U>
		std::ostream& operator<<(std::ostream& os, const quantity<U>& q) {
			os << q.amount << " " << unit_traits::Traits<units::Unit>::name(U);
			return os;
		}
	};

	namespace unit_literals
	{
		using namespace units;

		constexpr quantity<Unit::kilogram> operator "" _kg(long double const amount) {
			return quantity<Unit::kilogram> { static_cast<double>(amount) };
		}

		constexpr quantity<Unit::kilogram> operator "" _kg(unsigned long long const amount) {
			return quantity<Unit::kilogram> { static_cast<double>(amount) };
		}

		constexpr quantity<Unit::liter> operator "" _l(long double const amount) {
			return quantity<Unit::liter>{ static_cast<double>(amount) };
		}

		constexpr quantity<Unit::meter> operator "" _m(long double const amount) {
			return quantity<Unit::meter>{ static_cast<double>(amount) };
		}

		constexpr quantity<Unit::piece> operator "" _pcs(unsigned long long const amount) {
			return quantity<Unit::piece> { static_cast<double>(amount) };
		}
	}


	//----------------------------------------------------------------------------------//

	void Test() {
        /*
		using namespace unit_literals;
		using namespace unit_traits;


		units::Unit weight1 = units::Unit::kilogram;
		auto weight2 { 10.1_kg };


		std::cout << "weight1 type:  " << typeid(weight1).name() << std::endl;
		std::cout << "weight1 name:  " << Traits<units::Unit>::name(weight1) << std::endl;
		

		std::cout << "\n\nweight2 type:  " << typeid(weight2).name() << std::endl;
		std::cout << "weight1     :  " << weight2 << std::endl;
         */
	}

	void Test2() {
        /*
		using namespace unit_literals;
		using namespace unit_traits;

		std::cout << " ------------------ KG (kilogram) units src ---------------------------\n" << std::endl;

		auto weight1 { 1_kg }; // OK
		auto weight2 { 4.5_kg }; // OK
		auto weight3 { weight1 + weight2 }; // OK


		std::cout << "weight1 type:  " << typeid(weight1).name() << std::endl;
		std::cout << "weight1     :  " << weight3 << std::endl;
		std::cout << "weight2     :  " << weight2 << std::endl;
		std::cout << "weight3     :  " << weight3 << std::endl;


		std::cout << "\n------------------ _m (meter) units src ---------------------------\n" << std::endl;

		auto distance { 10.3_m };
		std::cout << "distance type:  " << typeid(distance).name() << std::endl;
		std::cout << "distance     :  " << distance << std::endl;
         */
	
	}
}


namespace Literals::Custom_Literals_Tests_Binanry {

	unsigned long long operator "" _b(const char* str) {
		unsigned long long result = 0;
		size_t size = std::strlen(str);

		for (size_t i = 0; i < size; ++i)
		{
			assert(str[i] == '1' || str[i] == '0');
			result |= (str[i] - '0') << (size - i - 1);
		}
		return result;
	}


	void Convert_FromBinary() {
		std::cout << 101100_b << std::endl; // result 44
	}
}

namespace Literals::Custom_Variadic_Literals_Binanry {

	template <char ... bits>
	struct to_binary;

	template <char bit, char... bits>
	struct to_binary<bit, bits...>
	{
		static_assert(bit == '0' || bit == '1', "Not a binary value!");
		static constexpr unsigned long long value = (bit - '0') << (sizeof...(bits)) | to_binary<bits...>::value;
	};

	template <char bit>
	struct to_binary<bit>
	{
		static_assert(bit == '0' || bit == '1', "Not a binary value!");
		static constexpr unsigned long long value = (bit - '0');
	};

	template <char... bits>
	constexpr unsigned long long operator "" _b()
	{
		return to_binary<bits...>::value;
	}

	//---------------------------------------------------------------------------//

	void Convert_FromBinary() {
		int arr[1010_b]; // At compile time !!!!

		std::cout << "101100_b = " << 101100_b << std::endl; //  44
		std::cout << "11_b = " << 11_b << std::endl; //  44
	}
}

namespace Literals::Custom_Variadic_Literals_BinanryRaw {

	using byte8 = unsigned char;
	using byte16 = unsigned short;
	using byte32 = unsigned int;

	
	namespace binary_literals_internals
	{
		template <typename CharT, char... bits>
		struct binary_struct;

		template <typename CharT, char... bits>
		struct binary_struct<CharT, '0', bits...> {
			static constexpr CharT value{ binary_struct<CharT, bits...>::value };
		};

		template <typename CharT, char... bits>
		struct binary_struct<CharT, '1', bits...> {
			static constexpr CharT value{
				static_cast<CharT>(1 << sizeof...(bits)) | binary_struct<CharT, bits...>::value
			};
		};

		template <typename CharT>
		struct binary_struct<CharT> {
			static constexpr CharT value{ 0 };
		};
	}

	template<char... bits>
	constexpr byte8 operator""_b8() {
		static_assert(sizeof...(bits) <= 8, "binary literal b8 must be up to 8 digits long");
		return binary_literals_internals::binary_struct<byte8, bits...>::value;
	}

	template<char... bits>
	constexpr byte16 operator""_b16() {
		static_assert(sizeof...(bits) <= 16, "binary literal b16 must be up to 16 digits long");
		return binary_literals_internals::binary_struct<byte16, bits...>::value;
	}

	template<char... bits>
	constexpr byte32 operator""_b32() {
		static_assert(sizeof...(bits) <= 32, "binary literal b32 must be up to 32 digits long");
		return binary_literals_internals::binary_struct<byte32, bits...>::value;
	}


	void Test() {
		std::cout << "00000011_b8 = "      << 00000011_b8 << std::endl;
		std::cout << "000010101100_b16 = " << 1010000010101100_b16 << std::endl;

		auto b1 = 1010_b8;
		auto b2 = 101010101010_b16;
		auto b3 = 101010101010101010101010_b32;

		std::cout << b1 << std::endl;
		std::cout << b2 << std::endl;
		std::cout << b3 << std::endl;
	}
}

namespace Literals::String_Literals {
	using namespace std::string_literals;

	void Test1() {
		
		auto s1{ "text"s }; // std::string
		auto s2{ L"text"s }; // std::wstring
		auto s3{ u"text"s }; // std::u16string
		auto s4{ U"text"s }; // std::u32string

		std::cout << "s1 type:  " << typeid(s1).name() << std::endl;
		std::cout << "s2 type:  " << typeid(s2).name() << std::endl;
		std::cout << "s3 type:  " << typeid(s3).name() << std::endl;
		std::cout << "s4 type:  " << typeid(s4).name() << std::endl;
	}
}





namespace Literals::Chrono_Literals {
	using namespace std::literals::chrono_literals;	

	void Test() {
		auto timer{ 2h + 42min + 15s };
		std::cout << "timer type:  " << typeid(timer).name() << std::endl;
	}
}




namespace Literals::Complex_Literals {
	using namespace std::literals::complex_literals;

	void Test() {
		auto complex_varialbe { 12.0 + 4.5i }; // std::complex<double>
		std::cout << "complex_varialbe type:  " << typeid(complex_varialbe).name() << std::endl;
	}
}

namespace Literals::RawStringLiterals {

	void BaseString() {
		// A Normal string 
		std::string string1 = "Geeks.\nFor.\nGeeks.\n";
		// A Raw string 
		std::string string2 = R"(Geeks.\nFor.\nGeeks.\n)";

		std::cout << string1 << std::endl;
		std::cout << string2 << std::endl;
	}

	void Test_Auto() {
		auto filename{ R"(C:\Users\Marius\Documents\)" };
		auto pattern{ R"((\w+)=(\d+)$)" };

		auto sqlselect{R"(SELECT *
						  FROM Books
						  WHERE Publisher='Paktpub'
						  ORDER BY PubDate DESC)" };

		std::cout << filename << std::endl;
		std::cout << pattern << std::endl;
		std::cout << sqlselect << std::endl;
	}


	void Test_Delimiter() {
		auto text{ R"!!(This text contains both "( and )".)!!" };
		std::cout << text << std::endl;
	}
}


namespace Literals::SimpleExample
{
    /*
    struct Distance {
        long double meters {0};

        explicit operator long double() const {
            return meters;
        }
    };

    constexpr Distance operator "" _km(long double value) {
        return Distance {.meters = value * 1000};
    }
    */

    constexpr long double operator "" _km(long double value) {
        return value * 1000;
    }

    void Test()
    {
        long double d = 1.03_km;
        std::cout << "1.03_km = " << d << std::endl;
    }
}

void Literals::TestAll()
{
    SimpleExample::Test();

	// Custom_Literals_Tests::Test();
	// Custom_Literals_Tests::Test2();

	// Custom_Literals_Tests_Binanry::Convert_FromBinary();
	// Custom_Variadic_Literals_Binanry::Convert_FromBinary();
	// Custom_Variadic_Literals_BinanryRaw::Test();

	// RawStringLiterals::BaseString();
	// RawStringLiterals::Test_Auto();
	// RawStringLiterals::Test_Delimiter();
	
	// String_Literals::Test1();

	// Chrono_Literals::Test();

	// Complex_Literals::Test();
};
