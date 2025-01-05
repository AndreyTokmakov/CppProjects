cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 26)

project(Databases)
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

include(FetchContent)

# add_compile_options(-c -Wall -Wextra -O3 -std=c++26)
add_compile_options(-c -Wall -O3 -std=c++26)

include_directories(${THIRD_PARTY_DIR}/mysql-connector-cpp/include)
include_directories(${THIRD_PARTY_DIR}/mysql-connector-cpp/build/include/mysqlx)
link_directories(${THIRD_PARTY_DIR}/mysql-connector-cpp/build)
message(STATUS "MySQL: ${THIRD_PARTY_DIR}/mysql-connector-cpp/build")

message(STATUS "[dependency] Fetching PostgreSQL library (libpqxx)")
FetchContent_Declare(libpqxx
        GIT_REPOSITORY https://github.com/jtv/libpqxx.git
        GIT_TAG 7.9.2
)
FetchContent_MakeAvailable(libpqxx)


message(STATUS "SQLite: ${THIRD_PARTY_DIR}/sqlite")
include_directories(${THIRD_PARTY_DIR}/sqlite)
link_directories(${THIRD_PARTY_DIR}/sqlite/.libs)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        MySQL/MySQL.cpp
        PostgreSQL/PostgreSQL.cpp
        MySQL/MySqlX.cpp
        MySQL/SQLite.cpp
)

message(STATUS "EXTRA_LIBS: ${EXTRA_LIBS}")

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        mysqlclient
        mysqlcppconn8
        pqxx
        sqlite3
        ${EXTRA_LIBS}
)