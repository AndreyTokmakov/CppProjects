
# Prefetching

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


