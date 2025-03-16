cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 26)

project(Coroutines)
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

# add_compile_options(-c -Wall -Wextra -O3 -std=c++26)
add_compile_options(-c -Wall -O3 -std=c++26)

include_directories(${THIRD_PARTY_DIR}/tinycoro/include)
include_directories(${PROJECT_SOURCE_DIR}/std)
include_directories(${PROJECT_SOURCE_DIR}/utils)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        utils/Utilities.cpp
        TinyCoro/TinyCoro.cpp
        std/Coroutines.cpp
        std/generators/Generators.cpp

        std/experiments/Experiments.cpp
        std/experiments/Calculating_Average.cpp
        std/experiments/PinBall_Game.cpp
        std/experiments/Generic_TaskBased_Coroutine.cpp
        std/experiments/FileReader.cpp
        std/experiments/TaskCoordination.cpp
        std/experiments/Event_Processor.cpp

        std/simple_examples/Coroutine_Lifecycle_CoReturn.cpp
        std/simple_examples/Coroutine_Lifecycle_CoAwait.cpp

        std/simple_examples/Resuming_Coroutine_1.cpp

        std/simple_examples/Returning_Coroutine.cpp
        std/simple_examples/Returning_Coroutine_2.cpp

        std/simple_examples/Waitable_Coroutine.cpp
        std/simple_examples/Waitable_Coroutine_2.cpp
        std/simple_examples/Waitable_Coroutine_Update_Promise_State.cpp

        std/simple_examples/Multiple_Awaiters_Resolution.cpp
        std/simple_examples/Multiple_Awaiters_Resolution_2.cpp

        std/simple_examples/Yield_Coroutine.cpp
        std/simple_examples/Yield_Coroutine_Values_from_List.cpp

        std/experiments/Waitable_Coroutine_With_Mutex.cpp
        std/simple_examples/Awaiter_Lifecycle_Steps.cpp
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME}
        pthread
        ${EXTRA_LIBS}
)