//============================================================================
// Name        : CSV.h
// Created on  : 26.11.2021
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : C++ CSV
//============================================================================

#include "Csv.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

namespace Csv
{
   void CSVReader::readFile(std::string_view filePath) noexcept {
       std::vector<std::string> lines;
       for (std::fstream file(filePath.data());
            std::getline(file, lines.emplace_back());) {
       }
       lines.pop_back();

       dataSet.reserve(lines.size());
       for (const auto &line: lines) {
           split_string(line, dataSet.emplace_back(), ",");
       }
   }

   void CSVReader::split_string(const std::string& str,
                                std::vector<std::string>& cont,
                                std::string&& delimiter) const noexcept {
       size_t pos = 0, prev = 0;
       while ((pos = str.find(delimiter, prev)) != std::string::npos) {
           cont.emplace_back(str, prev, pos - prev);
           prev = pos + delimiter.length();
       }
       cont.emplace_back(str, prev, str.length() - prev);
   }
}

