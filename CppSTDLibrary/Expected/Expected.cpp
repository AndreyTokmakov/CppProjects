/**============================================================================
Name        : Expected.cpp
Created on  : 20.05.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Expected C++23 Library tests
============================================================================**/

#include <iostream>
#include <string_view>
#include <expected>
#include <vector>
#include <chrono>

#include "Expected.h"

namespace Expected
{
    enum class ParseError {
        InvalidChar,
        Overflow
    };

    template<typename Ty, typename ErrorType>
    void isError(const std::expected<Ty, ErrorType> &ex) {
        if (!ex) {
            std::cout << "Contains an error value\n";
            std::cout << "Error: " << ex.error() << std::endl;
        } else
            std::cout << "OK. No error!\n";
    }

    template<typename Ty, typename ErrorType>
    void basic_functions_tests(const std::expected<Ty, ErrorType> &ex)
    {
        std::cout << "error()     : " << ex.error() << std::endl;
        std::cout << "bool()      : " << std::boolalpha<<  static_cast<bool>(ex) << std::endl;
        std::cout << "bool(not)   : " << std::boolalpha<<  static_cast<bool>(not ex) << std::endl;
        std::cout << "bool(!)     : " << std::boolalpha<<  static_cast<bool>(!ex) << std::endl;
        if (ex.has_value()) {
            std::cout << "value()     : " << ex.value() << std::endl;
            std::cout << "has_value() : " << ex.has_value() << std::endl;
        }
        std::cout << "error_or()  : " << ex.error_or(100500) << std::endl;

    }

    void BasicFunctions()
    {
        std::expected<double, int> ok = 3.14;
        basic_functions_tests(ok);

        std::cout << "------------------- Error ------------------------\n";

        std::expected<double, int> err = std::unexpected(3);
        basic_functions_tests(err);
    }

    void Test_Error()
    {
        std::expected<double, int> ex = std::unexpected(3);

        isError(ex);

        if (ex == std::unexpected(3))
            std::cout << "The error value is equal to 3\n";

        if (ex.error() == 3)
            std::cout << "The error value is equal to 3 (Same as above)\n";
    }


    void Test_Error_2()
    {
        std::expected<double, ParseError> ex = std::unexpected(ParseError::InvalidChar);

        if (!ex) {
            if (ex == std::unexpected(ParseError::InvalidChar)) {
                std::cout << "InvalidChar\n";
            } else if (ex == std::unexpected(ParseError::InvalidChar)) {
                std::cout << "Overflow\n";
            }
        }
    }


    void Test_OK()
    {
        std::expected<double, int> ex = 3.14;
        isError(ex);
    }

    void Emplace()
    {
        std::expected<double, int> ex;
        basic_functions_tests(ex);

        std::cout << "------------------- After emplace: 1 ------------------------\n";

        ex.emplace(1);
        basic_functions_tests(ex);
    }
};

namespace Expected
{
    std::expected<int, std::string> getInt(const std::string& arg)
    {
        try {
            return std::stoi(arg);
        }
        catch (...) {
            return std::unexpected{std::string(arg + ": Error")};
        }
    }

    void Transform()
    {
        const std::vector<std::string> strings = {"66", "foo", "-5"};
        for (const std::string& str: strings)
        {
            std::expected res = getInt(str)
                    .transform( [](int n) { return n + 100; })
                    .transform( [](int n) { return "[" + std::to_string(n) + "]"; });

            std::cout << *res << std::endl;
        }
    }
}

namespace Expected::MonadicOperations
{
    template<typename SuccessType, typename ErrorType>
    void printExpectedResult(const std::expected<SuccessType, ErrorType>& result)
    {
        if (result)
            std::cout << *result << '\n';
        else
            std::cout << "Error: " << result.error() << '\n';
    }
}

namespace Expected::MonadicOperations::Transform_AndThen
{
    std::expected<std::string, std::error_condition> read_input() {
        std::string s;
        if (not (std::cin >> s))
            return std::unexpected{std::make_error_condition(std::io_errc::stream)};
        return s;
    }

    std::expected<int, std::error_condition> to_int(const std::string& s) {
        try {
            return std::stoi(s);
        } catch (std::exception& e) {
            return std::unexpected{std::make_error_condition(std::errc::argument_out_of_domain)};
        }
    }

    int add_ten(int v) { return v + 10; }

    std::expected<int, std::error_condition> log_error(const std::error_condition& err) {
        std::cerr << "Operation failed : " << err.message() << "\n";
        return std::unexpected{err};
    }

    void Test()
    {
        std::expected result = read_input()
                .and_then(to_int)    // invoked if the expected contains a value  the callable has to return a std::expected,
                                        // but can change the type std::expected<T,Err> -> std::expected<U,Err>
                .transform(add_ten)  // invoked if the expected contains a value
                                        // the callable can return any type std::expected<T,Err> -> std::expected<U,Err>
                .or_else(log_error); // invoked if the expected contains an error the callable has to return a std::expected,
                                        // but can change the type std::expected<V,T> -> std::expected<V,U>

        std::cout << *result << std::endl;
    }
}


namespace Expected::MonadicOperations::AndThen
{
    std::expected<int, std::string> incrementIfPositive(int value)
    {
        if (value > 0)
            return value + 1;
        return std::unexpected("Value must be positive");
    }

    std::expected<int, std::string> getInput(int x)
    {
        if (x % 2 == 0)
            return x;
        return std::unexpected("Value not even!");
    }

