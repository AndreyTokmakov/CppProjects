cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(FinTechMarketProjects)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(FinTechMarketProjects 
    main.cpp
    Experiments/Experiments.cpp Experiments/Experiments.h
    MatchingOrderBook/MatchingOrderBook.cpp MatchingOrderBook/MatchingOrderBook.h
    MatchingOrderBookEx/MatchingOrderBookEx.cpp MatchingOrderBookEx/MatchingOrderBookEx.h
    OrderBook/OrderBook.cpp OrderBook/OrderBook.h
    OrderBook2/OrderBook2.cpp OrderBook2/OrderBook2.h
        OrderBook3/OrderBook3.cpp OrderBook3/OrderBook3.h Utilities/Utilities.cpp Utilities/Utilities.h DaVinchiTest/DaVinchiTest.cpp DaVinchiTest/DaVinchiTest.h DaVinchiTest/DaVinchiTest_LinkedList.cpp DaVinchiTest/DaVinchiTest_LinkedList.h)

TARGET_LINK_LIBRARIES(FinTechMarketProjects pthread)