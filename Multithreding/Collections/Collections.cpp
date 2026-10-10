/**============================================================================
Name        : Collections.cpp
Created on  : 10.10.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Collections.cpp
============================================================================**/

#include "Collections.hpp"

void collections::TestAll()
{
    // ring_buffers::TestAll();
    // RingBuffer::TestAll();
    // Multithreading_Collections::TestAll();
    // SingleConsumerProducerQueue::TestAll();
    Queue::TestAll();
    // BlockingQueue::TestAll();
    LockFreeQueue::TestAll();
    // mpmc_queue::TestAll();
    // SCSP_RingBuffer_Blocking::TestAll();
    // SCSP_RingBuffer::TestAll();
    // ingBuffer_vs_CVMutexQueue::TestAll();
    // RingBuffer_vs_CVMutexQueue_2::TestAll();
    // RingBuffer_vs_CVMutexQueue_Debug::TestAll();
    // ThreadSafeQueue_CV_vs_RingBuffer::TestAll();

}
