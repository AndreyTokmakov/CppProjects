//============================================================================
// Name        : ThreadsafeQueue.h
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Queue
//============================================================================

#include <iostream>
#include <syncstream>
#include <future>
#include <chrono>
#include <string>
#include <vector>
#include <thread>
#include <algorithm>
#include <queue>

#include "../Utilities/Integer.h"
#include "PerfUtilities.hpp"
#include "ThreadsafeQueue.h"
#include "DateTimeUtilities.hpp"

#define LOG  std::osyncstream { std::cout } << DateTimeUtilities::getCurrentTime() << " "

namespace Queues::Multithreading 
{

	void RunTest1() {
		Queues_On_Condition_Variable::ThreadSafeQueue<Integer> queue;

		std::future<void> producer = std::async(std::launch::async, [&]()-> void {
			LOG << "Producer: started." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			queue.emplace(1);
			LOG << "Producer: done" << std::endl;
		});

		std::future<void> consumer = std::async(std::launch::async, [&]()-> void {
			LOG << "Consumer: started" << std::endl;
			auto&& entry = queue.wait_and_pop();
			LOG << "Consumer: We've got some" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));
			entry.printInfo();
			LOG << "Consumer: done" << std::endl;
		});

		producer.wait();
		consumer.wait();

		LOG << "Done!!" << std::endl;
	}

	void RunTest_WairFor() {
		Queues_On_Condition_Variable::ThreadSafeQueue<Integer> queue;

		std::future<void> producer = std::async(std::launch::async, [&]()-> void {
			LOG << "Producer: started." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			queue.emplace(1);
			LOG << "Producer: done" << std::endl;
		});

		std::future<void> consumer = std::async(std::launch::async, [&]()-> void {
			LOG << "Consumer: started" << std::endl;
			Integer entry;
			while (true)
			{
				auto ok = queue.wait_for_and_pop(entry, std::chrono::milliseconds(250));
				if (ok) {
					LOG << "Consumer: We've got some" << std::endl;
					break;
				}
				else {
					LOG << "Timeout" << std::endl;
				}

			}			std::this_thread::sleep_for(std::chrono::seconds(2));
			entry.printInfo();
			LOG << "Consumer: done" << std::endl;
		});

		producer.wait();
		consumer.wait();

		LOG << "Done!!" << std::endl;
	}

	void Perf_Test_1()
	{
		Queues_On_Condition_Variable::ThreadSafeQueue<size_t> queue;
		std::vector<std::future<void>> workers;

		constexpr size_t MAX_VALUE {10'000'000};
		constexpr size_t PRODUCER_COUNT {10};

		const PerfUtilities::ScopedTimer timer { "Perf_Test_1" };
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
				[[maybe_unused]] auto ok = queue.wait_for_and_pop(entry, std::chrono::milliseconds(1));
			}
			std::cout << "Consumer done. Last read value: " << entry << "\n";
		}));

		std::for_each(workers.cbegin(), workers.cend(), [](const auto& Task) {Task.wait(); });
	}

	//-------------------------------------------------------------------------------------

	void RunTest2()
	{
		Queues_On_TimesMutex::ThreadsafeQueue<Integer> queue;

		const std::future<void> producer = std::async(std::launch::async, [&]()-> void {
			LOG << "Producer: started." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(1));
			queue.emplace(1);
			LOG << "Producer: done" << std::endl;
		});

		const std::future<void> consumer = std::async(std::launch::async, [&]()-> void {
			LOG << "Consumer: started" << std::endl;
			auto&& entry = queue.wait_and_pop();
			LOG << "Consumer: We've got some" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(2));
			entry.printInfo();
			LOG << "Consumer: done" << std::endl;
		});

		producer.wait();
		consumer.wait();

		LOG << "Done!!" << std::endl;
	}

	//-------------------------------------------------------------------------------------

	void RunTests()
	{
		RunTest1();
		// RunTest_WairFor();

		// Perf_Test_1();
	}
}