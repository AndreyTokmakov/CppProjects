cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Tests)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

add_compile_options(-c -Wall -Wextra -O3 -std=c++23)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        Assembly/AssemblyDemo.cpp
        BinManipulation/BinManipulation.cpp BinManipulation/BinManipulation.h
        ExpressionTemplates/ExpressionTemplates.h ExpressionTemplates/ExpressionTemplates.cpp
        Punycode/Punycode.h Punycode/Punycode.cpp
        Encoding/Enconding.cpp Encoding/Enconding.h
        Encoding/URLDecoding.cpp Encoding/URLDecoding.h
        Encoding/Punycode.cpp Encoding/Punycode.h
        Encoding/StringUtils_ToStudy.cpp
        Encoding/StringUtils.cpp Encoding/StringUtils.h
        Encoding/Unicode.cpp Encoding/Unicode.h
        Crow/Crow.cpp Crow/Crow.h Crow/Logger.cpp Crow/Logger.h Crow/Utils.cpp Crow/Utils.h
        Geometry/PointsAndLines.cpp Geometry/PointsAndLines.h
        Encoding/PunycodeFinal.cpp Encoding/PunycodeFinal.h
        Encoding/Convertaion_UTF8_UTF32.cpp Encoding/Convertaion_UTF8_UTF32.h
        CopyElision_RVO/CopyElision_RVO.cpp CopyElision_RVO/CopyElision_RVO.h
        Templates_Metaprogramming/Templates.cpp Templates_Metaprogramming/Templates.h
        DebugLogger/DebugLogger.cpp DebugLogger/DebugLogger.h
        Collections/CollectionsTests.cpp Collections/CollectionsTests.h
        ObjectOrientedExperimetns/RAIIWrapper.cpp ObjectOrientedExperimetns/RAIIWrapper.h
        Algorithms/Algorithms.cpp Algorithms/Algorithms.h
        ConstexprMap/ConstexprMap.cpp ConstexprMap/ConstexprMap.h
        Concepts/Concepts.cpp Concepts/Concepts.h
        Multithreading/Multithreading.cpp Multithreading/Multithreading.h
        Multithreading/Queue.cpp Multithreading/Queue.h
        DesignPatterns/DesignPatterns.cpp DesignPatterns/DesignPatterns.h
        Iterators/Iterators.cpp Iterators/Iterators.h
        Files_Filesystem/Files.cpp Files_Filesystem/Files.h
        Files_Filesystem/FileUtilities.cpp Files_Filesystem/FileUtilities.h
        Helpers/Wrapper.h Helpers/UnorderedPair.h Helpers/Helpers.h Helpers/ScopedTimer.h
        Strings/Strings.cpp Strings/Strings.h
        Strings/StringUtilities.cpp Strings/StringUtilities.h
        DataStructures/LRUCache.cpp DataStructures/LRUCache.h
        DataStructures/DVector.cpp DataStructures/DVector.h
        DataStructures/EventLoop.cpp DataStructures/EventLoop.h
        DataStructures/MaxStack.cpp DataStructures/MaxStack.h
        DataStructures/MinStack.cpp DataStructures/MinStack.h
        DataStructures/MinMaxStack.cpp DataStructures/MinMaxStack.h
        DataStructures/RingBuffer.cpp DataStructures/RingBuffer.h
        Performance/Performance.cpp Performance/Performance.h
        Memory/Memory.cpp Memory/Memory.h
        Memory/CustomStackAllocator.cpp Memory/CustomStackAllocator.h
        BinaryAnalyzer/BinaryAnalyzer.cpp BinaryAnalyzer/BinaryAnalyzer.h
        TestAssignments/TestAssignments.h  TestAssignments/TestAssignments.cpp
        TestAssignments/ThinkCell.cpp TestAssignments/Bullish.cpp
        TableFormatter/TableFormatter.cpp TableFormatter/TableFormatter.h
        Coroutines/Coroutines.cpp Coroutines/Coroutines.h
        Date_Time_Chrono/Date_Time_Chrono.cpp Date_Time_Chrono/Date_Time_Chrono.h
        Optional/Optional.cpp Optional/Optional.h
        Math/Math.cpp Math/Math.h
        ObjectOrientedExperimetns/OOP_Experiments.cpp ObjectOrientedExperimetns/OOP_Experiments.h
        Cpp23_Features/Cpp23_Features.cpp Cpp23_Features/Cpp23_Features.h
        Auto/AutoTests.cpp Auto/AutoTests.h
        ObjectOrientedExperimetns/VirtualTables.cpp ObjectOrientedExperimetns/VirtualTables.h
        Heap/Heap.cpp Heap/Heap.h
        Comparators/Comparators.cpp Comparators/Comparators.h
        FindMinMaxValues/FindMinMaxValues.cpp FindMinMaxValues/FindMinMaxValues.h
        DataStructures/RateLimiter.cpp DataStructures/RateLimiter.h
        DataStructures/BitReader.cpp DataStructures/BitReader.h
        FunctionCall_LookUp/FunctionCall_LookUp.cpp FunctionCall_LookUp/FunctionCall_LookUp.h
        Memory/AlignedStackAllocator.cpp Memory/AlignedStackAllocator.h
        Memory/UsingCustomAllocator_List.cpp Memory/UsingCustomAllocator_List.h
        Logging/LowLatencyLogger.cpp
        Logging/LowLatencyLogger.h
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        # tbb
)