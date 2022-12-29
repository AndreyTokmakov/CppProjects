/**============================================================================
Name        : EventLoop.h
Created on  : 21.12.2022.
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description : EventLoop
============================================================================**/

#include "EventLoop.h"

#include <iostream>
#include <functional>
#include <thread>
#include <condition_variable>

namespace EventLoop
{
    class EventLoop
    {
    public:
        using callable_t = std::function<void()>;

        EventLoop() = default;
        EventLoop(const EventLoop&) = delete;
        EventLoop(EventLoop&&) noexcept = delete;

        ~EventLoop() noexcept {
            enqueue([this]{
                m_running = false;
            });
            m_thread.join();
        }

        EventLoop& operator= (const EventLoop&) = delete;
        EventLoop& operator= (EventLoop&&) noexcept = delete;

        void enqueue(callable_t&& callable) noexcept
        {
            {
                std::lock_guard<std::mutex> guard(m_mutex);
                m_writeBuffer.emplace_back(std::move(callable));
            }
            m_condVar.notify_one();
        }

    private:
        std::vector<callable_t> m_writeBuffer;
        std::mutex m_mutex;
        std::condition_variable m_condVar;
        bool m_running { true }; // TODO: make it atomic??
        std::thread m_thread { &EventLoop::threadFunc, this };

        void threadFunc() noexcept
        {
            std::vector<callable_t> localCopy;
            while (m_running) {
                if (std::unique_lock<std::mutex> lock(m_mutex); true) {
                    m_condVar.wait(lock, [this] { return !m_writeBuffer.empty(); });
                    std::swap(localCopy, m_writeBuffer);
                }

                std::for_each(localCopy.cbegin(), localCopy.cend(), [](auto& func) {
                    func();
                });
                localCopy.clear();
            }
        }
    };
}

void EventLoop::TestAll()
{
    EventLoop eventLoop;
    eventLoop.enqueue([]{std::cout << "message from a different thread 1\n"; });
    eventLoop.enqueue([]{std::cout << "message from a different thread 2\n"; });
}