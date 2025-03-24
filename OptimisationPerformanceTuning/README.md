# OptimisationPerformanceTuning

0. [Overview](#Overview)
2. [Links](#Tests)

## Overview
C++ Optimisation and Performance Tuning experiments



<a name="Tests"></a>
## Some links to the external resources:
1. Performance Benefits of Using Huge Pages for Code.
    https://easyperf.net/blog/2022/09/01/Utilizing-Huge-Pages-For-Code
2. Virtual Function True price:
   https://johnnysswlab.com/the-true-price-of-virtual-functions-in-c/
3. Measuring Memory Subsystem Performance
   https://johnnysswlab.com/measuring-memory-subsystem-performance/
4. Crash course introduction to parallelism: Multithreading
   https://johnnysswlab.com/crash-course-introduction-to-parallelism-multithreading/
5. 2-minute read: Class Size, Member Layout and Speed
   https://johnnysswlab.com/2-minute-read-class-size-member-layout-and-speed/
6. Excessive copying in C++ and your program’s speed
   https://johnnysswlab.com/excessive-copying-in-c-and-your-programs-speed/




================================================================================================================
Prefetching
================================================================================================================

Prefetching
Even with good locality, sometimes you access patterns that the hardware prefetcher 
   (which tries to fetch upcoming cache lines automatically) might not anticipate.
Prefetching is an optimization where you explicitly load data into cache beforeyou need it, to overlap memory latency with computation.
In C++, you can use compiler intrinsics like __builtin_prefetch() (GCC/Clang) or platform-specific 
   functions to hint that a certain memory address will be needed soon. For example:


    for (size_t i = 0; i < N; ++i) {
        if (i + 16 < N) {
            __builtin_prefetch(&array[i + 16], 0, 1);  // preload data 16 elements ahead
        }
        process(array[i]);
    }

================================================================================================================
Cache Blocking (Loop Tiling)
================================================================================================================


Cache blocking (also known as loop tiling) is a technique to improve reuse of data in caches by working on 
subsets of data that fit into the cache.
When an algorithm accesses a large data set with multiple loops, it might repeatedly bring data in and out of the cache.
By blocking, we divide the problem into chunks that can stay in cache during computation, thus reducing memory bandwidth usage

Example Suppose we have two arrays and we want to do something with every pair of elements.
Instead of iterating one huge loop, we can process in blocks:

    const int BLOCK = 1024;
    for (int start = 0; start < N; start += BLOCK) {
        int end = std::min(start + BLOCK, N);
        for (int i = start; i < end; ++i) {
            result[i] += compute(A[i], B[i]);
        }
    }