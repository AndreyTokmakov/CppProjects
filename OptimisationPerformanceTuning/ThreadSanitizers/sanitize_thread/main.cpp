/**============================================================================
Name        : main.cpp
Created on  : 23.12.2023
Author      : Andrei Tokmakov
Version     : 1.0
Copyright   : Your copyright notice
Description :
============================================================================**/

#include <iostream>
#include <vector>
#include <thread>

// INFO : https://github.com/CoffeeBeforeArch/CoffeeBeforeArch.github.io/blob/master/_posts/2020-08-04-atomic-vs-mutex.md


int main()
{
    constexpr uint32_t interCount = 100'000, jobsCount = 8;
    int sharedVariable = 0;  /** Shared value for our threads **/

    // std::mutex mtx;
    auto incrementor = [&]() {
        for (uint32_t i = 0; i < interCount; i++)
        {
            // std::lock_guard<std::mutex> lock {mtx};
            sharedVariable++;
        }
    };

    {
        std::vector<std::jthread> jobs;
        for (uint32_t id = 0; id < jobsCount; id++)
            jobs.emplace_back(incrementor);
    }

    std::cout << "Final result: " << sharedVariable << '\n';
    return EXIT_SUCCESS;
}


/**
==================
WARNING: ThreadSanitizer: data race (pid=30168)
  Read of size 4 at 0x7ffd2ae4fdf8 by thread T2:
    #0 operator() /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/ThreadSanitizers/sanitize_thread/main.cpp:21 (test_bin+0x401294)
    #1 __invoke_impl<void, main()::<lambda()> > /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/invoke.h:61 (test_bin+0x4018ed)
    #2 __invoke<main()::<lambda()> > /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/invoke.h:96 (test_bin+0x401874)
    #3 _M_invoke<0> /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:292 (test_bin+0x4017da)
    #4 operator() /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:299 (test_bin+0x401784)
    #5 _M_run /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:244 (test_bin+0x40173e)
    #6 <null> <null> (libstdc++.so.6+0xe8b42)

  Previous write of size 4 at 0x7ffd2ae4fdf8 by thread T1:
    #0 operator() /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/ThreadSanitizers/sanitize_thread/main.cpp:21 (test_bin+0x4012a2)
    #1 __invoke_impl<void, main()::<lambda()> > /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/invoke.h:61 (test_bin+0x4018ed)
    #2 __invoke<main()::<lambda()> > /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/invoke.h:96 (test_bin+0x401874)
    #3 _M_invoke<0> /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:292 (test_bin+0x4017da)
    #4 operator() /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:299 (test_bin+0x401784)
    #5 _M_run /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:244 (test_bin+0x40173e)
    #6 <null> <null> (libstdc++.so.6+0xe8b42)

  Location is stack of main thread.

  Location is global '<null>' at 0x000000000000 ([stack]+0x1edf8)

  Thread T2 (tid=30171, running) created by main thread at:
    #0 pthread_create <null> (libtsan.so.2+0x40cd6)
    #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xe8ebb)
    #2 main /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/ThreadSanitizers/sanitize_thread/main.cpp:26 (test_bin+0x40132b)

  Thread T1 (tid=30170, running) created by main thread at:
    #0 pthread_create <null> (libtsan.so.2+0x40cd6)
    #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xe8ebb)
    #2 main /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/ThreadSanitizers/sanitize_thread/main.cpp:25 (test_bin+0x401318)

SUMMARY: ThreadSanitizer: data race /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/ThreadSanitizers/sanitize_thread/main.cpp:21 in operator()
==================
==================
WARNING: ThreadSanitizer: data race (pid=30168)
  Write of size 4 at 0x7ffd2ae4fdf8 by thread T2:
    #0 operator() /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/ThreadSanitizers/sanitize_thread/main.cpp:21 (test_bin+0x4012a2)
    #1 __invoke_impl<void, main()::<lambda()> > /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/invoke.h:61 (test_bin+0x4018ed)
    #2 __invoke<main()::<lambda()> > /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/invoke.h:96 (test_bin+0x401874)
    #3 _M_invoke<0> /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:292 (test_bin+0x4017da)
    #4 operator() /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:299 (test_bin+0x401784)
    #5 _M_run /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:244 (test_bin+0x40173e)
    #6 <null> <null> (libstdc++.so.6+0xe8b42)

  Previous write of size 4 at 0x7ffd2ae4fdf8 by thread T1:
    #0 operator() /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/ThreadSanitizers/sanitize_thread/main.cpp:21 (test_bin+0x4012a2)
    #1 __invoke_impl<void, main()::<lambda()> > /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/invoke.h:61 (test_bin+0x4018ed)
    #2 __invoke<main()::<lambda()> > /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/invoke.h:96 (test_bin+0x401874)
    #3 _M_invoke<0> /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:292 (test_bin+0x4017da)
    #4 operator() /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:299 (test_bin+0x401784)
    #5 _M_run /home/andtokm/DiskS/Utils/bin/gcc-13.1/include/c++/13.1.0/bits/std_thread.h:244 (test_bin+0x40173e)
    #6 <null> <null> (libstdc++.so.6+0xe8b42)

  Location is stack of main thread.

  Location is global '<null>' at 0x000000000000 ([stack]+0x1edf8)

  Thread T2 (tid=30171, running) created by main thread at:
    #0 pthread_create <null> (libtsan.so.2+0x40cd6)
    #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xe8ebb)
    #2 main /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/ThreadSanitizers/sanitize_thread/main.cpp:26 (test_bin+0x40132b)

  Thread T1 (tid=30170, running) created by main thread at:
    #0 pthread_create <null> (libtsan.so.2+0x40cd6)
    #1 std::thread::_M_start_thread(std::unique_ptr<std::thread::_State, std::default_delete<std::thread::_State> >, void (*)()) <null> (libstdc++.so.6+0xe8ebb)
    #2 main /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/ThreadSanitizers/sanitize_thread/main.cpp:25 (test_bin+0x401318)

SUMMARY: ThreadSanitizer: data race /home/andtokm/DiskS/ProjectsUbuntu/CppProjects/OptimisationPerformanceTuning/ThreadSanitizers/sanitize_thread/main.cpp:21 in operator()
==================
FINAL VALUE IS: 131072
ThreadSanitizer: reported 2 warnings

 */
