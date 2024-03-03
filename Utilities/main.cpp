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
#include <fstream>
#include <charconv>
#include <optional>

#include "StringUtilities.h"
#include "FileUtilities.h"

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
        int32_t bytesWriten = FileUtilities::WriteToFile(testFilePath, "12345");
    }

    void AppendToFile()
    {
        int32_t bytesWriten = FileUtilities::AppendToFile(testFilePath, "12345");
        std::cout << bytesWriten << std::endl;
    }
}


namespace CSV_Reader
{
    using Headers = std::map<std::string, uint16_t>;

    constexpr char delimiter { ',' };
    constexpr char dQuotesSymbol { '"' };
    constexpr char sQuotesSymbol { '\'' };

    struct Value
    {
        std::string data;

        template<typename ... Args>
        explicit Value(Args&& ... param): data { std::forward<Args>(param)... } {
        }

        friend std::ostream& operator<<(std::ostream& stream, const Value& val)
        {
            stream << val.data;
            return stream;
        }

        template<typename T>
        [[nodiscard]]
        std::optional<T> get() const
        {
            T value {};
            if (const auto [ptr, error_code] = std::from_chars(data.data(), data.data() + data.size(), value);
                std::errc{} != error_code) {
                return std::nullopt;
            }
            return value;
        }
    };

    struct Row
    {
        std::vector<Value> values {};
        Headers* headers {nullptr};

        explicit Row(Headers* headersPtr = nullptr): headers {headersPtr} {
        };

        [[nodiscard]]
        size_t size() const noexcept {
            return values.size();
        }

        template<class ... Args>
        void emplaceValue(Args&& ... params)
        {
            values.emplace_back(std::forward<Args>(params)...);
        }

        // TODO: return optional ?
        Value operator[](const std::string& hdr) const
        {
            if (nullptr == headers)
                return Value{}; // TODO: std::nullopt

            if (const auto iter = headers->find(hdr); headers->end() != iter)
               return values[iter->second];

           return Value{}; // TODO: std::nullopt
        }

        Value operator[](const size_t idx) const
        {
            return values[idx];
        }
    };

    struct CSVData
    {
        Headers headers;
        std::vector<Row> rows;

        [[nodiscard]]
        std::size_t size() const noexcept {
            return rows.size();
        }

        [[nodiscard]]
        bool hasHeaders() const noexcept
        {
            return !headers.empty();
        }
    };

    size_t parseLine(const std::string& line,
                     Row& parts)
    {
        if (line.empty())
            return 0;

        bool sQuotes { false },  dQuotes { false };
        size_t prev {0}, idx {0};
        for (; idx < line.size(); ++idx)
        {
            const char ch { line[idx] };
            if (dQuotesSymbol == ch) {
                dQuotes = !dQuotes;
            } else if (sQuotesSymbol == ch) {
                sQuotes = !sQuotes;
            } else if (delimiter == ch && !dQuotes && !sQuotes) {
                parts.emplaceValue(line, prev, idx - prev);
                prev = idx + 1;
                continue;
            }
        }
        parts.emplaceValue(line, prev, idx - prev);
        return parts.size();
    }

    CSVData readCsv(const std::filesystem::path& filePath,
                    bool skipHeader = false)
    {
        CSVData csvData {};
        if (std::ifstream file {filePath}; file.is_open() && file.good())
        {
            std::string line;

            // Reading the header line
            if (!skipHeader)
            {
                Row header;
                if (std::getline(file, line))
                    parseLine(line, header);
                else
                    return csvData;

                for (uint16_t idx {0}; Value& hdr: header.values)
                    csvData.headers[std::move(hdr.data)] = idx++;
            }

            // Read the remaining lines of the CSV file.
            while (std::getline(file, line)) {
                parseLine(line, csvData.rows.emplace_back(&csvData.headers));
            }
        }
        return csvData;
    }
}

namespace CSV_Reader_Tests
{
    using namespace CSV_Reader;



    void Test_ParseLine()
    {
        Row parts;
        const std::string line { R"(1,2.4,Jonh,Dow,"Street, 1-22-3",123,"my name is 'Max'")"};

        parseLine(line, parts);

        /*
        for (const auto& p: parts)
            std::cout << p << std::endl;
        */
    }

    void Test_ParseFile()
    {
        constexpr std::string_view csvFile { R"(../../Utilities/data/anime.csv)"};
        CSVData data = readCsv(csvFile);

        // for (const auto& [hdr, idx]: data.headers)
        //    std::cout << hdr << " = " << idx << std::endl;

        for (const Row& row: data.rows)
        {
            std::cout << row["anime_id"] << "    " <<  row["name"] << "  " << row["episodes"] << std::endl;
        }
    }

    void Value_Tests()
    {
        Value val {"123"};
        std::cout << val.get<int>().value() << std::endl;
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

    // CSV_Reader_Tests::Test_ParseLine();
    // CSV_Reader_Tests::Test_ParseFile();
    CSV_Reader_Tests::Value_Tests();

    return EXIT_SUCCESS;
}
