cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

enable_testing()
project(UnitTestsy)
add_compile_options(-c -Wall -Werror -Wextra -O3 -std=c++2a)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
include_directories(../src)

# include all components
add_executable(UnitTests
        main.cpp
        tests2.cpp
        fixtures.cpp
        ParametrizedTests.cpp)

target_link_libraries(UnitTests ${GTEST_LIBRARIES} pthread myLib)

