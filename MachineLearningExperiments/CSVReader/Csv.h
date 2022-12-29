//============================================================================
// Name        : CSV.h
// Created on  : 26.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ CSV
//============================================================================

#ifndef CPPCLIONPROJECTS_CSV_H
#define CPPCLIONPROJECTS_CSV_H

#include <vector>
#include <string>

namespace Csv
{
    class CSVReader {
    public:
        std::vector<std::vector<std::string>> dataSet;

    public:
        void readFile(std::string_view filePath) noexcept;

    private:
        void split_string(const std::string& str,
                          std::vector<std::string>& cont,
                          std::string&& delimiter) const noexcept;
    };



    void TestAll();
}

#endif //CPPCLIONPROJECTS_CSV_H
