/**============================================================================
Name        : Contracts.cpp
Created on  : 09.09.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Contracts.cpp
============================================================================**/

#include "Contracts.hpp"

#include <algorithm>
#include <complex>
#include <iostream>
#include <utility>
#include <vector>
#include <string>
#include <ranges>
#include <atomic>

namespace
{
    template<typename  T>
    std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
    {
        for (const auto& v : vec)
            os << v << " ";
        return os;
    }
}

namespace Contracts
{
    double calculateSquareRoot(double value)
        [[pre: value >= 0]]
    {
        return std::sqrt(value);
    }

    double divide(int numerator, int denominator)
        [[pre: denominator != 0]]
    {
        return numerator / denominator;
    }

    std::vector<int> getTopValue(const std::vector<int>& values, const size_t size)
        [[pre: !values.empty() ]]
        [[pre: size > 0 && size <= values.size() ]]
    {
        std::cout << std::boolalpha << values.empty() << std::endl;

        std::vector<int> sorted = values;
        std::sort(sorted.begin(), sorted.end(), std::greater<int>());
        return std::vector<int>{sorted.begin(), sorted.begin() + size};
    }

    /**
    std::string normalizeString(std::string text)
        [[post: !std::ranges::any_of(r: result, isspace)]]
    {
        text.erase(std::remove_if(text.begin(), text.end(), isspace), text.end());
        return text;
    }
    **/

    void Divide_Test()
    {
        std::cout << divide(10, 2) << std::endl;
        std::cout << divide(10, 0) << std::endl;

        // 5
        // contract violation in function Contracts::divide at
        // /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/CppSTDLibrary/Contracts/Contracts.cpp:29: denominator != 0
        // terminate called without an active exception
    }


    void Precondition_GetVectorTopValue()
    {
        {
            std::vector<int> values {1, 2, 3, 4, 5};
            auto result = getTopValue(values, 3);
            std::cout << result << std::endl;
        }
        /*
        {
            std::vector<int> values {1, 2, 3, 4, 5};
            auto result = getTopValue(values, 32);
            std::cout << result << std::endl;
        } */
        {
            std::vector<int> values;
            auto result = getTopValue(values, 32);
            std::cout << result << std::endl;
        }
    }
}

namespace Contracts::Class_Methods
{
    enum class ConnectionState
    {
        Idle,
        Disconnected,
        Connected,
        Error
    };

    template<typename  T>
    std::ostream& operator<<(std::ostream& stream, const ConnectionState state)
    {
        switch (state) {
            case ConnectionState::Idle: return stream << "Idle";
            case ConnectionState::Disconnected: return stream << "Disconnected";
            case ConnectionState::Connected:return stream << "Connected";
            case ConnectionState::Error: return stream << "Error";
        }
        return stream;
    }

    struct QueryResult {};

    struct DatabaseConnection
    {
        ConnectionState state { ConnectionState::Idle };
        std::string connectionString;

    public:

        DatabaseConnection() = default;

        explicit DatabaseConnection(std::string connectionString) :
            connectionString(std::move(connectionString)) {
        }

        void Connect()
            [[pre: state == ConnectionState::Disconnected || state == ConnectionState::Idle ]]
            [[pre: !connectionString.empty() ]]
            [[post: state == ConnectionState::Connected || state == ConnectionState::Error ]]
        {
            try {
                state = ConnectionState::Connected;
                std::cout << "Connected to " << connectionString << std::endl;
            }
            catch (...) {
                state = ConnectionState::Error;
                std::cout << "Connection error" << std::endl;
            }
        }

        void Disconnect()
            [[pre: state == ConnectionState::Connected]]
            [[post: state == ConnectionState::Disconnected]]
        {
            state = ConnectionState::Disconnected;
            std::cout << "Disconnected" << std::endl;
        }

        QueryResult executeQuery(const std::string& query = std::string{})
            [[pre: state == ConnectionState::Connected || state == ConnectionState::Error]]
            [[pre: !query.empty()]]
        {
            std::cout << "Executing query: " << query << std::endl;
            return QueryResult {};
        }
    };

    void Test()
    {
        {
            DatabaseConnection connection("localhost");
            connection.Connect();
            connection.Disconnect();

            // Connected to localhost
            // Disconnected
        }
        {
            DatabaseConnection connection;
            connection.Connect();
            connection.executeQuery("SELECT * FROM table");

            // contract violation in function Contracts::Class_Methods::DatabaseConnection::Connect
            //      at /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/CppSTDLibrary/Contracts/Contracts.cpp:137: !connectionString.empty()
            // terminate called without an active exception

        }
    }

    void Test2()
    {
        DatabaseConnection connection("localhost");
        connection.Disconnect();

        // contract violation in function Contracts::Class_Methods::DatabaseConnection::Disconnect
        //   at /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/CppSTDLibrary/Contracts/Contracts.cpp:151: state == ConnectionState::Connected
        // terminate called without an active exception
    }

    void Test3()
    {
        DatabaseConnection connection("localhost");
        connection.Connect();
        // connection.executeQuery("SELECT * FROM table");
        connection.executeQuery();
        connection.Disconnect();

        // Connected to localhost
        // contract violation in function Contracts::Class_Methods::DatabaseConnection::executeQuery
        //    at /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/CppSTDLibrary/Contracts/Contracts.cpp:160: !query.empty()
        // terminate called without an active exception
    }
}


void Contracts::TestAll()
{
    // Divide_Test();
    // Precondition_GetVectorTopValue();

    // Class_Methods::Test();
    // Class_Methods::Test2();
    // Class_Methods::Test3();
}