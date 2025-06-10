cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(Multithreding)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
set(PROJECT_VERSION 1.0.0.0)
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)


add_compile_options(-c -Wall -Wextra -O3 -std=c++26)

# include all components
add_executable(${PROJECT_NAME}
        main.cpp
        ThreadPoolBlocking/ThreadPoolBlocking.cpp
        AsyncLaunch/AsyncLaunch.cpp
        Atomic/Atomic.cpp
        Atomic/Atomic_MemoryOrder.cpp
        Atomic/AtomicFlag.cpp
        Atomic/Atomic_Wait.cpp
        Atomic/Atomic_CompareExchange.cpp
        Atomic/Atomic_IsLockFree.cpp
        Atomic/AtomicPerformanceTests.cpp
        Atomic/Atomic_Bool.cpp
        Barrier/Barrier.cpp
        CallOnce/CallOnce.cpp
        ConditionVariable/ConditionVariable.cpp
        Exceptions_Handling/Exceptions_Handling.cpp
        Future/Future.cpp
        JThreads/JThreads.cpp
        Latch/Latch.cpp
        Collections/LockFreeQueue.cpp
        Collections/Queue.cpp
        Collections/BlockingQueue.cpp
        Collections/SingleConsumerProducerQueue.cpp
        Collections/SCSP_RingBuffer.cpp
        Collections/SCSP_RingBuffer_Blocking.cpp
        Collections/RingBuffer.cpp
        Collections/RingBuffer_vs_CVMutexQueue.cpp
        Collections/RingBuffer_vs_CVMutexQueue_2.cpp
        Collections/RingBuffer_vs_CVMutexQueue_Debug.cpp
        Collections/ThreadSafeQueue_CV_vs_RingBuffer.cpp
        Multithreading_Communication/Multithreading_Communication.cpp
        PackagedTask/PackagedTask.cpp
        ParallelExecutionAlgoritms/ParallelExecutionAlgoritms.cpp
        Promise/Promise.cpp
        Semaphore/Semaphore.cpp
        Synchronization/Synchronization.cpp
        ThreadPools/ThreadPools.cpp
        ThreadPools/ThreadPoolsEx.cpp
        Threads/Threads.cpp
        ThreadPoolLimited/ThreadPoolLimited.cpp
        FalseSharing/FalseSharingExperiments.cpp
        SynchStream_Custom/SynchStream_Custom.cpp
        SynchStream/SynchStream.cpp
        SpinLock/SpinLock.cpp
        Threads/StoppingThreads.cpp
        Utilities/Utilities.cpp
        Utilities/Utilities_Tests.cpp
        PerformanceExperiments/Multithreading_Performance.cpp
        PerformanceExperiments/PerformanceExperiments.cpp
        PerformanceExperiments/ConditionVariable_vs_Atomic.cpp
        PerformanceExperiments/Mutex_vs_AtomicLock.cpp
        PerformanceExperiments/SpinLock.cpp
        Experiments/Experiments.cpp
)

# Link libraries to the project/executable
target_link_libraries(${PROJECT_NAME} PUBLIC
        pthread
)