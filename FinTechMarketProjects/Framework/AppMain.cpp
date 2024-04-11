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

template <typename T>
concept NumberConcept = std::is_arithmetic_v<T>;

void sink(const NumberConcept auto& value)
{

}


// INFO: MarkerDataProvider --> NIC (kernel bypass) --> OrderGatewayServer

// INFO: Иметь в OrderBook разные HashMap-s для диапазона SYMBOL которые будут обрабатываться
//       в ОДНОПОТОЧНОМ режиме --> в рамках потока из ThreadPool-а

// TODO:
//  • Basic Types:
//      - Side
//      - Order
//      - Concepts
//  • Financial protocol parsing
//      - FAST parser
//  • OrderGatewayServer:
//      - TCP Epoll Server | Netmap, DPDK ??
//  • MemPool: Create orders from the Pool
//     ---> Tests --> Different Namespace
//  • Logger
//      - Different handlers | ToFile && ToNetwork
//  • Database
//      - How efficiently store data
//      - Ro restore data
//  • OrderHandler
//      - Store and publish orders --> To the market ??
//  • Networking
//      - TCP Server | Epoll ()
//      - Multicast | Broadcast serer
//      - NetMap - ZeroCopy poller
//  • LockFree Queue's
//  • IPC <--> ZeroMQ ???
//  • FPGA ???? (https://www.mavensecurities.com/fpgas-in-trading/)

void AppMain::TestAll([[maybe_unused]] const std::vector<std::string_view>& args)
{
    using namespace Framework;

    /*
    OrderBook book;
    DataProvider dataProvider(book);
    dataProvider.setStream(dataFilePathPart);

    dataProvider.start();
    */

    sink(1);
    sink(1.23131222222222);

    // Tests::TestAll();
}
