/**============================================================================
Name        : Queue.h
Created on  : 02.06.2025
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : Queue.h
============================================================================**/

#ifndef CPP_PROJECTS_MULTITHREADING_COLLECTIONS_H
#define CPP_PROJECTS_MULTITHREADING_COLLECTIONS_H

namespace Collections::Queue { void TestAll(); }
namespace Collections::RingBuffer { void TestAll(); }
namespace Collections::BlockingQueue { void TestAll(); }
namespace Collections::LockFreeQueue { void TestAll(); }
namespace Collections::SCSP_RingBuffer { void TestAll(); }
namespace Collections::SCSP_RingBuffer_Blocking { void TestAll(); }
namespace Collections::SingleConsumerProducerQueue { void TestAll(); }
namespace Collections::ThreadSafeQueue_CV_vs_RingBuffer { void TestAll(); }
namespace Collections::RingBuffer_vs_CVMutexQueue { void TestAll(); };
namespace Collections::RingBuffer_vs_CVMutexQueue_2 { void TestAll(); };
namespace Collections::RingBuffer_vs_CVMutexQueue_Debug { void TestAll(); };

#endif //CPP_PROJECTS_MULTITHREADING_COLLECTIONS_H
