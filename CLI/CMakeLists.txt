cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(CLI)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++23)

# include all components
add_executable(${PROJECT_NAME}
    main.cpp
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME}
        pthread
)