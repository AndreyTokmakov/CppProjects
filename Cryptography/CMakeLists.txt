cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Cryptography)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

include_directories(${THIRD_PARTY_DIR}/cryptopp)
link_directories(${THIRD_PARTY_DIR}/cryptopp)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        experiments/sha256_Original.cpp
        experiments/Sha256.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        -lcryptopp
)