cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Tests)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++23)

# include all components
add_executable(Tests MACOSX_BUNDLE 
    main.cpp
        ExpressionTemplates/ExpressionTemplates.h ExpressionTemplates/ExpressionTemplates.cpp
        Punycode/Punycode.h Punycode/Punycode.cpp
        Encoding/Enconding.cpp Encoding/Enconding.h
        Encoding/URLDecoding.cpp Encoding/URLDecoding.h
        Encoding/Punycode.cpp Encoding/Punycode.h
        CopyElision_RVO/CopyElision_RVO.cpp CopyElision_RVO/CopyElision_RVO.h
        Encoding/StringUtils_ToStudy.cpp
        Encoding/StringUtils.cpp Encoding/StringUtils.h
        Encoding/Unicode.cpp Encoding/Unicode.h
        Geometry/PointsAndLines.cpp Geometry/PointsAndLines.h
        Encoding/PunycodeFinal.cpp Encoding/PunycodeFinal.h
        Encoding/Convertaion_UTF8_UTF32.cpp Encoding/Convertaion_UTF8_UTF32.h
        Templates_Metaprogramming/Templates.cpp Templates_Metaprogramming/Templates.h
        DataStructures/LRUCache.cpp DataStructures/LRUCache.h DebugLogger/DebugLogger.cpp DebugLogger/DebugLogger.h Collections/CollectionsTests.cpp Collections/CollectionsTests.h ObjectOrientedExperimetns/RAIIWrapper.cpp ObjectOrientedExperimetns/RAIIWrapper.h
        Helpers/Long.h Algorithms/Algorithms.cpp Algorithms/Algorithms.h ConstexprMap/ConstexprMap.cpp ConstexprMap/ConstexprMap.h Multithreading/Multithreading.cpp Multithreading/Multithreading.h DesignPatterns/DesignPatterns.cpp DesignPatterns/DesignPatterns.h Iterators/Iterators.cpp Iterators/Iterators.h Files_Filesystem/Files.cpp Files_Filesystem/Files.h Helpers/UnorderedPair.h Helpers/Utilities.h Helpers/Object.h Strings/Strings.cpp Strings/Strings.h DataStructures/EventLoop.cpp DataStructures/EventLoop.h Performance/Performance.cpp Performance/Performance.h Memory/Memory.cpp Memory/Memory.h DataStructures/MaxStack.cpp DataStructures/MaxStack.h BinaryAnalyzer/BinaryAnalyzer.cpp BinaryAnalyzer/BinaryAnalyzer.h ThinkCell/ThinkCell.cpp ThinkCell/ThinkCell.h TableFormatter/TableFormatter.cpp TableFormatter/TableFormatter.h Coroutines/Coroutines.cpp Coroutines/Coroutines.h Date_Time_Chrono/Date_Time_Chrono.cpp Date_Time_Chrono/Date_Time_Chrono.h Optional/Optional.cpp Optional/Optional.h Math/Math.cpp Math/Math.h ObjectOrientedExperimetns/OOP_Experiments.cpp ObjectOrientedExperimetns/OOP_Experiments.h Cpp23_Features/Cpp23_Features.cpp Cpp23_Features/Cpp23_Features.h Auto/AutoTests.cpp Auto/AutoTests.h ObjectOrientedExperimetns/VirtualTables.cpp ObjectOrientedExperimetns/VirtualTables.h)

 # Link phread library
TARGET_LINK_LIBRARIES(Tests pthread)