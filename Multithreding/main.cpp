/**============================================================================
Name        : main.cpp
Created on  : 11.22.2019
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Multithreading tests main entry point
============================================================================**/

#include "Semaphore/Semaphore.h"
#include "Latch/Latch.h"
#include "ThreadPools/ThreadPools.h"
#include "ThreadPools/ThreadPoolsEx.h"

#include "Threads/Threads.h"
#include "Threads/StoppingThreads.h"

#include "ThreadPoolBlocking/ThreadPoolBlocking.h"
#include "ThreadPoolLimited/ThreadPoolLimited.h"

#include "Atomic/Atomic.h"
#include "Atomic/Atomic_Bool.h"
#include "Atomic/Atomic_Wait.h"
#include "Atomic/Atomic_MemoryOrder.h"
#include "Atomic/AtomicFlag.h"
#include "Atomic/Atomic_CompareExchange.h"
#include "Atomic/Atomic_IsLockFree.h"
#include "Atomic/AtomicPerformanceTests.h"

#include "Promise/Promise.h"
#include "Future/Future.h"
#include "PackagedTask/PackagedTask.h"
#include "ConditionVariable/ConditionVariable.h"
#include "Synchronization/Synchronization.h"
#include "CallOnce/CallOnce.h"
#include "Integer/Integer.h"
#include "JThreads/JThreads.h"
#include "AsyncLaunch/AsyncLaunch.h"
#include "Exceptions_Handling/Exceptions_Handling.h"
#include "ParallelExecutionAlgoritms/ParallelExecutionAlgoritms.h"
#include "Multithreading_Communication/Multithreading_Communication.h"
#include "Collections/Collections.h"

#include "PerformanceExperiments/Mutex_vs_AtomicLock.h"
#include "PerformanceExperiments/Multithreading_Performance.h"
#include "PerformanceExperiments/PerformanceExperiments.h"

#include "Barrier/Barrier.h"
#include "FalseSharing/FalseSharingExperiments.h"
#include "SynchStream_Custom/SynchStream_Custom.h"
#include "SynchStream/SynchStream.h"

#include "SpinLock/SpinLock.h"
#include "SpinLock/FairSpinLock.h"
#include "SpinLock/SpinLockFutex.h"
#include "SpinLock/SpinLock_PerformanceTests.h"

#include "Utilities/Utilities.h"
#include "Experiments/Experiments.h"


int main([[maybe_unused]] int argc,
         [[maybe_unused]] char** argv)
{
    // Experiments::TestAll();

    // AsyncLaunch::TestAll();

    // Atomic::TestAll();
    // AtomicFlag::TestAll();
    // Atomic_Bool::TestAll();
    // Atomic_Wait::TestAll();
    // Atomic_MemoryOrder::TestAll();
    // Atomic_CompareExchange::TestAll();
    // Atomic_IsLockFree::TestAll();
    // AtomicPerformanceTests::TestAll();

    // Barrier::TEST_ALL();
    // CallOnce::TEST_ALL();
    // ConditionVariable::TEST_ALL();
    // Exceptions_Handling::TEST_ALL();
    // Future::TEST_ALL();

    // Latch::TEST_ALL();


    // Multithreading_Communication::TEST_ALL();
    // Promise::TEST_ALL();
    // PackagedTask::TEST_ALL();
    // ParallelExecutionAlgoritms::TEST_ALL();

    // Semaphore::TestAll();

    // Synchronization::TEST_ALL();
    // SynchStream::TEST_ALL();
    // SynchStream_Custom::TEST_ALL();

    // SpinLock::TestAll();
    // FairSpinLock::TestAll();
    SpinLockFutex::TestAll();
    // SpinLock_PerformanceTests::TestAll();

    // StoppingThreads::TestAll();

    // ThreadPools::TestAll();
    // ThreadPoolsEx::TestAll();

    // ThreadPoolBlocking::TEST_ALL();
    // ThreadPoolLimited::TestAll();

    // Threads::TEST_ALL();
    // JThreads::TEST_ALL();

    /** Performance **/

    // Multithreading_Performance::TEST_ALL();
    // FalseSharingExperiments::TEST_ALL();
    // PerformanceExperiments::TestAll();

    /** ThreadSafeCollections **/

    // Multithreading_Collections::TestAll();
    // SingleConsumerProducerQueue::TestAll();
    // Queue::TestAll();
    // BlockingQueue::TestAll();
    // LockFreeQueue::TEST_ALL()
    // RingBuffer::TestAll();
    // SCSP_RingBuffer_Blocking::TestAll();
    // SCSP_RingBuffer::TestAll();
    // Collections::RingBuffer_vs_CVMutexQueue::TestAll();
    // Collections::RingBuffer_vs_CVMutexQueue_2::TestAll();
    // Collections::RingBuffer_vs_CVMutexQueue_Debug::TestAll();
    // ThreadSafeQueue_CV_vs_RingBuffer::TestAll();

    /** Utilities **/

    // Utilities::TestAll();

    return EXIT_SUCCESS;
}