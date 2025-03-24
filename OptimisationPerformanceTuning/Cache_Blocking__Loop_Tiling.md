
# Cache Blocking (Loop Tiling)


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

