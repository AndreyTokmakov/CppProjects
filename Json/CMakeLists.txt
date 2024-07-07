cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Json)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

include_directories(${THIRD_PARTY_DIR}/jsoncpp/include)
include_directories(${THIRD_PARTY_DIR}/json/include)
include_directories(${THIRD_PARTY_DIR}/rapidjson/include)

link_directories(${THIRD_PARTY_DIR}/jsoncpp/build/lib)
link_directories(${THIRD_PARTY_DIR}/jsoncpp/build/lib)


include_directories(Utils)

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        RapidJson/RapidJson.cpp RapidJson/RapidJson.h
        Nlohmann/Nlohmann.cpp Nlohmann/Nlohmann.h
        Utils/FileUtilities.cpp Utils/FileUtilities.h
        Utils/PertTools.cpp Utils/PertTools.h
        PerformanceTests.cpp  PerformanceTests.h
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
        -ljsoncpp
)