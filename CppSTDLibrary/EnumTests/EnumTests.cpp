//============================================================================
// Name        : EnumTests.h
// Created on  : 02.06.2020
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Enum src
//============================================================================

#include "EnumTests.h"

#include <iostream>
#include <array>
#include <string>
#include <cstdint>
#include <utility>
#include <format>

namespace EnumTests::EnumNewStyle {

	void EnumsStictType_Test() {
		enum class Options { None, One, All };
		Options opts = Options::All;
	}

	void EnumsStictType_ToInt() {
		enum Options { None, One, All };
		Options opts = Options::All;
		std::cout << opts << std::endl;
	}
};

namespace EnumTests::EnumClassTests {

	enum class Color {
		black,
		white,
		gray
	};

	void __printColor(Color color) {
		switch (color) {
		case Color::gray:
			std::cout << "white + " << std::endl;
			[[fallthrough]];
		case Color::black:
			std::cout << "black" << std::endl;
			break;
		case Color::white:
			std::cout << "white" << std::endl;
			break;
		}
	}

	void PrintColor() {
		std::cout << "Color::black" << std::endl;
		const Color color1 = Color::black;
		__printColor(color1);

		std::cout << "\nColor::white" << std::endl;
		const Color color2 = Color::white;
		__printColor(color2);

		std::cout << "\nColor::gray" << std::endl;
		const Color color3 = Color::gray;
		__printColor(color3);
	}
};

namespace EnumTests::Traits {

	enum class Color {
		Red,
		Green,
		Black
	};

	template <typename T>
	class Traits {
	public:
		static const std::string name(size_t index) {
			return "";
		}
	};

	template <>
	class Traits<Color> {
	public:
		static const std::string name(size_t index) {
			switch (index) {
				case static_cast<int>(Color::Red) :
					return "Red";
				case static_cast<int>(Color::Green) :
					return "Green";
				case static_cast<int>(Color::Black) :
					return "Black";
				default:
					return "unknown";
			}
		}
	};


	void TraitsTests() {
		std::cout << Traits<Color>::name(1) << std::endl;
	}
}

namespace EnumTests::Http {

	enum class Method {
		None = 0,
		GET = 1,
		HEAD = 2,
		POST = 3,
		PUT = 4,
		DELETE = 5,
		CONNECT = 6,
		OPTIONS = 7,
		TRACE = 8,
		PATCH = 9
	};

	std::string toString(Method method) {
		switch (method) {
			case Http::Method::None:
				return "None";
				break;
			case Http::Method::GET:
				return "GET";
				break;
			case Http::Method::HEAD:
				return "HEAD";
				break;
			case Http::Method::POST:
				return "POST";
				break;
			case Http::Method::PUT:
				return "PUT";
				break;
			case Http::Method::DELETE:
				return "DELETE";
				break;
			case Http::Method::CONNECT:
				return "CONNECT";
				break;
			case Http::Method::OPTIONS:
				return "OPTIONS";
				break;
			case Http::Method::TRACE:
				return "TRACE";
				break;
			case Http::Method::PATCH:
				return "PATCH";
				break;
			default:
				return "None";
				break;
		}
	}

	Method fromString(const std::string& method) {
		if (0 == method.compare("GET"))
			return Method::GET;
		else if (0 == method.compare("HEAD"))
			return Method::HEAD;
		else if (0 == method.compare("POST"))
			return Method::POST;
		else if (0 == method.compare("PUT"))
			return Method::PUT;
		else if (0 == method.compare("DELETE"))
			return Method::DELETE;
		else if (0 == method.compare("CONNECT"))
			return Method::CONNECT;
		else if (0 == method.compare("OPTIONS"))
			return Method::OPTIONS;
		else if (0 == method.compare("TRACE"))
			return Method::TRACE;
		else if (0 == method.compare("PATCH"))
			return Method::PATCH;
		return Method::None;
	}

	void Test() {
		Method method1 = fromString("GET");
		std::cout << toString(method1) << std::endl;

		Method method2 = static_cast<Method>(5);
		std::cout << toString(method2) << std::endl;
	}
}

namespace EnumTests::Http2 {

	enum class Method {
		None = 0,
		GET = 1,
		HEAD = 2,
		POST = 3,
		PUT = 4,
		DELETE = 5,
		CONNECT = 6,
		OPTIONS = 7,
		TRACE = 8,
		PATCH = 9
	};

	using T = std::pair<std::string_view, Method>;
	static constexpr std::array<T, 10> http_methods_mapping {{
		{std::string_view("None"),    Method::None},
		{std::string_view("GET"),     Method::GET},
		{std::string_view("HEAD"),    Method::HEAD},
		{std::string_view("POST"),    Method::POST},
		{std::string_view("PUT"),     Method::PUT},
		{std::string_view("DELETE"),  Method::DELETE},
		{std::string_view("CONNECT"), Method::CONNECT},
		{std::string_view("OPTIONS"), Method::OPTIONS},
		{std::string_view("TRACE"),   Method::TRACE},
		{std::string_view("PATCH"),   Method::PATCH}
	}};

