/**============================================================================
Name        : SQLite.cpp
Created on  : 05.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "SQLite.h"

#include <iostream>
#include <memory>
#include <optional>
#include <chrono>

#include <sqlite3.h>


namespace SQLite
{
    constexpr std::string_view dbPath { "/tmp/database.db" };

    struct UserDTO
    {
        explicit UserDTO(std::string name, std::string description):
            name { std::move(name) }, description { std::move(description) } {
        }

        uint32_t id {0};
        std::string name {};
        std::string description {};
        std::chrono::system_clock::time_point timestamp { std::chrono::system_clock::now() };

        std::vector<std::string> dataParts;
    };


    void Test()
    {
        const std::unique_ptr<sqlite3, decltype(&sqlite3_close)> database { nullptr, sqlite3_close };
        if (sqlite3* ptr = database.get(); SQLITE_OK != sqlite3_open(dbPath.data(), &(ptr)))
        {
            std::cerr << "Failed to open database " << dbPath << std::endl;
        }
    }

}


void SQLite::TestAll()
{
    Test();
}

