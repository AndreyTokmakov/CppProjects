/**============================================================================
Name        : Logging.cpp
Created on  : 22.05.2021
Author      : Tokmakov Andrey
Version     : 1.0
Copyright   : Your copyright notice
Description : C++ Logging lib
============================================================================**/


#define _CRT_SECURE_NO_WARNINGS

#include "Logger.h"

#include <iostream>
#include <string>
#include <chrono>
#include <fstream>

#include <vector>

#include <ratio>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <iomanip>
#include <utility>


namespace Logging {

	struct LogRecord final {
	private:
		std::time_t timestamp { std::time(nullptr) };
		std::string text;
		
		/*
		template<typename ... _Types>
		LogRecord(_Types&& ... params) : text(std::forward<_Types>(params)...) {
		}
		*/

	public:
		explicit LogRecord(std::string&& str): text(std::move(str)) {
		}

		LogRecord& operator=(const LogRecord& right) = delete;
		LogRecord(const LogRecord& obj) = delete;

		LogRecord& operator=(LogRecord&& right) noexcept {
			// std::cout << "[Move assignment operator] LogRecord" << std::endl;
			if (this != &right) {
				this->timestamp = std::exchange(right.timestamp, 0);
				this->text = std::move(right.text);
			}
			return *this;
		}

		LogRecord(LogRecord&& obj) noexcept :
            timestamp { std::exchange(obj.timestamp, 0) },
            text { std::move(obj.text) } {
			// std::cout << "[Move  contructor]\n]";
		}

		friend class Logger;

	};

	class Logger final
	{
	// private:
	public:
		std::thread dumperThread;

		std::mutex mtx;
		std::condition_variable cond_var;

		std::vector<LogRecord> logs;
		std::vector<LogRecord> logsToWrite;

		// Boolean telling the background thread to terminate.
		bool stopThread { false };

		inline static constexpr size_t STORAGE_SIZE {100};

		inline static constexpr std::chrono::milliseconds WAIT_TIMEOUT { 
			std::chrono::milliseconds(100) };

	protected:
		void logsDumper() noexcept 
		{
			// Open log file.
			std::ofstream logFile { R"(C:\Temp\test_run.log)" };
			if (logFile.fail()) {
				// TODO: Handle error
				std::cerr << "Failed to open logfile." << std::endl;
				return;
			}

			// Create a lock for mtx1, but do not yet acquire a lock on it.
			std::mutex mtxConditional;
			std::unique_lock<std::mutex> lock { mtxConditional };

			while (true) {
				// bool status = cond_var.wait_for(lock, WAIT_TIMEOUT, [&] {return not logsToWrite.empty(); });
				auto status = cond_var.wait_for(lock, WAIT_TIMEOUT);
				// TODO: do something with status???

				dumpLogs(logsToWrite, logFile);

				if (stopThread) {
					// Write logs left 
					dumpLogs(logs, logFile);
					return;
				}
			}
		}

		static void dumpLogs(std::vector<LogRecord>& logs,
                             std::ofstream& stream) noexcept {
			for (const auto& entry : logs)
				stream << std::put_time(std::gmtime(&entry.timestamp), "%c: ") << entry.text << '\n';
			logs.clear();
		}


	public:
		// Starts a background thread writing log entries to a file.
		Logger() {
			logs.reserve(STORAGE_SIZE);
			// Start background thread.
			dumperThread = std::thread{&Logger::logsDumper, this };
		}

		// Gracefully shut down background thread.
		~Logger() {
			if (!stopThread) {
				stop();
			}
		}

		// Prevent copy construction and assignment.
		Logger(const Logger& src) = delete;
		Logger& operator=(const Logger& rhs) = delete;

		void log(std::string&& entry) {
			std::lock_guard<std::mutex> lock { mtx };
			logs.emplace_back(std::move(entry));

            // FIXME: Or just check [ logs.size() > STORAGE_SIZE && logsToWrite.empty() ]
			if (0 == logs.size() % STORAGE_SIZE && logsToWrite.empty())
			{
				logs.swap(logsToWrite);
				cond_var.notify_all();
			}
		}

		void stop() {
			{
				// Gracefully and synchronized shut down the thread.
				std::lock_guard<std::mutex> lock{ mtx };
				// logsToWrite.insert(logsToWrite.end(), logs.begin(), logs.end());
				stopThread = true;
			}
			cond_var.notify_all();

			// Wait until thread is shut down. This should be outside the above code
			// block because the lock on m_mutex must be released before calling join()!
			dumperThread.join();
		}
	};
}


void Logging::TEST_ALL() {
	Logger logger;

	for (int i = 1; i <= 100'000; ++i) {
		logger.log(std::string("Log_Record" + std::to_string(i)));
		if (0 == i % 1000)
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}
	logger.stop();

}
