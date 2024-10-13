cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(OpenSSL)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

SET(OPENSSL_ROOT_DIR ${THIRD_PARTY_DIR}/openssl)

link_directories(${OPENSSL_ROOT_DIR})
include_directories("${OPENSSL_ROOT_DIR}/include")

# #[===[
find_package(OpenSSL REQUIRED)
message (STATUS "OPENSSL_FOUND: ${OPENSSL_FOUND}")
message (STATUS "   OPENSSL_INCLUDE_DIR: ${OPENSSL_INCLUDE_DIR}")
message (STATUS "   OPENSSL_CRYPTO_LIBRARIES: ${OPENSSL_CRYPTO_LIBRARIES}")
message (STATUS "   OPENSSL_SSL_LIBRARY: ${OPENSSL_SSL_LIBRARY}")
message (STATUS "   OPENSSL_SSL_LIBRARIES: ${OPENSSL_SSL_LIBRARIES}")
message (STATUS "   OPENSSL_LIBRARIES: ${OPENSSL_LIBRARIES}")
message (STATUS "   OPENSSL_VERSION: ${OPENSSL_VERSION}")
message (STATUS "   OPENSSL_APPLINK_SOURCE: ${OPENSSL_APPLINK_SOURCE}")
# ]===]

add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        CertificateGenerator.cpp
        Encryption.cpp
)

 # Link phread library
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        rt
        crypto
        # ssl
)