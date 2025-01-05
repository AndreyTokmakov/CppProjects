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
#include <filesystem>

#include <sqlite3.h>

namespace
{
    const std::string usersTableName = "users";
    const std::string TIME_FORMAT = "%Y-%m-%d %H:%M:%S";

}

namespace SQLite
{
    constexpr std::string_view dbPath { "/tmp/database.db" };

    using SqlLiteConnection = std::unique_ptr<sqlite3, decltype(&sqlite3_close)>;
    using SqlLiteStatement = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

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


    SqlLiteConnection establishConnection(const std::filesystem::path& dbPath)
    {
        SqlLiteConnection connection { nullptr, sqlite3_close };
        if (sqlite3* conn = nullptr; SQLITE_OK == sqlite3_open(dbPath.c_str(), &conn))
        {
            connection.reset(conn);
            return connection;
        }

        throw std::runtime_error("Could not open database '" + dbPath.string() + "'");
    }

    bool createTable(sqlite3* connection)
    {
        const std::string stmtStr = "CREATE TABLE " + usersTableName + " ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,name TEXT,description TEXT,timestamp BIGINT);";

        char* errorMessage { nullptr };
        if (SQLITE_OK != sqlite3_exec(connection, stmtStr.c_str(), nullptr, nullptr, &errorMessage))
        {
            // std::cerr << "Failed to create table: [" << sqlite3_errmsg(connection) << "]"<< std::endl;
            std::cerr << "Failed to create table: [" << errorMessage << "]"<< std::endl;
            sqlite3_free(errorMessage);
            return false;
        }
        return true;
    }

    bool insertUser(sqlite3* connection, const UserDTO& user)
    {
        const std::string stmtStr = std::string("INSERT INTO " + usersTableName +
            " (name,description,timestamp)  VALUES (?,?,?);");

        SqlLiteStatement statement { nullptr, &sqlite3_finalize };
        if (sqlite3_stmt* stmt = nullptr;
            SQLITE_OK == sqlite3_prepare_v2(connection, stmtStr.c_str(), -1, &stmt, nullptr))
        {
            statement.reset(stmt);

            sqlite3_bind_text(statement.get(), 1, user.name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(statement.get(), 2, user.description.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int64(statement.get(), 3, user.timestamp.time_since_epoch().count());

            if (SQLITE_DONE == sqlite3_step(statement.get()))
            {
                std::cout << "User created" << std::endl;
                return true;
            }
            else
            {
                std::cerr << "Failed to create user" << std::endl;
                return false;
            }
        }
        else
        {
            std::cerr << "Failed to prepare statement: [" << stmtStr << "]" << std::endl;
            return false;
        }
    }

}


void SQLite::TestAll()
{
    const SqlLiteConnection dbConnection  =establishConnection(dbPath);

    createTable(dbConnection.get());

    insertUser(dbConnection.get(), UserDTO("John", "Doe"));
    insertUser(dbConnection.get(), UserDTO("Max", "Doe"));
    insertUser(dbConnection.get(), UserDTO("Bill", "Doe"));
}

