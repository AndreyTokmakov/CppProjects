//============================================================================
// Name        : ThreadsafeQueue.h
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Queue
//============================================================================

#include <iostream>
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds
#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <stdexcept>
#include <algorithm>
#include <queue>
#include <deque>

#include "../Utilities/Integer.h"
#include "ThreadsafeQueue.h"
#include "../Utilities/ThreadHelperUtilities.h"


namespace Queues::PerformanceTests {

#define START_TIME_MEASURE auto start = std::chrono::high_resolution_clock::now();
#define STOP_TIME_MEASURE  { auto end = std::chrono::high_resolution_clock::now(); \
                           auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count(); \
						   std::cout << "Result: " << duration << " microseconds" << std::endl;}
}

namespace Queues::Multithreading 
{

	void RunTest1() {
		Queues_On_Condition_Variable::ThreadsafeQueue<Integer> queue;

		std::future<void> producer = std::async(std::launch::async, [&]()-> void {
			THREAD_INFO << "Producer: started." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			queue.emplace(1);
			THREAD_INFO << "Producer: done" << std::endl;
		});

		std::future<void> consumer = std::async(std::launch::async, [&]()-> void {
			THREAD_INFO << "Consumer: started" << std::endl;
			auto&& entry = queue.wait_and_pop();
			THREAD_INFO << "Consumer: We've got some" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));
			entry.printInfo();
			THREAD_INFO << "Consumer: done" << std::endl;
		});

		producer.wait();
		consumer.wait();

		THREAD_INFO << "Done!!" << std::endl;
	}

	void RunTest_WairFor() {
		Queues_On_Condition_Variable::ThreadsafeQueue<Integer> queue;

		std::future<void> producer = std::async(std::launch::async, [&]()-> void {
			THREAD_INFO << "Producer: started." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			queue.emplace(1);
			THREAD_INFO << "Producer: done" << std::endl;
		});

		std::future<void> consumer = std::async(std::launch::async, [&]()-> void {
			THREAD_INFO << "Consumer: started" << std::endl;
			Integer entry;
			while (true)
			{
				auto ok = queue.wait_for_and_pop(entry, std::chrono::milliseconds(250));
				if (ok) {
					THREAD_INFO << "Consumer: We've got some" << std::endl;
					break;
				}
				else {
					THREAD_INFO << "Timeout" << std::endl;
				}

			}			std::this_thread::sleep_for(std::chrono::seconds(2));
			entry.printInfo();
			THREAD_INFO << "Consumer: done" << std::endl;
		});

		producer.wait();
		consumer.wait();

		THREAD_INFO << "Done!!" << std::endl;
	}

	void Perf_Test_1() {
		Queues_On_Condition_Variable::ThreadsafeQueue<size_t> queue;
		std::vector<std::future<void>> workers;

		constexpr size_t MAX_VALUE {10'000'000};
		constexpr size_t PRODUCER_COUNT {10};

		START_TIME_MEASURE;

		for (size_t n = 0; n < PRODUCER_COUNT; ++n) {
			workers.emplace_back(std::async([&queue, n] {
				for (size_t i = MAX_VALUE * n; i <= MAX_VALUE * (n + 1); ++i) {
					queue.emplace(i);
				}
			}));
		}

		workers.emplace_back(std::async([&] {
			std::cout << "Consumer started\n";
			size_t entry{ 0 };
			constexpr long TARGET_VALUE { MAX_VALUE * PRODUCER_COUNT };

			while (TARGET_VALUE != entry) {
				auto ok = queue.wait_for_and_pop(entry, std::chrono::milliseconds(1));
			}
			std::cout << "Consumer done. Last read value: " << entry << "\n";
		}));

		std::for_each(workers.cbegin(), workers.cend(), [](const auto& Task) {Task.wait(); });

		STOP_TIME_MEASURE;
	}

	//-------------------------------------------------------------------------------------

	void RunTest2() {
		Queues_On_TimesMutex::ThreadsafeQueue<Integer> queue;

		std::future<void> producer = std::async(std::launch::async, [&]()-> void {
			THREAD_INFO << "Producer: started." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			queue.emplace(1);
			THREAD_INFO << "Producer: done" << std::endl;
			});

		std::future<void> consumer = std::async(std::launch::async, [&]()-> void {
			THREAD_INFO << "Consumer: started" << std::endl;
			auto&& entry = queue.wait_and_pop();
			THREAD_INFO << "Consumer: We've got some" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));
			entry.printInfo();
			THREAD_INFO << "Consumer: done" << std::endl;
			});

		producer.wait();
		consumer.wait();

		THREAD_INFO << "Done!!" << std::endl;
	}

	//-------------------------------------------------------------------------------------

	void RunTests()
	{
		RunTest1();
		// RunTest_WairFor();

		// Perf_Test_1();
	}
}