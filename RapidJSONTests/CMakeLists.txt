cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(RapidJSONTests)
add_compile_options(-c -Wall -Werror -Wextra -O3 -std=c++2a)

SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
SET(PROJECT_VERSION 1.0.0.0)

include_directories(../../third_party/rapidjson/include)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# include all components
add_executable(${PROJECT_NAME} MACOSX_BUNDLE 
    main.cpp
)