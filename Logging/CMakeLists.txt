cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Logging)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++23)

include_directories(${THIRD_PARTY_DIR}/quill/quill/include)
link_directories(${THIRD_PARTY_DIR}/quill/build/quill)

include_directories(${THIRD_PARTY_DIR}/spdlog/include)

include_directories(${THIRD_PARTY_DIR}/xtr/single_include)
link_directories(${THIRD_PARTY_DIR}/xtr/build/g++-14.2-lto-release)
# include_directories(${THIRD_PARTY_DIR}/fmt/include)

# include_directories(${THIRD_PARTY_DIR}/xtr/third_party/fmt/include)
# link_directories(${THIRD_PARTY_DIR}/xtr/third_party/fmt/build)

include_directories(Quill)

# find_package(quill REQUIRED)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        # Quill/Quill.cpp
        SPDLog/SPDLog.cpp
        SpdLogWrapper/SpdLogWrapper.cpp
        SpdLogWrapper/logger.hpp
        SpdLogWrapper/spdlog_logger.hpp
        SpdLogWrapper/spdlog_logger.cpp
        SpdLogWrapper/logger_factory.hpp
        SpdLogWrapper/logger_factory.cpp
        SpdLogWrapperEx/SpdLogWrapperEx.cpp
        # XTR/XTR.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PRIVATE
        pthread
        ${EXTRA_LIBS}
        # libquill.a
        # quill::quill
        # fmt
        systemd
)