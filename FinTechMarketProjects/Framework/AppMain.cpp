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

namespace AppMain
{

}

void AppMain::TestAll([[maybe_unused]] const std::vector<std::string_view>& args)
{


    using namespace Framework;

    OrderBook book;
    DataProvider dataProvider(book);
    // dataProvider.setStream();

    dataProvider.start();
}