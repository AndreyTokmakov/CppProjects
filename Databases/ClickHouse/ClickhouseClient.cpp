/**============================================================================
Name        : ClickhouseClient.cpp
Created on  : 21.06.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : ClickhouseClient.cpp
============================================================================**/

#include "ClickhouseClient.h"

#include <iostream>
#include <clickhouse/client.h>


namespace ClickhouseClient
{
    using namespace clickhouse;

    void Test()
    {
        Client client(ClientOptions().SetHost("0.0.0.0").SetUser("myuser").SetPassword("mypassword"));

        client.Select("select * from test.users", [] (const Block& block)
        {
            std::cout << block.GetRowCount() << " | " << block.GetColumnCount() << std::endl;
            //std::cout << block.GetColumnName(0) << std::endl;

            if (block.GetColumnCount()) {
                std::cout << block.GetColumnName(0) << " "
                          << block.GetColumnName(1) << " "
                          << block.GetColumnName(2) << std::endl;
            }

            /*
            for (size_t i = 0; i < block.GetRowCount(); ++i)
            {
                std::cout << "data" << std::endl;
                const std::shared_ptr<Column>& column = block[i];
                std::cout << column->As<ColumnString>() << std::endl;
            }*/

            /*
            for (size_t i = 0; i < block.GetRowCount(); ++i) {
                std::cout << block[0]->As<ColumnUInt64>()->At(i) << " "
                          << block[1]->As<ColumnString>()->At(i) << "\n";
            }*/
        });

    }

}


void ClickhouseClient::TestAll()
{
    ClickhouseClient::Test();
}