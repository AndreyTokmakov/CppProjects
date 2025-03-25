### Lock-Free and Atomic Operations

Locks (mutexes) provide mutual exclusion but can become a scalability issue if many threads contend on the same lock.</br>
The context switch or spinning costs can add up, and if a lock is heavily contended,
threads spend time waiting (which is wasted CPU time).</br>
Lock-free programming aims to avoid traditional locks by using atomic operations and careful algorithms so that 
threads can progress without blocking each other.</br>

C++ offers atomic types (`std::atomic<T>`) which support operations like `load`, `store`, `fetch_add`, etc. atomically
(often implemented via lock-free CPU instructions).

For example, a simple counter increment across threads:

```
std::atomic<int> counter{0};
// Each thread does:
for (int i = 0; i < 1000000; ++i) {
    counter.fetch_add(1, std::memory_order_relaxed);
}
```

This will run in parallel without a mutex. </br>
Under the hood, on x86 this fetch_add might use a `LOCK XADD` instruction which locks the memory bus for an instant to do the atomic add.</br>
It’s still not free (atomic ops have a cost, especially if many CPUs try to update the same variable, they serialize), but it avoids the overhead of a mutex and thread scheduling. 
Lock-free data structures (like queues, stacks, etc.) use atomic primitives (compare-and-swap, etc.) to coordinate multiple threads without explicit locks. </br>
They can achieve much higher concurrency, but are tricky to implement correctly.</br>

<b><u>When to use lock-free</b></u></br>
In high-performance scenarios where a mutex is a bottleneck. </br>
For example, a logging system where many threads produce log messages could use a lock-free queue to enqueue messages instead 
of a locked queue, to reduce contention.</br>
Or in real-time systems where blocking is unacceptable.</br>
Always weigh complexity vs benefit; lock-free algorithms can have subtle bugs (like ABA problem, memory reclamation issues).</br>
Sometimes a well-designed sharded locking (reducing contention by having multiple locks) or using higher-level frameworks 
(like concurrent queues from TBB or folly) is easier.