	class Map {
	public:
		std::array<std::pair<std::string_view, Method>, http_methods_mapping.size()> data;

	public:
		[[nodiscard]]
		constexpr Method get_by_name(const std::string& key) const {
            // Do not compile on ubuntu
            /*
			const auto res = std::find_if(data.begin(), data.end(), [&key](const auto& v) {
				return v.first == key;
			});
			if (data.end() != res) {
				return res->second;
			}
            */
			return Method::None;
		}

		[[nodiscard]]
		constexpr Method get_by_name(std::string_view key) const {
            // Do not compile on ubuntu
            /*
			const auto res = std::find_if(data.begin(), data.end(), [&key](const auto& v) {
				return v.first == key;
			});
			if (data.end() != res) {
				return res->second;
			}
            */
			return Method::None;
		}

		[[nodiscard]]
		constexpr std::string_view get_by_method(Method method) const {
            /*
			const auto res = std::find_if(data.begin(), data.end(), [&method](const auto& v) {
				return v.second == method;
			});
			if (data.end() != res) {
				return res->first;
			}
             */
			return std::string_view("None");
		}
	};

	static constexpr auto mapping = Map{ http_methods_mapping };

	Method getMetchod(const std::string& name) {
		return mapping.get_by_name(name);
	}

	// consteval   // Do not compile on ubuntu
    constexpr Method getMetchod(std::string_view name) {
		return mapping.get_by_name(name);
	}

    // consteval   // Do not compile on ubuntu
    constexpr std::string_view getName(Method method) {
		return mapping.get_by_method(method);
	}

	void Test_GetByName() {
		constexpr Method method1 = getMetchod(std::string_view("GET"));
		// std::cout << getName(method1) << std::endl;
	}
}

namespace EnumTests::Iteration {

	enum class Method {
		None = 0,
		GET = 1,
		HEAD = 2,
		POST = 3,
		PUT = 4,
		DELETE = 5,
		CONNECT = 6,
		OPTIONS = 7,
		TRACE = 8,
		PATCH = 9,
		END = 10
	};

	Method& operator ++ (Method& e) {
		if (e == Method::END) {
			throw std::out_of_range("for Method& operator ++ (Method&)");
		}
		e = Method(static_cast<std::underlying_type<Method>::type>(e) + 1);
		return e;
	}

	std::ostream& operator<<(std::ostream& os, Method opt) {
		return os << static_cast<unsigned short>(opt);
	}


	void Test() {
		for (Method m = Method::None; m != Method::END; ++m) {
			std::cout << m << std::endl;
		}
	}
}


namespace EnumTests::DerivedEnums {

	// altitude may be altitude::high or altitude::low
	enum class altitude : char
	{
		high = 'h',
		low = 'l', // C++11 allows the extra comma
	};

	std::ostream& operator<<(std::ostream& os, altitude al) {
		return os << static_cast<char>(al);
	}

	void Test() {

		altitude a = altitude::low;
		std::cout << "a = " << a << std::endl;
		std::cout << sizeof(altitude) << std::endl;
	}

	/********************************************************************************/

	enum class Options : uint32_t {
		None = 0, 
		one = 1 << 0,  
		two = 1 << 1, 
		three = 1 << 2, 
	};

	std::ostream& operator<<(std::ostream& os, Options opt) {
		return os << static_cast<unsigned short>(opt);
	}

	void Bit_Enum_Test() {
		
		std::cout << "Sizeof(Options) = " << sizeof(Options) << "\n" << std::endl;

		std::cout << Options::None << std::endl;
		std::cout << Options::one << std::endl;
		std::cout << Options::two << std::endl;
		std::cout << Options::three << std::endl;
	}
}


namespace EnumTests::Colors {

	enum class WebColor { 
		red = 0xFF0000, 
		green = 0x00FF00,
		blue = 0x0000FF 
	};

	//------------------------------------------------------------//

	void Test() {
		//std::cout << WebColor::red << std::endl;
	}
}

namespace EnumTests::Underlying_Type {

	enum e1 {};
	enum class e2 {};
	enum class e3 : unsigned {};
	enum class e4 : int {};
	enum class e5 : uint8_t {};

	void Enum_Tests() {

		constexpr bool e1_t = std::is_same_v< std::underlying_type_t<e1>, int >;
		constexpr bool e2_t = std::is_same_v< std::underlying_type_t<e2>, int >;
		constexpr bool e3_t = std::is_same_v< std::underlying_type_t<e3>, int >;
		constexpr bool e4_t = std::is_same_v< std::underlying_type_t<e4>, int >;
		constexpr bool e5_t = std::is_same_v< std::underlying_type_t<e5>, int >;

		std::cout << "underlying type for 'e1' is " << (e1_t ? "int" : "non-int") << '\n'
			<< "underlying type for 'e2' is " << (e2_t ? "int" : "non-int") << '\n'
			<< "underlying type for 'e3' is " << (e3_t ? "int" : "non-int") << '\n'
			<< "underlying type for 'e4' is " << (e4_t ? "int" : "non-int") << '\n'
			<< "underlying type for 'e5' is " << (e5_t ? "int" : "non-int") << '\n';
	}

