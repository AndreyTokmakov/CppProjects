
#  Loop Unrolling and Vectorization

Loop unrolling is a classic optimization where the loop body is repeated multiple times per iteration,
reducing the loop overhead (index increment, branch/jump for loop) and sometimes enabling further optimizations.
It often goes hand-in-hand with vectorization and instruction-level parallelism.

What is loop unrolling? It’s easier to show by example. Suppose we have a simple loop:

    // Simple loop                                  // Unrolled loop (by a factor of 4)
    for (int i = 0; i < N; ++i) {                   int i = 0;
        sum += arr[i];                              for (; i + 3 < N; i += 4) {
    }                                                   sum += arr[i];
                                                        sum += arr[i+1];
                                                        sum += arr[i+2];
                                                        sum += arr[i+3];
                                                    }
                                                    for (; i < N; ++i) {  // handle leftover
                                                        sum += arr[i];
                                                    }

Now the loop overhead (the check and increment) happens every 4 elements instead of every element.
This can reduce the number of branches and improve throughput.
Additionally, unrolling may expose more opportunities for the CPU to pipeline instructions or for the compiler
to schedule instructions better (hiding latencies).

# Bnefits:

1. Fewer branch instructions (the loop counter check/jump) per amount of work, which means less branch prediction
   to worry about and less pipeline disruption.

2. More straight-line code in the inner loop, which can be optimized better by the CPU’s instruction scheduler.
   It can also help with auto-vectorization because the compiler may more easily see how to use SIMD across
   the unrolled group of operations.

3. Possibility of combining with other optimizations: e.g., software pipelining or unroll-and-jam
   (unrolling an outer loop and intermixing operations from inner loops)


# Drawbacks:

1. Code size increase:
   The unrolled loop is longer in terms of machine code.
   This can bloat the instruction cache or even prevent the CPU’s loop buffer/micro-op cache from holding the
   whole loop efficiently
   If the loop body is large or the unroll factor is big, you might end up hurting performance by causing more instruction
   cache misses.
   Agner Fog’s optimization guide notes that excessive unrolling can overload the code cache and even defeat
   the loopback buffer on some CPU

2. If N is not a multiple of the unroll factor, you need extra handling for the leftover iterations (as shown above),
   which adds a bit of complexity.

3. Diminishing returns:
   Unrolling beyond a certain point gives no benefit and can even hurt.
   Often unrolling by 2x or 4x is enough.
   Compilers sometimes unroll small loops automatically at higher optimization levels, so manual unrolling might be redundant.

