//============================================================================
// Name        : DateAndTime.h
// Created on  : 02.04.2022
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : DateAndTime
//============================================================================

#include "DateAndTime.h"

#include <charconv>
#include <iostream>
#include <string_view>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>

namespace DateAndTime
{
    void Asctime()
    {
        std::time_t result = std::time(nullptr);
        std::cout << std::asctime(std::localtime(&result)) << result << " seconds since the Epoch" << std::endl;
    }

    void StringToDate_1() {
        constexpr std::string_view timeStr {"2022:04:02 09:06:00"};
        tm tm1;
        sscanf(timeStr.data(), "%4d:%2d:%2d %2d:%2d:%2d",
               &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday,
               &tm1.tm_hour, &tm1.tm_min, &tm1.tm_sec);

        std::cout << tm1.tm_year << '\n'
                  << tm1.tm_mon << '\n'
                  << tm1.tm_mday << '\n'
                  << tm1.tm_hour << '\n'
                  << tm1.tm_min << '\n'
                  << tm1.tm_sec << std::endl;
    }

    void StringToTime_1(std::string s)
    {
        char buf[11];
        int a, b, c;
        sscanf(s.c_str(), "%d:%d:%d", &a, &b, &c);
        std::cout << a << "-" << b << "-" << c << std::endl;
    }

    void StringToDate_3()
    {
        constexpr std::string_view timeStr {"Tue 10/30/2001 10:59:10 AM"};
        tm dateTime {};
        strptime(timeStr.data(), "%a %m/%d/%Y %r", &dateTime);

        std::cout << dateTime.tm_year << '\n'
                  << dateTime.tm_mon << '\n'
                  << dateTime.tm_mday << '\n'
                  << dateTime.tm_hour << '\n'
                  << dateTime.tm_min << '\n'
                  << dateTime.tm_sec << std::endl;
    }


    void StringToTime_Manual_1(std::string_view str) {
        std::array<int, 4> date {};
        size_t pos { 0 }, prev { 0 }, idx {0};
        while ((pos = str.find_first_of(":.", prev)) != std::string::npos) {
            const auto [ptr, errCode] { std::from_chars(str.data() + prev, str.data() + pos, date[idx++]) };
            prev = pos + 1;
        }
        const auto [ptr, errCode] { std::from_chars(str.data() + prev, str.data() + str.length() - prev, date[idx++]) };

        for (auto i: date)
            std::cout << i << std::endl;
    }

    void StringToTime_Manual_2(std::string_view str) {
        std::array<int, 4> date {};
        for (size_t first = 0, idx {0}; first < str.size(); ) {
            const size_t second = str.find_first_of(":.", first);
            if (first != second)
                const auto [ptr, errCode] {
                        std::from_chars(str.data() + first, str.data() + second - first, date[idx++]) };
            if (second == std::string_view::npos)
                break;
            first = second + 1;
        }

        for (auto i: date)
            std::cout << i << std::endl;
    }


    size_t timestampToInt(std::string_view str) {
        std::array<int, 4> date {};
        for (size_t first = 0, idx {0}; first < str.size(); ) {
            const size_t second = str.find_first_of(":.", first);
            if (first != second)
                const auto [ptr, errCode] {
                        std::from_chars(str.data() + first, str.data() + second - first, date[idx++]) };
            if (second == std::string_view::npos)
                break;
            first = second + 1;
        }
        return static_cast<size_t>(date[0] * 3600 + date[1] * 60 + date[2]) * 1'000'000 + date[3];
    }

    void Timestamp2Integer() {
        constexpr std::string_view timeStr {"12:13:30.410000"};
        auto ms = timestampToInt(timeStr);
        std::cout <<  1648846800000 + ms << std::endl;
    }
};


namespace DateAndTime::Performance
{

#define START_TIME_MEASURE auto start = std::chrono::high_resolution_clock::now();
#define STOP_TIME_MEASURE  { auto end = std::chrono::high_resolution_clock::now(); \
                           auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
						   std::cout << "Result: " << duration << " microseconds" << std::endl;}

    std::array<int, 4> str2Date1(std::string_view str) {
        std::array<int, 4> date {};
        size_t pos { 0 }, prev { 0 }, idx {0};
        while ((pos = str.find_first_of(":.", prev)) != std::string::npos) {
            const auto [ptr, errCode] { std::from_chars(str.data() + prev, str.data() + pos, date[idx++]) };
            prev = pos + 1;
        }
        const auto [ptr, errCode] {
            std::from_chars(str.data() + prev, str.data() + str.length() - prev, date[idx++]) };
        return date;
    }

    std::array<int, 4> str2Date2(std::string_view str) {
        std::array<int, 4> date {};
        for (size_t first = 0, idx {0}; first < str.size(); ) {
            const size_t second = str.find_first_of(":.", first);
            if (first != second)
                const auto [ptr, errCode] {
                        std::from_chars(str.data() + first, str.data() + second - first, date[idx++]) };
            if (second == std::string_view::npos)
                break;
            first = second + 1;
        }
        return date;
    }

    void Test()
    {
        constexpr size_t COUNT {10'000'000};
        constexpr std::string_view timeStr {"11:13:35.0400123"};

        {
            START_TIME_MEASURE
            for (size_t i = 0; i < COUNT; i++)
                auto date = str2Date1(timeStr);
            STOP_TIME_MEASURE
        }
        {
            START_TIME_MEASURE
            for (size_t i = 0; i < COUNT; i++)
                auto date = str2Date2(timeStr);
            STOP_TIME_MEASURE
        }
    }
}


void DateAndTime::TestAll() {
    // Asctime();
    // StringToDate_1();
    // StringToDate_3();
    // StringToTime_1("11:16:12");
    Timestamp2Integer();

    // StringToTime_Manual_1("11:13:35.0400123");
    // StringToTime_Manual_2("11:13:35.0400123");

    // Performance::Test();
};

