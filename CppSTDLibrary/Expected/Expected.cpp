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
#include <fstream>
#include <print>

#include "Expected.h"

namespace
{
    [[maybe_unused]]
    [[nodiscard]]
    std::string readFileToString(std::string_view path) noexcept
    {
        std::string data {};
        if (std::fstream file(path.data(), std::ios::in | std::ios::binary); file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            const auto bytesLength{ file.tellg() };
            file.seekg(0, std::ios_base::beg);

            data.resize(bytesLength);
            file.read(data.data(), bytesLength);
        }
        return data;
    }

    [[nodiscard]]
    std::string readFileToString(std::ifstream& file) noexcept
    {
        std::string data {};
        if (file.is_open() && file.good())
        {
            file.seekg(0, std::ios_base::end);
            const auto bytesLength{ file.tellg() };
            file.seekg(0, std::ios_base::beg);

            data.resize(bytesLength);
            file.read(data.data(), bytesLength);
        }
        return data;
    }
}

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
        for (std::string input: {"10", "abc", "null", "-1", "1"})
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
        return std::string {"ERROR ----> : '"}.append(error).append("'");
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
        for (std::string input: {"10", "abc", "null", "-1", "1"})
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
    std::string backToString(const int value)
    {
        return "'" + std::to_string(value) + "'";
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

namespace Expected::ReadFileExample
{
    constexpr std::string_view primaryFile { R"(../../CppSTDLibrary/test_data/primary.json)" };
    constexpr std::string_view secondaryFile { R"(../../CppSTDLibrary/test_data/secondary.json)" };

    constexpr std::string_view badFileMissing { R"(../../CppSTDLibrary/test_data/bad.json)" };
    constexpr std::string_view dataFileMissing { R"(../../CppSTDLibrary/test_data/wrong_name.json)" };

    std::expected<std::string,std::string> readFile(std::ifstream&& fStream)
    {
        return readFileToString(fStream);
    }

    std::expected<std::ifstream, std::string> open_file(const std::string& filename)
    {
        std::println("Trying to open file {}",filename);
        std::ifstream file{ filename };
        if (!file.is_open()) {
            return std::unexpected{ std::format("Failed to open file: {}", filename) };
        }
        return std::move(file);
    }

    void HandleFileOpenError()
    {
        auto result = open_file(dataFileMissing.data())
             .or_else([](const std::string& error) {
                 std::println("Error occurred: {} opening fallback file: {}", error, primaryFile);
                 return open_file(primaryFile.data());
        });
    }

    void HandleFileOpenError_Default()
    {
        auto result = open_file(dataFileMissing.data())
            .transform_error([](const std::string& error){
             return std::string{"ERROR"};
        });

        std::cout << "Error: " << result.error() << std::endl;
    }

    void HandleFileOpenError_MultipleFallback()
    {
        const std::expected<std::string, std::string> result = open_file(badFileMissing.data())
        .or_else([](const std::string& error) {
             return open_file(dataFileMissing.data());
        })
        .or_else([](const std::string& error) {
            return open_file(primaryFile.data());
        }).and_then(readFile);

        if (result.has_value()) {
            std::cout << "File contents: " << result.value() << '\n';
        }
    }
}

namespace Expected::ReadFile_AndParse_Example
{
    constexpr std::string_view secondaryFile { R"(../../CppSTDLibrary/test_data/secondary.json)" };
    constexpr std::string_view badFile { R"(../../CppSTDLibrary/test_data/bad.json)" };

    struct Config
    {
        std::string apiKey {};
    };

    std::expected<std::ifstream, std::string> open_file(const std::string& filename)
    {
        std::println("Trying to open file {}",filename);
        std::ifstream file{ filename };
        if (!file.is_open()) {
            return std::unexpected{ std::format("Failed to open file: {}", filename) };
        }
        return std::move(file);
    }

    std::expected<Config, std::string> parse(std::ifstream&& file)
    {
        if (const std::string content { readFileToString(file) }; !content.empty()) {
            /** Some parsing logic here**/
            return Config {.apiKey = "SECRET_KEY"};
        }
        file.close();
        return std::unexpected{ "Parse error" };
    }

    void Read_and_Parse()
    {
        const std::expected<std::string, std::string> api_key_expected = open_file(badFile.data())
            .or_else([](const std::string& error){
                std::println("Error occurred: {} opening fallback file: {}", error, secondaryFile);
                return open_file(secondaryFile.data());
            })
            .and_then(parse).transform([](const Config& config) {
                return config.apiKey;
            })
            .transform_error([](const std::string& error) {
                return std::format("[ERROR] {}", error);
            });

        if (api_key_expected.has_value())
            std::println("API key = {}", api_key_expected.value());
        else
            std::print(std::cerr, "{}", api_key_expected.error());
    }
}


namespace Expected::Creation_Objects_Copying
{
    struct Object
    {
        int value1 { 0 };
        int value2 { 0 };

        Object(const int v1, const int v2): value1 {v1}, value2{v2} {
            std::println("Object({} {})", value1, value2);
        }

        ~Object() {
            std::cout << "~Object()" << '\n';
        }

        Object(const Object &other) {
            std::cout << "Object(const Object &other)" << std::endl;
        }

        Object(Object &&other) noexcept {
            std::cout << "Object(Object &&other)" << std::endl;
        }

        Object & operator=(const Object &other)
        {
            std::cout << "Object & operator=(const Object &other)" << std::endl;
            return *this;
        }

        Object & operator=(Object &&other) noexcept
        {
            std::cout << "Object & operator=(Object &&other)" << std::endl;
            return *this;
        }
    };

    struct HeavyError
    {
        int value1 { 0 };
        int value2 { 0 };

        HeavyError(const int v1, const int v2): value1 {v1}, value2{v2} {
            std::println("HeavyError({} {})", value1, value2);
        }

        ~HeavyError() {
            std::cout << "~HeavyError()" << '\n';
        }

        HeavyError(const HeavyError &) {
            std::cout << "HeavyError(const HeavyError &)" << std::endl;
        }

        HeavyError(HeavyError &&) noexcept {
            std::cout << "HeavyError(HeavyError &&)" << std::endl;
        }

        HeavyError & operator=(const HeavyError &) {
            std::cout << "HeavyError & operator=(const HeavyError &)" << std::endl;
            return *this;
        }

        HeavyError & operator=(HeavyError &&) noexcept {
            std::cout << "HeavyError & operator=(HeavyError &&)" << std::endl;
            return *this;
        }
    };

    enum class Error
    {
        Type1,
        Type2,
        Type3
    };

    std::expected<Object, Error> makeObject_Bad() {
        return Object{1,2};
    }

    std::expected<Object, Error> makeObject_InPlace() {
        return std::expected<Object, Error> { std::in_place, 1, 2 };
    }

    std::expected<Object, HeavyError> unexpected_Bad() {
        return std::unexpected<HeavyError>{ HeavyError {1, 2}};
    }

    std::expected<Object, HeavyError> unexpected_InPlace() {
        return std::unexpected<HeavyError>{ std::in_place, 1, 2 };
    }

    std::expected<Object, HeavyError> unexpected_UnExpect_Good() {
        return std::expected<Object, HeavyError> { std::unexpect, 1, 2 };
    }

    void returnExpected()
    {
        {
            const std::expected<Object, Error> result = makeObject_Bad();
        }
        std::cout << std::string(120, '-') << std::endl;
        {
            const std::expected<Object, Error> result = makeObject_InPlace();
        }

        /**
        Object(1 2)
        Object(Object &&other)
        ~Object()
        ~Object()
        ---------------------------------------------------------------------------------------------
        Object(1 2)
        ~Object()
        **/
    }

    void returnUneExpected()
    {
        {
            const std::expected<Object, HeavyError> result = unexpected_Bad();
        }
        std::cout << std::string(120, '-') << std::endl;
        {
            const std::expected<Object, HeavyError> result = unexpected_InPlace();
        }
        std::cout << std::string(120, '-') << std::endl;
        {
            const std::expected<Object, HeavyError> result = unexpected_UnExpect_Good();
        }

        /**
        *HeavyError(1 2)
        HeavyError(HeavyError &&)
        HeavyError(HeavyError &&)
        ~HeavyError()
        ~HeavyError()
        ~HeavyError()
        ---------------------------------------------------------------------------------------------
        HeavyError(1 2)
        HeavyError(HeavyError &&)
        ~HeavyError()
        ~HeavyError()
        ---------------------------------------------------------------------------------------------
        HeavyError(1 2)
        ~HeavyError()
        **/
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
    // MonadicOperations::TransformError::Test();

    // ReadFileExample::HandleFileOpenError();
    // ReadFileExample::HandleFileOpenError_Default();
    // ReadFileExample::HandleFileOpenError_MultipleFallback();

    // ReadFile_AndParse_Example::Read_and_Parse();

    // Creation_Objects_Copying::returnExpected();
    Creation_Objects_Copying::returnUneExpected();
};

