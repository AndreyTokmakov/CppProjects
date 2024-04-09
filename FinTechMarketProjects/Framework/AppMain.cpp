/**============================================================================
Name        : AppMain.cpp
Created on  : 24.03.2024
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : AppMain.cpp
============================================================================**/

#include "AppMain.h"
#include "DataProvider.h"
#include "OrderBook.h"
#include "Tests/Tests.h"

namespace
{
    constexpr std::string_view dataFilePath {"../../FinTechMarketProjects/data/orders.dat" };
    constexpr std::string_view dataFilePathPart {"../../FinTechMarketProjects/data/orders_part.csv" };
    constexpr std::string_view dataFilePathPartDebug {"../../FinTechMarketProjects/data/orders_part_debug.csv" };
    constexpr std::string_view dataFile_Test1 {"../../FinTechMarketProjects/data/orders_test_1.csv" };
    constexpr std::string_view dataFile_CancelTest {"../../FinTechMarketProjects/data/orders_test_cancel.csv" };

}

namespace AppMain
{

}



// TODO:
//  1. MemPool: Create orders from the Pool
//     ---> Tests --> Different Namespace
//  2. Logger
//  3. Networking
//  4. LockFree Queue's
//  5. IPC <--> ZeroMQ ???

void AppMain::TestAll([[maybe_unused]] const std::vector<std::string_view>& args)
{
    using namespace Framework;

    /*
    OrderBook book;
    DataProvider dataProvider(book);
    dataProvider.setStream(dataFilePathPart);

    dataProvider.start();
    */

    Tests::TestAll();

}
