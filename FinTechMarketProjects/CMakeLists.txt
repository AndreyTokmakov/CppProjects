cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(FinTechMarketProjects)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 26)

find_package(Boost REQUIRED COMPONENTS
        log_setup
        log
        filesystem
        program_options
)

add_compile_options(-c -Wall -Wextra -O3 -std=c++26 )

include_directories(Utilities)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        Experiments/Experiments.cpp Experiments/Experiments.h
        MatchingOrderBook/MatchingOrderBook.cpp MatchingOrderBook/MatchingOrderBook.h
        MatchingOrderBookEx/MatchingOrderBookEx.cpp MatchingOrderBookEx/MatchingOrderBookEx.h
        OrderBook/OrderBook.cpp OrderBook/OrderBook.h
        OrderBook2/OrderBook2.cpp OrderBook2/OrderBook2.h
        OrderBook3/OrderBook3.cpp OrderBook3/OrderBook3.h
        Utilities/Utilities.cpp Utilities/Utilities.h
        DaVinchiTest/DaVinchiTest.cpp DaVinchiTest/DaVinchiTest.h
        DaVinchiTest/DaVinchiTest_LinkedList.cpp DaVinchiTest/DaVinchiTest_LinkedList.h
        TestTask_OrderBook/TestTask_OrderBook.cpp TestTask_OrderBook/TestTask_OrderBook.h
        DataParsers/DataParsers.cpp  DataParsers/DataParsers.h
        NEW/OrderBookNew.h NEW/OrderBookNew.cpp
        Asymptoto/Asymptoto.cpp Asymptoto/Asymptoto.h

        MatchingEngine/MatchingEngine.h
        MatchingEngine/Order.cpp MatchingEngine/Order.h
        MatchingEngine/MatchingEngine.cpp
        MatchingEngine/MatchingEngineWithAlloc.cpp

        Framework/DataProvider.h Framework/DataProvider.cpp Framework/Types.h
        Framework/OrderBook.h Framework/OrderBook.cpp
        Framework/AppMain.h Framework/AppMain.cpp
        Framework/Common/Types.h
        Framework/Memory/MemoryPool.h Framework/Memory/StackAllocator.h
        Framework/Tests/Tests.cpp Framework/Tests/Tests.h Framework/Tests/Wrapper.h

        Utilities/Utilities.cpp Utilities/Utilities.h
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Utilities")
target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/Utilities")


TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        utils
        pthread
        ${Boost_LIBRARIES}
)
