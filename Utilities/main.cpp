/**============================================================================
Name        : Utilities.cpp
Created on  : 29.02.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Utilities
============================================================================**/

#include <iostream>
#include <string_view>
#include <vector>
#include <array>
#include <list>
#include <map>
#include <iomanip>
#include <thread>
#include <fstream>
#include <charconv>
#include <optional>

#include <x86intrin.h>

#include "StringUtilities.h"
#include "FileUtilities.h"
#include "CSVReader/Tests.h"
#include "Base64.h"
#include "HexConverter.h"
#include "PerfUtilities.h"

namespace
{
    template<typename T>
    std::ostream& operator<<(std::ostream & stream,
                             const std::vector<T>& collection)
    {
        for (const T& v: collection)
            stream << v << ' ';
        return stream;
    }

    template<typename T>
    std::ostream& operator<<(std::ostream & stream,
                             const std::list<T>& collection)
    {
        for (const T& v: collection)
            stream << v << ' ';
        return stream;
    }

    template<typename T, size_t _Size>
    std::ostream& operator<<(std::ostream & stream,
                             const std::array<T, _Size>& collection)
    {
        for (const T& v: collection)
            stream << v << ' ';
        return stream;
    }
}


namespace StringUtilities
{
    void slice_string(std::string &str, size_t from, size_t until)
    {
        if (!(str.length() > until && until > from))
            return;

        size_t pos = 0;
        for (size_t idx = from; idx <= until; ++idx)
            str[pos++] = str[idx];
        str.resize(pos);
        str.shrink_to_fit();
    }
}

namespace StringUtilitiesTests
{
    using namespace StringUtilities;

    void split_test_1()
    {

        const std::string text { "11_22_33_44" };

        {
            const std::vector<std::string> parts = split(text, "_");
            std::cout << parts << std::endl;
        }

        {
            const std::vector<std::string> parts = split(text, 10,"_");
            std::cout << parts << std::endl;
        }

        {
            std::vector<std::string_view> parts;
            split_to(text, parts,"_");
            std::cout << parts << std::endl;
        }
    }

    void trim_string_test()
    {
        for (const std::string& base: std::vector<std::string>{
                "   Some   Sample    String  "
        })
        {   std::cout << "Input: " << std::quoted(base) << std::endl;

            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_1(str);
                std::cout << std::quoted(str) << std::endl;
            }
            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_2(str);
                std::cout << std::quoted(str) << std::endl;
            }
            if (std::string str(base); not str.empty())
            {
                StringUtilities::trim_3(str);
                std::cout << std::quoted(str) << std::endl;
            }
        }
    }
    void strip_string_test()
    {
        std::string str1 { "\t\t  A good   examplE    \n\t\n" };

        std::cout << std::quoted(str1) << std::endl;
        strip(str1);
        std::cout << std::quoted(str1) << std::endl;
    }

    void remove_chars_from_string_test()
    {
        std::string str1 { "\t\t  A good   examplE    \n\t\n" };

        std::cout << std::quoted(str1) << std::endl;
        remove_chars_from_string(str1);
        std::cout << std::quoted(str1) << std::endl;
    }

    void Update_string_test()
    {
        std::string str { "0123456789___________________" };
        std::cout << std::quoted(str) << "  " << str.capacity() << std::endl;

        slice_string(str, 3, 8);

        std::cout << std::quoted(str) << "  " << str.capacity() << std::endl;
    }

    void Random_String()
    {
        for (int i = 10; i < 20; ++i)
            std::cout << randomString(i) << std::endl;
    }
}

namespace FileUtilities_Tests
{
    const std::string testFilePath { R"(/home/andtokm/DiskS/Temp/Folder_For_Testing/test_file.txt)" };

    void ReadFile()
    {
        std::string text = FileUtilities::ReadFile(testFilePath);
        std::cout << text << std::endl;
    }

    void ReadFile2String()
    {
        std::string text;
        FileUtilities::ReadFile2String(testFilePath, text);
        std::cout << text << std::endl;
    }

    void FileSize()
    {
        std::cout << FileUtilities::getFileSize(testFilePath) << std::endl;
        std::cout << FileUtilities::getFileSizeFS(testFilePath) << std::endl;
    }

    void WriteToFile()
    {
        int32_t _ = FileUtilities::WriteToFile(testFilePath, "12345");
    }

    void AppendToFile()
    {
        int32_t bytesWriten = FileUtilities::AppendToFile(testFilePath, "12345");
        std::cout << bytesWriten << std::endl;
    }
}

namespace Base64Tests
{
    void Test()
    {
        //const std::string result = Base64::base64Encode("111111111122222222222223333333333333");
        const std::string result = Base64::base64Encode2("Man");
        std::cout << result << std::endl;

    }

}

namespace TimeMeasurement
{
    using namespace PerfUtilities;

    void doSomething()
    {
        std::this_thread::sleep_for(std::chrono::milliseconds (150UL));
    }

    void testScopedTimer()
    {
        ScopedTimer timer { "Test1" };
        doSomething();
    }

    void testScopedTimer_TSC()
    {
        TSCScopedTimer timer { "Test2" };
        doSomething();
    }
}


// TODO: BitUtils
//      - check bit is set
//      - set bit
//      - unset bit
//      - check is Odd
//      - check is Even

int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    const std::vector<std::string_view> args(argv + 1, argv + argc);

    // StringUtilitiesTests::split_test_1();
    // StringUtilitiesTests::strip_string_test();
    // StringUtilitiesTests::trim_string_test();
    // StringUtilitiesTests::remove_chars_from_string_test();
    // StringUtilitiesTests::Update_string_test();
    // StringUtilitiesTests::Random_String();

    // FileUtilities_Tests::ReadFile();
    // FileUtilities_Tests::ReadFile2String();
    // FileUtilities_Tests::FileSize();

    // FileUtilities_Tests::WriteToFile();
    // FileUtilities_Tests::AppendToFile();

    // Base64Tests::Test();

    // CSV_Reader_Tests::TestAll();

    // HexConverter::TestAll();

    TimeMeasurement::testScopedTimer();
    TimeMeasurement::testScopedTimer_TSC();

    return EXIT_SUCCESS;
}
