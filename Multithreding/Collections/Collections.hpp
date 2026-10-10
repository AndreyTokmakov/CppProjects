/**============================================================================
Name        : Collections.hpp
Created on  : 10.10.2026
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Collections.hpp
============================================================================**/

#ifndef CPPPROJECTS_COLLECTIONS_HPP
#define CPPPROJECTS_COLLECTIONS_HPP

namespace collections
{
    void TestAll();

    namespace Queue { void TestAll(); }
    namespace RingBuffer { void TestAll(); }
    namespace BlockingQueue { void TestAll(); }
    namespace LockFreeQueue { void TestAll(); }
    namespace mpmc_queue { void TestAll(); }
    namespace SCSP_RingBuffer { void TestAll(); }
    namespace SCSP_RingBuffer_Blocking { void TestAll(); }
    namespace SingleConsumerProducerQueue { void TestAll(); }
    namespace ThreadSafeQueue_CV_vs_RingBuffer { void TestAll(); }
    namespace RingBuffer_vs_CVMutexQueue { void TestAll(); };
    namespace RingBuffer_vs_CVMutexQueue_2 { void TestAll(); };
    namespace RingBuffer_vs_CVMutexQueue_Debug { void TestAll(); };
}

#endif //CPPPROJECTS_COLLECTIONS_HPP
