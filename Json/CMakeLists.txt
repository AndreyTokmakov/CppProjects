cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Json)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

include_directories(${THIRD_PARTY_DIR}/jsoncpp/include)
link_directories(${THIRD_PARTY_DIR}/jsoncpp/build/lib)

include_directories(${THIRD_PARTY_DIR}/json/include)
include_directories(${THIRD_PARTY_DIR}/rapidjson/include)

include_directories(Utils)

#[[
include(FetchContent)
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json
        GIT_TAG v3.12.0
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(json)
#]]

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

add_executable(${PROJECT_NAME}
        main.cpp
        RapidJson/RapidJson.cpp RapidJson/RapidJson.h
        Nlohmann/Nlohmann.cpp Nlohmann/Nlohmann.h
        Nlohmann/CustomType_ToAndFromJson.cpp Nlohmann/CustomType_ToAndFromJson.h
        Utils/FileUtilities.cpp Utils/FileUtilities.h
        Utils/PertTools.cpp Utils/PertTools.h
        PerformanceTests.cpp  PerformanceTests.h
)

target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/Utilities")
target_link_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/Utilities")

target_link_libraries(${PROJECT_NAME} PUBLIC
        -ljsoncpp
        utils
        pthread
)