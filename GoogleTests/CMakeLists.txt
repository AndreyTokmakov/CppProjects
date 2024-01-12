cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(GoogleTests)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_subdirectory(tests)
add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

list(APPEND APP_SOURCES
        ${CMAKE_CURRENT_LIST_DIR}/src/Utils.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/Utils.h
        ${CMAKE_CURRENT_LIST_DIR}/src/Utils2.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/Utils2.h
 )

add_library(myLib STATIC ${APP_SOURCES})

# include all components
add_executable(GoogleTests
        src/main.cpp
        ${APP_SOURCES}
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
        pthread
)