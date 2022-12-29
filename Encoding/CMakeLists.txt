cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Encoding)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(Encoding 
    main.cpp
        Base64/Base64.cpp Base64/Base64.h URL/URLEncoding.cpp URL/URLEncoding.h)

 # Link phread library
TARGET_LINK_LIBRARIES(Encoding pthread)