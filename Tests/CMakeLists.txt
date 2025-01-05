cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 26)

project(Tests)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# add_compile_options(-c -Wall -Wextra -O3 -std=c++26)
add_compile_options(-c -Wall -O3 -std=c++26)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        Assembly/AssemblyDemo.cpp
        BinManipulation/BinManipulation.cpp
        BitFlags/BitFlags.cpp
        Punycode/Punycode.cpp
        Encoding/Enconding.h
        Encoding/URLDecoding.cpp
        Encoding/Punycode.cpp
        Encoding/StringUtils_ToStudy.cpp
        Encoding/StringUtils.cpp
        Encoding/Unicode.cpp
        Crow/Crow.cpp Crow/Logger.cpp Crow/Utils.cpp
        Geometry/PointsAndLines.cpp
        Encoding/PunycodeFinal.cpp
        Encoding/Convertaion_UTF8_UTF32.cpp
        CopyElision_RVO/CopyElision_RVO.cpp
        Templates/Templates.cpp
        Templates/ExpressionTemplates.cpp
        DebugLogger/DebugLogger.cpp
        Collections/CollectionsTests.cpp
        ObjectOrientedExperimetns/RAIIWrapper.cpp
        Algorithms/Algorithms.cpp
        ConstexprMap/ConstexprMap.cpp
        Concepts/Concepts.cpp
        Multithreading/Multithreading.cpp
        Multithreading/Queue.cpp
        DesignPatterns/DesignPatterns.cpp
        Iterators/Iterators.cpp
        Files_Filesystem/Files.cpp
        Files_Filesystem/FileUtilities.cpp
        Helpers/UnorderedPair.h
        Strings/Strings.cpp
        Strings/StringUtilities.cpp
        Performance/Performance.cpp
        Memory/Memory.cpp
        Memory/CustomStackAllocator.cpp
        BinaryAnalyzer/BinaryAnalyzer.cpp
        TestAssignments/TestAssignments.cpp
        TestAssignments/ThinkCell.cpp
        TestAssignments/Bullish.cpp
        TableFormatter/TableFormatter.cpp
        Coroutines/Coroutines.cpp
        Date_Time_Chrono/Date_Time_Chrono.cpp
        Optional/Optional.cpp
        Math/Math.cpp
        ObjectOrientedExperimetns/OOP_Experiments.cpp
        Cpp23_Features/Cpp23_Features.cpp
        Cpp23_Features/StackTrace.cpp
        Auto/AutoTests.cpp
        ObjectOrientedExperimetns/VirtualTables.cpp
        Heap/Heap.cpp
        FindMinMaxValues/FindMinMaxValues.cpp
        FunctionCall_LookUp/FunctionCall_LookUp.cpp
        Memory/AlignedStackAllocator.cpp
        Memory/UsingCustomAllocator_List.cpp
        Memory/ObjectMemoryPool.cpp
        Logging/LowLatencyLogger.cpp
        Cpp23_Features/Execution.cpp
)

message(STATUS "EXTRA_LIBS: ${EXTRA_LIBS}")


TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        ${EXTRA_LIBS}
)