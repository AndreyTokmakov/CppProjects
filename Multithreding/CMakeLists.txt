cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Multithreding)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++2a)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        ThreadPoolBlocking/ThreadPoolBlocking.cpp ThreadPoolBlocking/ThreadPoolBlocking.h
        AsyncLaunch/AsyncLaunch.cpp AsyncLaunch/AsyncLaunch.h
        ThreadHelperUtilities/ThreadHelperUtilities.cpp ThreadHelperUtilities/ThreadHelperUtilities.h
        Atomic/Atomic.cpp Atomic/Atomic.h
        Atomic/Atomic_MemoryOrder.cpp Atomic/Atomic_MemoryOrder.h
        Atomic/AtomicFlag.cpp Atomic/AtomicFlag.h
        Atomic/Atomic_CompareExchange.cpp Atomic/Atomic_CompareExchange.h
        Atomic/Atomic_IsLockFree.cpp Atomic/Atomic_IsLockFree.h
        Barrier/Barrier.cpp Barrier/Barrier.h
        CallOnce/CallOnce.cpp CallOnce/CallOnce.h
        ConditionVariable/ConditionVariable.cpp ConditionVariable/ConditionVariable.h
        Exceptions_Handling/Exceptions_Handling.cpp Exceptions_Handling/Exceptions_Handling.h
        Future/Future.cpp Future/Future.h
        Integer/Integer.h
        JThreads/JThreads.cpp JThreads/JThreads.h
        Latch/Latch.cpp Latch/Latch.h
        ThreadSafeCollections/LockFreeQueue.cpp ThreadSafeCollections/LockFreeQueue.h
        ThreadSafeCollections/Multithreading_Collections.cpp ThreadSafeCollections/Multithreading_Collections.h
        ThreadSafeCollections/Queue.cpp ThreadSafeCollections/Queue.h
        ThreadSafeCollections/SingleConsumerProducerQueue.cpp ThreadSafeCollections/SingleConsumerProducerQueue.h
        Multithreading_Communication/Multithreading_Communication.cpp Multithreading_Communication/Multithreading_Communication.h
        Multithreading_Performance/Multithreading_Performance.cpp Multithreading_Performance/Multithreading_Performance.h
        PackagedTask/PackagedTask.cpp PackagedTask/PackagedTask.h
        ParallelExecutionAlgoritms/ParallelExecutionAlgoritms.cpp ParallelExecutionAlgoritms/ParallelExecutionAlgoritms.h
        Promise/Promise.cpp Promise/Promise.h
        Semaphore/Semaphore.cpp Semaphore/Semaphore.h
        Synchronization/Synchronization.cpp Synchronization/Synchronization.h
        ThreadPools/ThreadPools.cpp ThreadPools/ThreadPools.h
        Threads/Threads.cpp Threads/Threads.h
        ThreadPoolLimited/ThreadPoolLimited.cpp ThreadPoolLimited/ThreadPoolLimited.h
        FalseSharing/FalseSharingExperiments.cpp FalseSharing/FalseSharingExperiments.h
        SynchStream_Custom/SynchStream_Custom.cpp SynchStream_Custom/SynchStream_Custom.h
        SynchStream/SynchStream.cpp SynchStream/SynchStream.h
        PerformanceExperiments/PerformanceExperiments.cpp PerformanceExperiments/PerformanceExperiments.h
        SpinLock/SpinLock.h SpinLock/SpinLock.cpp
        Threads/StoppingThreads.h Threads/StoppingThreads.cpp
        Utilities/Utilities.h Utilities/Utilities.cpp Utilities/Wrapper.h
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
        pthread
)