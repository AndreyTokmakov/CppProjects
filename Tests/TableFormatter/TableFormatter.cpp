/**============================================================================
Name        : TableFormatter.cpp
Created on  : 02.07.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : TableFormatter experiments
============================================================================**/

#include "TableFormatter.h"

#include <iostream>
#include <string>
#include <vector>

namespace TableFormatter
{

    struct ColumnInfo
    {
        std::string name {};
        size_t width {0};

        ColumnInfo() = default;

        ColumnInfo(std::string n, size_t w = 0):
                name { std::move(n) }, width {w} {
        }
    };

    // TODO:
    //  1. AddHeader()
    //     - Should have priority over columns number (if greater)
    //     - Should support [name, length]
    //  2. Support word-wrap inside one cell
    //  3. Support colors
    struct TablePrintFormatter
    {
        using ParamType = std::string;
        using Line = std::vector<ParamType>;
        using Table = std::vector<Line>;

        // TODO:
        //  1. headers --> std::optional ?
        //  2. combine 'headers' and 'columns' into one collection? columnsMetaInfo ??
        std::vector<ColumnInfo> headers {};
        std::vector<ColumnInfo> columns {};
        Table tableData;

        void setHeader(const Line& header)
        {
            headers.reserve(header.size());
            for (const ParamType& param: header)
                headers.emplace_back(param);
        }

        void setHeader(const std::vector<ColumnInfo>& header) {
            headers = header;
        }

        void setHeader(std::vector<ColumnInfo>&& header) {
            headers = std::move(header);
        }

        void addLine(const Line& line)
        {
            // TODO: emplace ??? use return value
            tableData.push_back(line);

            columns.resize(std::max(columns.size(), line.size()));
            for (size_t idx = 0; idx < line.size(); ++idx)
            {
                const auto& value { line[idx] };
                columns[idx].width = std::max(columns[idx].width, value.length());
            }
        }

        [[nodiscard]]
        std::vector<std::string> splitValue(const std::string& text,
                                            const size_t maxLen) const
        {
            std::vector<std::string> parts;
            for (size_t start = 0; true;)
            {
                if ((start + maxLen) < text.size())
                    parts.emplace_back(text, start, maxLen);
                else {
                    parts.emplace_back(text, start, text.size() - start);
                    break;
                }
                start += maxLen;
            }
            return parts;
        }

        // TODO: Place to center
        void printRow(const Line& line,
                      const size_t columnsCount) const
        {
            std::cout << "| ";
            for (size_t colID = 0; colID < columnsCount; ++colID)
            {
                std::cout.width(columns[colID].width);
                const std::string value = line.size() > colID ? line[colID] : std::string{};
                std::cout << value << " | ";
            }
            std::cout << "\n";
        }

        void printRow2(const Line& line,
                       const size_t columnsCount) const
        {
            std::vector<Line> wordWrappedLines { line };
            for (size_t columnID = 0; columnID < columnsCount; ++columnID)
            {
                const size_t width = columns[columnID].width;
                const std::string value = line.size() > columnID ? line[columnID] : std::string{};
                const size_t parts = value.size() / width + 1;
                if (parts > 1) {
                    wordWrappedLines.resize(parts);
                    const std::vector<std::string> valueWrappedParts = splitValue(value, width);
                    for (size_t n = 0; n < valueWrappedParts.size(); ++n)
                    {
                        Line& wrappedRow = wordWrappedLines[n];
                        wrappedRow.resize(columnsCount);
                        wrappedRow[columnID] = valueWrappedParts[n];
                    }
                }
            }

            for (const auto& row: wordWrappedLines)
            {
                printRow(row, columnsCount);
            }

            /*
            std::cout << "| ";
            for (size_t colID = 0; colID < columnsCount; ++colID)
            {
                std::cout.width(columns[colID].width);
                const std::string value = line.size() > colID ? line[colID] : std::string{};
                std::cout << value << " | ";
            }
            std::cout << "\n";
            */
        }

        void printSeparatorLine(const size_t length) const noexcept
        {
            std::cout.width(length);
            std::cout.fill('-');
            std::cout << '-' << '\n';
            std::cout.fill(' ');
        };

        // TODO: Remove function
        void debugPrint() const noexcept
        {
            const size_t columnCount = headers.empty() ? tableData.size() : headers.size();
            std::cout << "columnCount: " << columnCount << std::endl;

            /*
            for (const ColumnInfo& column: columns)
                std::cout << column.width << " ";
            std::cout << std::endl;
            */

            std::cout << std::endl;
        }

        void print()
        {
            size_t tableWidth { 1 };
            if (!headers.empty())
            {
                if (headers.size() > columns.size())
                    columns.resize(headers.size());
                for (size_t idx = 0; idx < headers.size(); ++idx)
                {
                    size_t& width = columns[idx].width;
                    const ColumnInfo& header = headers[idx];

                    if (header.width) {
                        width = header.width;
                    } else {
                        width = std::max(header.name.length(), width);
                    }

                    tableWidth += width + 3;
                }

                printSeparatorLine(tableWidth);

                // TODO: Refactor ???
                std::cout << "| ";
                for (size_t colID = 0; const ColumnInfo& hdr: headers)
                {
                    std::cout.width(columns[colID++].width);
                    std::cout << hdr.name << " | ";
                }
                std::cout << "\n";
            }

            // FIXME:
            const size_t columnToPrint = headers.empty() ? columns.size() : headers.size();
            printSeparatorLine(tableWidth);
            for (const auto& row: tableData)
            {
                printRow2(row, columnToPrint);
            }
            printSeparatorLine(tableWidth);
        }
    };

    void print()
    {
        TablePrintFormatter tbl;

        const std::string someLongStr {"Jonheee dddddddddddddddddddddddddddddddd4"};
        const std::string shortStr {"Jonh"};

        tbl.addLine({"Jonh", "Dow", "Male", "31", "1", "2"});
        tbl.addLine({ someLongStr, "Dow1", "Male", "31"});
        tbl.addLine({"Jon", "Dowr1", "Male"});
        tbl.addLine({"Jon", "Dowr2", "Male", "2323232", "One", "Two"});

        // tbl.setHeader(TablePrintFormatter::Line{"First name", "Second name"});
        tbl.setHeader(std::vector<ColumnInfo>{{"First name", 30}, {"Second name", 15}});

        // return tbl.debugPrint();

        std::cout << std::endl;
        tbl.print();
    }

    void experiments()
    {
        constexpr size_t maxLen { 8 };
        std::string text { "1111111122222222333333334" };

        std::vector<std::string> parts;
        size_t start = 0;
        while (true)
        {
            if ((start + maxLen) < text.size())
                parts.emplace_back(text, start, maxLen);
            else {
                parts.emplace_back(text, start, text.size() - start);
                break;
            }
            start += maxLen;
        }

        for (const std::string& str: parts)
            std::cout << str << std::endl;
    }
}

// TODO:
//  1. AddHeader()
//     - Should have priority over columns number (if greater)
//     - Should support [name, length]
//  2. Support word-wrap inside one cell
//  3. Support colors
//  4. Print line numbers ??

void TableFormatter::TestAll()
{
    TableFormatter::print();
    // TableFormatter::experiments();
};