    void Test()
    {
        for (int value: {-10, 0 , 10})
        {
            // std::cout << getInput(value).and_then(incrementIfPositive).value_or(0) << std::endl;
            std::expected<int, std::string> input = getInput(value);
            std::expected<int, std::string> result = input.and_then(incrementIfPositive);

            printExpectedResult(result);
        }
    }
}

namespace Expected::MonadicOperations::AndThen_Chaining
{
    std::expected<int, std::string> convertToInt(const std::string& input)
    {
        int value;
        auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
        if (ec == std::errc())
            return value;

        return std::unexpected("Conversion failed: invalid input (" + input + ")");
    }

    std::expected<int, std::string> multiplyByTwo(int number)
    {
        if (number > 0)
            return number * 2;
        return std::unexpected("Calculation failed: number must be positive (" + std::to_string(number) + ")");
    }

    std::expected<int, std::string> printAndReturn(int number)
    {
        std::cout << "Result: " << number << '\n';
        return std::expected<int, std::string>(number);
    }

    void Test()
    {
        for (std::string&& input: {"10", "abc", "null", "-1", "1"})
        {
            const std::expected<int, std::string> result = convertToInt(input)
                    .and_then(multiplyByTwo)
                    .and_then(printAndReturn);
            if (!result)
                std::cout << result.error() << '\n';
        }
    }
}

namespace Expected::MonadicOperations::AndThen_OrElso
{
    std::expected<int, std::string> convertToInt(const std::string& input)
    {
        int value;
        auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
        if (ec == std::errc())
            return value;

        return std::unexpected("Conversion failed: invalid input (" + input + ")");
    }

    std::expected<int, std::string> multiplyByTwo(int number)
    {
        if (number > 0)
            return number * 2;
        return std::unexpected("Calculation failed: number must be positive (" + std::to_string(number) + ")");
    }

    std::expected<int, std::string> printAndReturn(int number)
    {
        std::cout << "Result: " << number << '\n';
        return std::expected<int, std::string>(number);
    }

    std::expected<int, std::string> handleError(const std::string& error) {
        std::cerr << "Error encountered: " << error << '\n';
        return std::unexpected(error);
    }


    void Test()
    {
        for (auto input: {"10", "abc", "null", "-1", "1"})
        {
            const std::expected<int, std::string> result = convertToInt(input)
                    .and_then(multiplyByTwo)
                    .and_then(printAndReturn)
                    .or_else(handleError);
        }
    }
}

namespace Expected::MonadicOperations::TransformError
{
    std::string backToString(int value)
    {
        return std::string {"("}.append(std::to_string(value) ).append(")");
    }

    std::string errorAsString(std::string&& error)
    {
        return std::string {"ERROR: "}.append(error);
    }

    std::expected<int, std::string> convertToInt(const std::string& input)
    {
        int value;
        auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
        if (ec == std::errc())
            return value;

        return std::unexpected("Conversion failed: invalid input (" + input + ")");
    }

    void Test()
    {
        for (std::string && input: {"10", "abc", "null", "-1", "1"})
        {
            const std::expected<std::string, std::string> result = convertToInt(input)
                    .transform(backToString)
                    .transform_error(errorAsString);
            std::cout << (result.has_value() ? result.value() : result.error()) << std::endl;
        }
    }
}

namespace Expected::MonadicOperations::Transform_OrElse
{
    std::string backToString(int value)
    {
        return "[" + std::to_string(value) + "]";
    }

    std::expected<std::string, std::string> handleError(std::string&& error)
    {
        std::cerr << "Error: " << error << '\n';
        return std::unexpected(error);
    }

    std::expected<std::string, std::string> printIfOK(std::string&& result)
    {
        std::cout << "OK   : " << result << '\n';
        return result;
    }

    std::expected<int, std::string> convertToInt(const std::string& input)
    {
        int value;
        auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
        if (ec == std::errc())
            return value;

        return std::unexpected("Conversion failed: invalid input (" + input + ")");
    }

    void Test()
    {
        for (auto input: {"10", "abc", "null", "-1", "1"})
        {
            const std::expected<std::string, std::string> result = convertToInt(input)
                    .transform(backToString)
                    .and_then(printIfOK)
                    .or_else(handleError);
        }
    }
}namespace Expected::MonadicOperations::Transform
{
    std::string backToString(int value)
    {
        return "[" + std::to_string(value) + "]";
    }

    std::expected<int, std::string> convertToInt(const std::string& input)
    {
        int value;
        auto [ptr, ec] = std::from_chars(input.data(), input.data() + input.size(), value);
        if (ec == std::errc())
            return value;

        return std::unexpected("Conversion failed: invalid input (" + input + ")");
    }

    void Test()
    {
        for (auto input: {"10", "abc", "null", "-1", "1"})
        {
            const std::expected<std::string, std::string> result = convertToInt(input).transform(backToString);
            printExpectedResult(result);
        }
    }
}




void Expected::TestAll()
{
    // BasicFunctions();

    // Test_OK();

    // Test_Error();
    // Test_Error_2();

    // Emplace();

    // Expected::Transform();


    // MonadicOperations::AndThen::Test();
    // MonadicOperations::AndThen_Chaining::Test();
    // MonadicOperations::AndThen_OrElso::Test();
    // MonadicOperations::Transform::Test();
    // MonadicOperations::Transform_OrElse::Test();
    // MonadicOperations::Transform_AndThen()::Test();
    MonadicOperations::TransformError::Test();

};