	// -----------------------------------------------

	enum class AccountNumber : uint32_t {
		//No Enumerators
	}; 

	enum class Byte : uint8_t { 
	};

	void Uniform_Initialization_Tests() {
#if 0                                 // Implicit conversion not allowed 
		AccountNumber a0 = 817986000; // Error. We know that.
#endif // 0


		//cast-initialization works but not safe
		AccountNumber account1 = AccountNumber(817986000); //OK, but dangerous 


		Byte b0 {}; //OK. Value-Initialized with 0
		Byte b1 { 1 }; //OK
		Byte b3 = Byte{ 3 }; //OK

	}
}


namespace Bit_Flags_Enums {

	enum class State : std::uint8_t {
		engine_on = 0b00000001,
		lights_on = 0b00000010,
		wipers_on = 0b00000100
	};

	State operator|(State lhs, State rhs) {
		return static_cast<State>(static_cast<std::underlying_type_t<State>>(lhs) |
								  static_cast<std::underlying_type_t<State>>(rhs));
	}

	State operator&(State lhs, State rhs) {
		return static_cast<State>(static_cast<std::underlying_type_t<State>>(lhs) &
								  static_cast<std::underlying_type_t<State>>(rhs));
	}

	void Test() {
		{
			State current = State::engine_on;
			std::cout << static_cast<int>(current) << std::endl;
		}

		{
			State current = State::engine_on | State::lights_on;
			std::cout << static_cast<int>(current) << std::endl;
		}
	}
}


namespace EnumTests::UsingEmum_ClassScope
{
    enum struct Color {
        red,
        green,
        blue
    };

    struct Something
    {
        using enum Color;
    };

    void accessEnum_FromClassInstance()
    {
        Something s;
        const Color color = s.red;
    }
}

namespace EnumTests::EnumConversationTest
{
    enum class Color {
        red,
        green,
        blue
    };

    struct kEnumToStringViewBimap
    {
        static constexpr std::string_view operator[](Color color) noexcept
        {
            switch(color) {
                case Color::red: return "red";
                case Color::green: return "green";
                case Color::blue: return "blue";
            }
        }

        static constexpr Color operator[](std::string_view color) noexcept
        {
            if (color == "red") {
                return Color::red;
            } else if (color == "green") {
                return Color::green;
            } else if (color == "blue") {
                return Color::blue;
            }
        }
    };

    void Test()
    {
        static_assert(kEnumToStringViewBimap{}["red"] == Color::red);
        static_assert(kEnumToStringViewBimap{}["green"] == Color::green);
        static_assert(kEnumToStringViewBimap{}["blue"] == Color::blue);

        static_assert(kEnumToStringViewBimap{}[Color::red] == "red");
        static_assert(kEnumToStringViewBimap{}[Color::green] == "green");
        static_assert(kEnumToStringViewBimap{}[Color::blue] == "blue");
    }
}

namespace EnumTests::ToUnderlying
{
	enum class MyColor {
		RED,
		GREEN,
		BLUE
	};

	void To_Underlying()
	{
		auto color = std::to_underlying(MyColor::GREEN);
		// color == 1, decltype(color) == int

		std::cout << std::format("color underlying type is '{}'", color) << std::endl;
		static_assert(std::is_same_v<decltype(color), int>);
	}
}


namespace EnumTests::Is_Scoped_Enum
{
    enum RGB
    {
        Red,
        Green,
        Black
    };

    enum class Weekdays
    {
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday,
        Sunday
    };


    void is_scoped()
    {
        Weekdays weekday = Weekdays::Sunday;

        int type = std::to_underlying(weekday);
        std::cout << std::format("Weekdays underlying type is '{}'", type) << std::endl;

        static_assert(std::is_same_v<decltype(type), int>);

        static_assert(std::is_scoped_enum<decltype(weekday)>::value);
        static_assert(not std::is_scoped_enum<RGB>::value);
    }
}


void EnumTests::TestAll()
{
	// EnumClassTests::PrintColor();

	// EnumNewStyle::EnumsStictType_Test();

	// Traits::TraitsTests();

	// Http::Test();

	// Http2::Test_GetByName();

	// DerivedEnums::Test();
	// DerivedEnums::Bit_Enum_Test();

	// Iteration::Test();

	// Colors::Test();

	// Underlying_Type::Enum_Tests();

	// Underlying_Type::Uniform_Initialization_Tests();

	// Bit_Flags_Enums::Test();

    // EnumConversationTest::Test(); /** static constexpr functions **/

    // UsingEmum_ClassScope::accessEnum_FromClassInstance();

	ToUnderlying::To_Underlying();

    Is_Scoped_Enum::is_scoped();
};
