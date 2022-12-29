cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(BlockChain)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(BlockChain 
    main.cpp
        cryptography/sha256_Original.h cryptography/sha256_Original.cpp cryptography/Sha256.cpp cryptography/Sha256.h)


TARGET_LINK_LIBRARIES(BlockChain)