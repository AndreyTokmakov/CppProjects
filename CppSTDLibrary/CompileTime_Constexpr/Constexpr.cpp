/**============================================================================
Name        : Constexpr.h
Created on  : 09.05.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Constexpr.h
============================================================================**/

#include "Constexpr.h"

#include <iostream>
#include <string>
#include <string_view>
#include <exception>

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace Constexpr::ConstexprArray
{
    using T = std::pair<int, std::string_view>;
    static constexpr std::array<T, 5> values{ {
        {1, "one"sv},
        {2, "two"sv},
        {3, "three"sv},
        {4, "four"sv},
        {5, "five"sv}
    } };

    consteval std::string_view get_by_index(const int i) {
        return values[i].second;
    }

    void Test()
    {
        {
            constexpr std::string_view name = get_by_index(3);
            static_assert("four"sv == name);
        }
        {
            /** Will NOT COMPILE **/
           // constexpr std::string_view name = get(values.size() + 1);
        }
    }
}


namespace Constexpr::ConstexprMap
{
	template <typename K, typename V, std::size_t Size>
	struct Map
	{
		std::array<std::pair<K, V>, Size> data;

		[[nodiscard]]
		constexpr V at(const K &key) const
		{
			const auto res = std::find_if(data.begin(), data.end(), [&key](const auto &v) {
				return v.first == key;
			});
			if (data.end() != res) {
				return res->second;
			}
			throw std::range_error("Not Found");
		}
	};

	static constexpr std::array<std::pair<std::string_view, int>, 8> color_values
	{{
		{"black"sv, 7},
		{"blue"sv, 3},
		{"cyan"sv, 5},
		{"green"sv, 2},
		{"magenta"sv, 6},
		{"red"sv, 1},
		{"white"sv, 8},
		{"yellow"sv, 4}
	}};

	int lookup_value(const std::string_view sv)
	{
		// static const auto map = std::map<std::string_view, int>{color_values.begin(), color_values.end()};
		static constexpr auto map = Map<std::string_view, int, color_values.size()>{ {color_values} };
		return map.at(sv);
	}

	void Test()
	{
		static constexpr auto testMap = Map<std::string_view, int, color_values.size()>{ {color_values} };
		constexpr int colorCode = testMap.at("green"sv);
		static_assert(2 == colorCode);
	}
}


namespace Constexpr::ConstexprSwitch
{
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

	struct Map
	{
		using T = std::pair<std::string_view, Method>;
		static inline constexpr std::array<T, 10> http_methods_mapping{ {
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
		} };

		[[nodiscard]]
		static constexpr Method get_by_name(const std::string& key)
		{
			const auto res = std::find_if(http_methods_mapping.begin(), http_methods_mapping.end(), [&key](const auto& v) {
				return v.first == key;
			});
			if (http_methods_mapping.end() != res) {
				return res->second;
			}
			return Method::None;
		}

		[[nodiscard]]
		static constexpr Method get_by_name(std::string_view key)
		{
			if (const auto res = std::find_if(http_methods_mapping.begin(), http_methods_mapping.end(), [&key](const auto& v) {
				return v.first == key;}); http_methods_mapping.end() != res) {
				return res->second;
			}
			return Method::None;
		}

		[[nodiscard]]
		static constexpr std::string_view get_by_method(Method method)
		{
			if (const auto res = std::find_if(http_methods_mapping.begin(), http_methods_mapping.end(), [&method](const auto& v) {
				return v.second == method; }); http_methods_mapping.end() != res) {
				return res->first;
			}
			return std::string_view("None");
		}
	};

	static constexpr auto mapping { Map() };

	Method getMethod(const std::string& name) {
		return mapping.get_by_name(name);
	}

	consteval Method getMethod(const std::string_view name) {
		return mapping.get_by_name(name);
	}

	consteval std::string_view getName(Method method) {
		return mapping.get_by_method(method);
	}

	void Test()
	{
		constexpr Method get = getMethod(std::string_view("GET"));
		static_assert(Method::GET == get);

		constexpr std::string_view name = getName(Method::CONNECT);
		static_assert("CONNECT"sv == name);

		constexpr auto method = getMethod(name);
		std::cout << ((Method::CONNECT == method) ? "Method::CONNECT" : "None") << std::endl;
	}
}



namespace Constexpr::ConstexprObjects
{
	class Point3D
	{
		const int x;
		const int y;
		const int z;

	public:
		constexpr explicit Point3D(const int x = 0,
						  const int y = 0,
						  const int z = 0) : x{ x }, y{ y }, z{ z } { }

		[[nodiscard]]
		constexpr int getX() const {
			return x;
		}

		[[nodiscard]]
		constexpr int getY() const {
			return y;
		}

		[[nodiscard]]
		constexpr int getZ() const {
			return z;
		}

		friend std::ostream& operator<<(std::ostream& stream, const Point3D& pt);
		constexpr bool operator==(const Point3D& pt) const  = default;
	};

	std::ostream& operator<<(std::ostream& stream, const Point3D & pt)
	{
		stream << "[" << pt.x << ", " << pt.y << ", " << pt.z << "]" << std::endl;
		return stream;
	}


	void Test()
	{
		constexpr Point3D point(10, 10, 10);

		static_assert(point == Point3D {10, 10, 10});
		static_assert(point != Point3D {1, 2, 3});
	}
}



namespace Constexpr::Exceptions
{
    // https://www.sandordargo.com/blog/2025/05/07/cpp26-constexpr-exceptions

#if 0
    constexpr unsigned int divide(const unsigned int a,
                                  const unsigned int b)
    {
        if (b == 0u) {
            throw std::invalid_argument{"division by zero"};
        }
        return a / b;
    }

    // BEFORE: compilation error due reaching a throw expression
    constexpr auto b = divide(5, 0);
    // AFTER: still a compilation error but due the uncaught exception

    constexpr std::optional<unsigned int> checked_divide(const unsigned int a,
                                                         const unsigned int b) {
        try {
            return divide(a, b);
        } catch (...) {
            return std::nullopt;
        }
    }
#endif

	constexpr int might_throw(bool should_throw) {
    	if (should_throw) {
    		throw std::runtime_error("Oops");
    	}
    	return 42;
    }

    void Test()
    {
    	/// Fail to compiler of CONSTEXPR
    	// constexpr int x = might_throw(true);

    	// but OK in runtime
    	try {
    		std::cout << might_throw(true);
    	} catch (const std::exception& e) {
    		std::cout << e.what(); // "Oops"
    	}

    	constexpr int y = might_throw(false); // Ок, y == 42


        // Compilation error
        // constexpr auto a = checked_divide(5, 0);

        // Should compile
        // constexpr std::optional<unsigned int> result = checked_divide(5, 0);
    }
}

namespace Constexpr::Strings
{

    void Constexpr_Strings()
    {
        // constexpr std::string str {"Some_String"};

        [[maybe_unused]]
        constexpr char token[] {"test_string_1"};
        constexpr std::string_view str_view = "test_string_2"sv;

        static_assert("test_string_2"sv == str_view);
    }


    constexpr std::string buildString() {
        return "Some_String";
    }

    /*
    void Constexpr_Strings2() {
        constexpr std::string str = buildString();
    }*/
}


void Constexpr::TestAll()
{
    // ConstexprArray::Test();
	// ConstexprMap::Test();
	// ConstexprSwitch::Test();
	// ConstexprObjects::Test();

    Exceptions::Test();
    // Strings::Constexpr_Strings();
}
