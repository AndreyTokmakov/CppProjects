cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(JsonCPP)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

include_directories(../../third_party/jsoncpp/include)
link_directories(../../third_party/jsoncpp/build/lib)
add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME}
    main.cpp
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
        -ljsoncpp
)