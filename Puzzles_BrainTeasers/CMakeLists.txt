cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Puzzles_BrainTeasers)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

include_directories(${THIRD_PARTY_DIR}/cryptopp)
link_directories(${THIRD_PARTY_DIR}/cryptopp)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        False_Start.cpp
        No_Memory_CleanUp.cpp
        Class_Members_Creation_Order.cpp
        Lifetime_Global_and_Static_Variables.cpp
        Lifetime_Extension_Puzzle.cpp
        Assign_to_Function.cpp
        FunctionCall_Resolution_Templates.cpp
        Virtual_Func_in_Constructor.cpp
        Bad_Move_Constructor.cpp
        Shared_Ptr_Bad_Usage.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
)