/**============================================================================
Name        : MySqlX.cpp
Created on  : 05.01.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include "MySqlX.h"

#include <iostream>
#include <string_view>
#include <mysqlx/xdevapi.h>

namespace MySqlX
{
    void selectExamples()
    {
        mysqlx::Session session("0.0.0.0", "admin", "qwerty12345", "users");

        try
        {
            mysqlx::SqlResult res = session.sql("show variables like 'version'").execute();
            std::stringstream version;
            version << res.fetchOne().get(1).get<std::string>();
            int major_version;
            version >> major_version;
            std::cout << "major_version: " << major_version << std::endl;

        } catch (const mysqlx::Error &error) {
            std::cerr << "Error: " << error.what() << std::endl;
        }

        std::cout << std::string(180, '=') << std::endl;

        try
        {
            for (mysqlx::SqlResult result = session.sql("SHOW DATABASES").execute();
                const mysqlx::Row &row : result)
            {
                std::cout << row[0].get<std::string>() << std::endl;
            }
        } catch (const mysqlx::Error &error) {
            std::cerr << "Error: " << error.what() << std::endl;
        }

        session.close();
    }

    void listUsers()
    {
        mysqlx::Session session("0.0.0.0", "admin", "qwerty12345", "users");
        try
        {
            for (mysqlx::SqlResult result = session.sql("select * from users").execute();
                const mysqlx::Row& row : result)
            {
                const int id { row[0].get<int>()};
                const std::string name { row[1].get<std::string>()};
                const std::string description { row[2].get<std::string>()};
                const int64_t timestamp { row[3].get<int64_t>()};

                std::cout << id << " | " << name << " | " << description << " | " << timestamp  << std::endl;
            }
        } catch (const mysqlx::Error &error) {
            std::cerr << "Error: " << error.what() << std::endl;
        }
        session.close();
    }
}

// GRANT ALL ON users.* TO 'admin'@'%';
void MySqlX::TestAll()
{

    // selectExamples();
    listUsers();
}
