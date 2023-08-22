//============================================================================
// Name        : main.cpp
// Created on  : April 22, 2019
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Threads testing application
//============================================================================

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds
#include <string>


#include "Semaphore/Semaphore.h"
#include "Latch/Latch.h"
#include "ThreadPools/ThreadPools.h"
#include "ThreadPoolBlocking/ThreadPoolBlocking.h"
#include "ThreadPoolLimited/ThreadPoolLimited.h"
#include "Atomic/Atomic.h"
#include "Promise/Promise.h"
#include "Future/Future.h"
#include "PackagedTask/PackagedTask.h"
#include "Threads/Threads.h"
#include "ConditionVariable/ConditionVariable.h"
#include "Synchronization/Synchronization.h"
#include "ThreadHelperUtilities/ThreadHelperUtilities.h"
#include "CallOnce/CallOnce.h"
#include "Integer/Integer.h"
#include "JThreads/JThreads.h"
#include "AsyncLaunch/AsyncLaunch.h"
#include "Exceptions_Handling/Exceptions_Handling.h"
#include "ParallelExecutionAlgoritms/ParallelExecutionAlgoritms.h"
#include "Multithreading_Communication/Multithreading_Communication.h"
#include "Multithreading_Collections/Multithreading_Collections.h"
#include "Multithreading_Performance/Multithreading_Performance.h"
#include "Barrier/Barrier.h"
#include "FalseSharing/FalseSharingExperiments.h"
#include "LockFreeDataStructures/LockFreeQueue.h"
#include "SynchStream_Custom/SynchStream_Custom.h"
#include "SynchStream/SynchStream.h"
#include "PerformanceExperiments/PerformanceExperiments.h"


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    // AsyncLaunch::TEST_ALL();
    // Atomic::TEST_ALL();
    // Barrier::TEST_ALL();
    // CallOnce::TEST_ALL();
    // ConditionVariable::TEST_ALL();
    // Exceptions_Handling::TEST_ALL();
    // Future::TEST_ALL();

    // Latch::TEST_ALL();
    // LockFreeQueue::TEST_ALL()
    // Multithreading_Performance::TEST_ALL();
    // Multithreading_Collections::TEST_ALL();
    // Multithreading_Communication::TEST_ALL();
    // Promise::TEST_ALL();
    // PackagedTask::TEST_ALL();
    // ParallelExecutionAlgoritms::TEST_ALL();
    // FalseSharingExperiments::TEST_ALL();
    Semaphore::TEST_ALL();
    // Synchronization::TEST_ALL();
    // SynchStream::TEST_ALL();
    // SynchStream_Custom::TEST_ALL();

    // ThreadPools::TEST_ALL();
    // ThreadPoolBlocking::TEST_ALL();
    // ThreadPoolBlocking::TEST_ALL();
    // ThreadPoolLimited::TestAll();

    // Threads::TEST_ALL();
    // JThreads::TEST_ALL();

    // PerformanceExperiments::TestAll();

    return EXIT_SUCCESS;
}