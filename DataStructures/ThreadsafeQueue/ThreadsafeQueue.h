//============================================================================
// Name        : ThreadsafeQueue.h
// Author      : Tokmakov Andrey
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Queue
//============================================================================

#ifndef THREADSAFE_QUEUE_INCLUDE_GUARD
#define THREADSAFE_QUEUE_INCLUDE_GUARD

#include <iostream>
#include <deque>
#include <mutex>

#include <condition_variable>

namespace Queues::Multithreading {

	namespace Queues_On_Condition_Variable {

		template<typename T>
		class ThreadSafeQueue {
		private:
			mutable std::mutex mutex;
			std::deque<T> queue;
			std::condition_variable updated;

		public:
            ThreadSafeQueue() = default;

			void push(T&& new_value) noexcept
            {
				{
					std::lock_guard<std::mutex> lock(mutex);
					queue.push_back(std::move(new_value));
				}
				updated.notify_one();
			}

			template <typename... Args>
			void emplace(Args&& ... args) noexcept
            {
				{
					std::lock_guard<std::mutex> lock(mutex);
					queue.emplace_back(std::forward<Args>(args)...);
				}
				updated.notify_one();
			}

			/*template <typename... Args>
			void AddRange(Args&&... args) {
				std::lock_guard<std::mutex> lock(mtx);
				(elements.push_back(args), ...);
			}*/

			void wait_and_pop(T& value) noexcept
            {
				std::unique_lock<std::mutex> lock(mutex);
				updated.wait(lock, [this] {
					return false == queue.empty();
					});
				value = std::move(queue.front());
				queue.pop_front();
			}

			template<class _Rep, class _Period>
			bool wait_for_and_pop(T& value,
                                  const std::chrono::duration<_Rep, _Period>& _Rel_time) noexcept
            {
				std::unique_lock<std::mutex> lock(mutex);
				bool ok = updated.wait_for(lock, _Rel_time, [this] {
					return false == queue.empty();
                });
				if (!ok)
					return false;
				value = std::move(queue.front());
				queue.pop_front();
				return true;
			}

			T&& wait_and_pop() noexcept
            {
				std::unique_lock<std::mutex> lock(mutex);
				updated.wait(lock, [this] {
					return false == queue.empty();
					});
				auto&& entry = queue.front();
				queue.pop_front();
				return std::move(entry);
			}

			bool try_pop(T& value) noexcept {
				std::lock_guard<std::mutex> lock(mutex);
				if (queue.empty())
					return false;
				value = std::move(queue.front());
				queue.pop_front();
				return true;
			}

			std::shared_ptr<T> try_pop() {
				std::lock_guard<std::mutex> lock(mutex);
				if (queue.empty())
					return std::shared_ptr<T>();
				std::shared_ptr<T> result =
					std::make_shared<T>(std::move(queue.front()));
				queue.pop_front();
				return result;
			}

			bool empty() const noexcept {
				std::lock_guard<std::mutex> lock(mutex);
				return queue.empty();
			}

			bool size() const noexcept {
				std::lock_guard<std::mutex> lock(mutex);
				return queue.size();
			}
		};
	}


	namespace Queues_On_TimesMutex {

		template<typename T>
		class ThreadsafeQueue {
		private:
			mutable std::timed_mutex mutex;
			std::deque<T> qeque;

		public:
			ThreadsafeQueue() {
			}

			/*
			void push(T&& new_value) noexcept {
				std::lock_guard<std::mutex> lock(mutex);
				qeque.push_back(std::move(new_value));
			}
			*/

			template <typename... Args>
			void emplace(Args&& ... args) noexcept {
				mutex.lock();
				qeque.emplace_back(std::forward<Args>(args)...);
				mutex.unlock();
			}

			/*template <typename... Args>
			void AddRange(Args&&... args) {
				std::lock_guard<std::mutex> lock(mtx);
				(elements.push_back(args), ...);
			}*/

			void wait_and_pop(T& value) noexcept {
				mutex.lock();
				if (false == qeque.empty()) {
					value = std::move(qeque.front());
					qeque.pop_front();
				}
				mutex.unlock();
			}

			template<class _Rep, class _Period>
			bool wait_for_and_pop(T& value, const std::chrono::duration<_Rep, _Period>& _Rel_time) noexcept {
				if (true == mutex.try_lock_for(_Rel_time) && false == qeque.empty()) {
					value = std::move(qeque.front());
					qeque.pop_front();
					mutex.unlock();
					return true;
				}
				return false;
			}

			T&& wait_and_pop() noexcept {
				while (true) {
					if (true == mutex.try_lock_for(std::chrono::seconds(10)) && false == qeque.empty()) {
						auto&& entry = qeque.front();
						qeque.pop_front();
						mutex.unlock();
						return std::move(entry);
					}
				}
			}

			bool try_pop(T& value) noexcept {
				std::lock_guard<std::mutex> lock(mutex);
				if (qeque.empty())
					return false;
				value = std::move(qeque.front());
				qeque.pop_front();
				return true;
			}

			std::shared_ptr<T> try_pop() {
				std::lock_guard<std::mutex> lock(mutex);
				if (qeque.empty())
					return std::shared_ptr<T>();
				std::shared_ptr<T> result =
					std::make_shared<T>(std::move(qeque.front()));
				qeque.pop_front();
				return result;
			}

			bool empty() const noexcept {
				std::lock_guard<std::mutex> lock(mutex);
				return qeque.empty();
			}

			bool size() const noexcept {
				std::lock_guard<std::mutex> lock(mutex);
				return qeque.size();
			}
		};
	}

	void RunTests();
}

#endif /* THREADSAFE_QUEUE_INCLUDE_GUARD */