
=================================================================================================
                Setup GCC
=================================================================================================

export GCC_VERSION=13.1
export GCC_PATH=/home/andtokm/DiskS/Utils/bin/gcc-$GCC_VERSION

export PATH=${GCC_PATH}/bin:${PATH}
export LD_LIBRARY_PATH=${GCC_PATH}/lib64
export CC=gcc-$GCC_VERSION CXX=g++-$GCC_VERSION

=================================================================================================
                Compile and run
=================================================================================================

make clean all && ./test_bin

=================================================================================================
                Output
=================================================================================================



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
