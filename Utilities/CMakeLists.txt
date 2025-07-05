cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Utilities)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2c)

add_library(utils
        StringUtilities.cpp
        FileUtilities.cpp
        CSVReader/CSVReader.cpp
        CSVReader/Tests.cpp
        Base64.cpp
        HexConverter.cpp
        PerfUtilities.cpp
        ThreadUtilities.cpp
        DateTimeUtilities.cpp
)

add_executable(${PROJECT_NAME}
        main.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        utils
        pthread
        ${EXTRA_LIBS}
)